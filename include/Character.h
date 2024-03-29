//
// Created by Jana Trpkovska on 16.1.2024.
//

#ifndef OPENGLPRJ_CHARACTER_H
#define OPENGLPRJ_CHARACTER_H

#include <glm/glm.hpp>

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

#endif //OPENGLPRJ_CHARACTER_H
