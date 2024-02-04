//
// Created by Jana Trpkovska on 24.12.2023.
//

#ifndef OPENGLPRJ_MAZE_H
#define OPENGLPRJ_MAZE_H
#pragma once

#include <vector>
#include <stack>
#include <algorithm>
#include <iostream>
#include <random>
#include <cstdlib>
#include <glm/glm.hpp>

#include <Cube.h>

constexpr unsigned mazeWidth = 10, mazeHeight = 10;
constexpr float wallSize = 0.1f;
const unsigned int maxCubes = 5;
extern std::vector<Cube> level;

std::vector<glm::vec3> generateMaze();
std::vector<Cube> convertMazeToWorld(std::vector<glm::vec3> maze);
std::vector<glm::vec3> generateRandomCubePositions(const std::vector<glm::vec3>& maze);
void setupMaze();

#endif //OPENGLPRJ_MAZE_H