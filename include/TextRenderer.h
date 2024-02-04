//
// Created by Jana Trpkovska on 04.2.2024.
//

#ifndef OPENGLPRJ_TEXTRENDERER_H
#define OPENGLPRJ_TEXTRENDERER_H
#pragma once

#include <map>
#include <vector>

#include <Shader.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Character.h>

extern std::map<GLchar, Character> Characters;
extern unsigned int textVAO, textVBO;

void InitializeTextRenderer();
void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color);
float CalculateTextWidth(std::string text, float scale);
void CleanupTextRenderer();

#endif //OPENGLPRJ_TEXTRENDERER_H
