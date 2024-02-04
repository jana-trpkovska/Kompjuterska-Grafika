//
// Created by Jana Trpkovska on 04.2.2024.
//

#ifndef OPENGLPRJ_RENDERER_H
#define OPENGLPRJ_RENDERER_H
#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>

#include <Shader.hpp>

extern const std::string program_name;
extern GLFWwindow *window;

//shaders
extern Shader ourShader;
extern Shader textShader;
extern Shader lampShader;

//vertex data
extern float vertices[];
extern unsigned int VBO, VAO;
extern unsigned int lightVAO;

//textures
extern unsigned int texture1, texture2, texture3;

//timing
extern float lastFrame;

//random cubes
extern unsigned int collectedCubes;

//Timer
extern float timer; //in seconds
extern bool gameWon;
extern bool gameLost;

//lamp position
extern glm::vec3 lightPos;

void Initialize();
void SetUpVertexData();
void SetUpTextures();
unsigned int loadTexture(const char* path);
void SetUpLightParameters();
void RenderScene();
void Cleanup();


#endif //OPENGLPRJ_RENDERER_H