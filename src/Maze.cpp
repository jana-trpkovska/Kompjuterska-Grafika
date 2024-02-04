//
// Created by Jana Trpkovska on 24.12.2023.
//

#include <Maze.h>

std::vector<Cube> level;

static void initializeWalls(std::vector<glm::vec3>& array){
    for(unsigned i=0;i<mazeWidth;i++){
        for(unsigned j=0;j<mazeHeight;j++){
            for(unsigned orientation=0;orientation<4;orientation++){
                array.push_back(glm::vec3(i,j,orientation));
            }
        }
    }
}

static bool getCell(int x, int y, bool cells[][mazeHeight]){
    if (x>=0 && x<(int)mazeWidth && y>=0 && y<(int)mazeHeight){
        return cells[y][x];
    } else{
        return true;
    }
}

static std::vector<glm::vec2> getUnvisitedNeighbors(glm::vec2 pos, bool cells[][mazeHeight]){
    std::vector<glm::vec2> result;
    if (!getCell(pos.x-1, pos.y, cells)){
        result.push_back(glm::vec2(pos.x-1, pos.y));
    }
    if (!getCell(pos.x, pos.y-1, cells)){
        result.push_back(glm::vec2(pos.x, pos.y-1));
    }
    if (!getCell(pos.x+1, pos.y, cells)){
        result.push_back(glm::vec2(pos.x+1, pos.y));
    }
    if (!getCell(pos.x, pos.y+1, cells)){
        result.push_back(glm::vec2(pos.x, pos.y+1));
    }
    return result;
}

static void removeWall(glm::vec2 currentCell, glm::vec2 newCell, std::vector<glm::vec3>& walls){
    auto remove = [&](unsigned wall, unsigned second)
    {
        walls.erase(std::remove(walls.begin(), walls.end(), glm::vec3(currentCell.x, currentCell.y, wall)), walls.end());
        walls.erase(std::remove(walls.begin(), walls.end(), glm::vec3(newCell.x, newCell.y, second)), walls.end());
    };

    if (newCell.x < currentCell.x){
        remove(0,2);
    }
    else if (newCell.x > currentCell.x){
        remove(2,0);
    }
    if (newCell.y < currentCell.y)
        remove(1, 3);
    else if (newCell.y > currentCell.y)
        remove(3, 1);
}

std::vector<glm::vec3> generateMaze(){
    std::vector<glm::vec3> walls;
    initializeWalls(walls);

    glm::vec2 currentCell(0,0);
    bool cells[mazeWidth][mazeHeight]={{0}};
    cells[(int)currentCell.y][(int)currentCell.x] = true;
    unsigned visitedCells = 1;

    std::stack<glm::vec2> used;

    while (visitedCells < mazeHeight*mazeWidth){
        std::vector<glm::vec2> neighbors = getUnvisitedNeighbors(currentCell, cells);

        if (neighbors.size()!=0){
            glm::vec2 newCell = neighbors[std::rand() % neighbors.size()];
            used.push(currentCell);
            removeWall(currentCell, newCell, walls);
            currentCell = newCell;
            cells[(int)currentCell.y][(int)currentCell.x] = true;
            visitedCells++;
        }
        else if (used.size() != 0){
            currentCell = used.top();
            used.pop();
        }
    }

    walls.push_back(glm::vec3(0, -0.0001, 1));
    walls.push_back(glm::vec3(mazeWidth-1, mazeWidth-1 + 0.9, 1));

    return walls;
}

std::vector<Cube> convertMazeToWorld(std::vector<glm::vec3> maze){
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

void setupMaze(){

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
}