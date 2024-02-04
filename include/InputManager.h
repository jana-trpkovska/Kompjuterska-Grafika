//
// Created by Jana Trpkovska on 04.2.2024.
//

#ifndef OPENGLPRJ_INPUTMANAGER_H
#define OPENGLPRJ_INPUTMANAGER_H
#pragma once

#include <GLFW/glfw3.h>
#include <Camera.hpp>

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

extern Camera camera;
extern float lastX;
extern float lastY;
extern bool firstMouse;
extern float deltaTime;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

#endif //OPENGLPRJ_INPUTMANAGER_H