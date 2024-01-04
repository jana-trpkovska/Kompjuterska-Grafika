//
// Created by Jana Trpkovska on 24.12.2023.
//

#ifndef OPENGLPRJ_MAZE_H
#define OPENGLPRJ_MAZE_H

#include <vector>
#include <stack>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <glm/glm.hpp>

constexpr unsigned mazeWidth = 5, mazeHeight = 5;
constexpr float wallSize = 0.1f;
std::vector<glm::vec3> generateMaze();

#endif //OPENGLPRJ_MAZE_H