
#include <Maze.h>
#include <InputManager.h>
#include <TextRenderer.h>
#include <Renderer.h>

int main() {

    //initialize glfw and shaders
    Initialize();

    //initialize text renderer
    InitializeTextRenderer();

    //initialize vertex and light data
    SetUpVertexData();

    //load textures
    SetUpTextures();

    //initialize maze
    setupMaze();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {

        //render full scene
        RenderScene();
    }

    //de-allocate all resources
    Cleanup();
    CleanupTextRenderer();

    // glfw: terminate.
    glfwTerminate();
    return 0;
}
