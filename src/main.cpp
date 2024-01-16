#include <OpenGLPrj.hpp>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

#include <Camera.hpp>
#include <Shader.hpp>

#include <Maze.h>
#include <Cube.h>
#include <Character.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <random>

const std::string program_name = ("3D Maze Game");

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color);


static std::vector<Cube> convertMazeToWorld(std::vector<glm::vec3> maze);
std::vector<glm::vec3> generateRandomCubePositions(const std::vector<glm::vec3>& maze);
float CalculateTextWidth(std::string text, float scale);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
static Camera camera(glm::vec3(0.0f, 1.1f, 3.0f));
static float lastX = SCR_WIDTH / 2.0f;
static float lastY = SCR_HEIGHT / 2.0f;
static bool firstMouse = true;

// timing
static float deltaTime = 0.0f; // time between current frame and last frame
static float lastFrame = 0.0f;
unsigned int collectedCubes = 0;

// Timer variables
float timer = 120.0f; //in seconds
bool gameWon = false;
bool gameLost = false;

std::vector<Cube> level;
const unsigned int maxCubes = 5; // Maximum number of cubes to place

std::map<GLchar, Character> Characters;
unsigned int textVAO, textVBO;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(
      GLFW_OPENGL_FORWARD_COMPAT,
      GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                                          program_name.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile our shader program
    // ------------------------------------
    std::string shader_location("../res/shaders/");

    std::string used_shaders("shader");
    std::string text_shaders("text");

    Shader ourShader(shader_location + used_shaders + std::string(".vert"),
                     shader_location + used_shaders + std::string(".frag"));

    Shader textShader(shader_location + text_shaders + std::string(".vert"),
                      shader_location + text_shaders + std::string(".frag"));

    glm::mat4 projection_text = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection_text));

    // FreeType
    // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    // find path to font
    std::string font_name = "../../../fonts/arial.ttf";
    if (font_name.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
        return -1;
    }

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // configure textVAO/textVBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // load and create a texture
    // -------------------------
    unsigned int texture1, texture2, texture3;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
            GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(
            true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can
    // find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load("../res/textures/floor_texture.jpg", &width,
                                    &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
            GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("../res/textures/wood.jpg", &width, &height,
                     &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 3
    // ---------
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    // set the texture wrapping parameters
    glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
            GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("../res/textures/gradient.jpg", &width, &height,
                     &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has
    // to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();
    ourShader.setInt("material.diffuse", 0);
    ourShader.setInt("material.specular", 1);

    for (unsigned i=0;i<mazeWidth;i++) {
        for(unsigned j=0;j<mazeHeight;j++){
            level.push_back({glm::vec3(i,0,j), glm::vec3(1)});
        }
    }

    std::vector<glm::vec3> initMaze = generateMaze();
    initMaze.erase(std::remove(initMaze.begin(), initMaze.end(), glm::vec3(0, 0, 1)), initMaze.end());
    initMaze.erase(std::remove(initMaze.begin(), initMaze.end(), glm::vec3(mazeWidth - 1, mazeHeight - 1, 3)), initMaze.end());
    std::vector<Cube> maze = convertMazeToWorld(initMaze);
    level.reserve(level.size() + maze.size());
    level.insert(level.end(), maze.begin(), maze.end());

    // Generate random positions for the cubes within the maze
    std::vector<glm::vec3> cubePositions = generateRandomCubePositions(initMaze);

    // Ensure there are at least 5 cubes placed in free spaces
    for (const auto& cubePos : cubePositions) {
        // Create cubes at random positions within the maze
        level.push_back({ cubePos + glm::vec3(0.0f, 0.8f, 0.0f), glm::vec3(0.2f) }); // Adjust the height of the cubes
    }

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        timer -= deltaTime;

        if (timer <= 0.0f && collectedCubes < maxCubes) {
            gameLost = true;
            timer = 0.0f;
        }

        if (collectedCubes == maxCubes) {
            gameWon = true;
            timer = 0.0f;
        }

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (gameWon) {
            float textWidth = CalculateTextWidth("WINNER", 2.0f);
            RenderText(textShader, "WINNER", (SCR_WIDTH - textWidth) / 2.0f, SCR_HEIGHT / 2.0f, 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        } else if (gameLost) {
            float textWidth = CalculateTextWidth("GAME OVER", 2.0f);
            RenderText(textShader, "GAME OVER", (SCR_WIDTH - textWidth) / 2.0f, SCR_HEIGHT / 2.0f, 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        }

        RenderText(textShader, "Time left: " + std::to_string((int)timer) + "s", SCR_WIDTH - 200.0f, SCR_HEIGHT - 50.0f, 0.5f, glm::vec3(1.0f));
        RenderText(textShader, "Collected Cubes: " + std::to_string(collectedCubes), 20.0f, SCR_HEIGHT - 50.0f, 0.5f, glm::vec3(1.0f));

        // activate shader
        ourShader.use();
        ourShader.setVec3("light.position", camera.Position);
        ourShader.setVec3("light.direction", camera.Front);
        ourShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
        ourShader.setVec3("viewPos", camera.Position);

        // light properties
        ourShader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
        // we configure the diffuse intensity slightly higher; the right lighting conditions differ with each lighting method and environment.
        // each environment and lighting type requires some tweaking to get the best out of your environment.
        ourShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("light.constant", 1.0f);
        ourShader.setFloat("light.linear", 0.09f);
        ourShader.setFloat("light.quadratic", 0.032f);

        // material properties
        ourShader.setFloat("material.shininess", 32.0f);


        // pass projection matrix to shader (note that in this case it could change
        // every frame)
        glm::mat4 projection = glm::perspective(
                glm::radians(camera.Zoom), SCR_WIDTH * 1.0f / SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        // render boxes
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < level.size(); i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, level[i].position);
            model = glm::scale(model, level[i].size);
            ourShader.setMat4("model", model);

            if (i < mazeHeight * mazeWidth) {
                // Set the floor texture
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture1);
            } else if (i >= mazeHeight * mazeWidth && i < level.size() - maxCubes) {
                // Set the wall texture
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture2);
            } else {
                // Set texture for the cubes
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture3);
            }

            if (i >= level.size() - maxCubes) {
                // Render only the last five cubes
                glDrawArrays(GL_TRIANGLES, 0, 36);

                if (glm::distance(camera.Position, level[i].position) < 0.5f) {
                    level.erase(level.begin() + i);
                    collectedCubes++;
                    std::cout<<collectedCubes<<std::endl;
                }
            } else {
                // Render walls and other elements
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
        // etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime,level);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime,level);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime,level);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime,level);
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposd, double yposd) {
    float xpos = static_cast<float>(xposd);
    float ypos = static_cast<float>(yposd);
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset =
            lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// ---------------------------------------------------------------------------------------------------------
static std::vector<Cube> convertMazeToWorld(std::vector<glm::vec3> maze){
    std::vector<Cube> result;

    for (glm::vec3 m : maze) {
        if ((unsigned)m.z == 0){
            result.push_back({glm::vec3(m.x - 0.5f + wallSize/2, 1, m.y), glm::vec3(wallSize, 1, 1)});
        } else if ((unsigned)m.z == 1){
            result.push_back({glm::vec3(m.x, 1, m.y - 0.5f + wallSize/2), glm::vec3(1, 1, wallSize)});
        } else if ((unsigned)m.z == 2){
            result.push_back({glm::vec3(m.x + 0.5f - wallSize/2, 1, m.y), glm::vec3(wallSize, 1, 1)});
        } else if ((unsigned)m.z == 3){
            result.push_back({glm::vec3(m.x, 1, m.y + 0.5f - wallSize/2), glm::vec3(1, 1, wallSize)});
        }
    }

    // Logic to place cubes within free spaces of the maze
    unsigned int cubesPlaced = 0;

    // Iterate through maze coordinates and check for free spaces to place cubes
    for (glm::vec3 m : maze) {
        bool isFreeSpace = true;

        // Check if the current position in the maze is a free space (not a wall or existing cube)
        for (const Cube& cube : result) {
            if (glm::distance(glm::vec2(m.x, m.y), glm::vec2(cube.position.x, cube.position.z)) < 0.1f) {
                isFreeSpace = false; // Found a cube or wall nearby
                break;
            }
        }

        if (isFreeSpace && cubesPlaced < maxCubes) {
            // Place a cube at the current free position within the maze
            result.push_back({ glm::vec3(m.x, 0.3f, m.y), glm::vec3(0.2f) }); // Adjust the size and height of the cubes as needed
            cubesPlaced++;
        }

        if (cubesPlaced >= maxCubes) {
            break; // Placed the maximum number of cubes
        }
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------------
std::vector<glm::vec3> generateRandomCubePositions(const std::vector<glm::vec3>& maze) {
    std::vector<glm::vec3> freeSpaces; // Store free positions within the maze

    // Find free spaces where cubes can be placed
    for (unsigned i = 0; i < mazeWidth; ++i) {
        for (unsigned j = 0; j < mazeHeight; ++j) {
            glm::vec3 currentPosition = glm::vec3(i, 0, j);

            // Check if the current position is a free space (not a wall)
            if (std::find(maze.begin(), maze.end(), currentPosition) == maze.end()) {
                freeSpaces.push_back(currentPosition); // Add free position to the list
            }
        }
    }

    // Shuffle the free spaces to randomly choose positions for cubes
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(freeSpaces.begin(), freeSpaces.end(), gen);

    // Ensure there are at least 5 free spaces available to place cubes
    unsigned numCubes = std::min(static_cast<unsigned>(5), static_cast<unsigned>(freeSpaces.size()));

    // Return a subset of free positions where cubes can be placed
    return std::vector<glm::vec3>(freeSpaces.begin(), freeSpaces.begin() + numCubes);
}

// ---------------------------------------------------------------------------------------------------------
void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state
    shader.use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// ---------------------------------------------------------------------------------------------------------
float CalculateTextWidth(std::string text, float scale) {
    float width = 0.0f;
    for (char c : text) {
        Character ch = Characters[c];
        width += (ch.Advance >> 6) * scale;
    }
    return width;
}