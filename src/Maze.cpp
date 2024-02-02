//
// Created by Jana Trpkovska on 24.12.2023.
//

#include <Maze.h>

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