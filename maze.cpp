#include "maze.h"
#include <iostream>
#include <vector>
#include <utility>
#include <random>
#include <vector>
#include <algorithm>

Maze::Maze()
{
}

Maze::Maze(int size) {
    this->size = size;
    this->width = size * 2 + 1;
    this->maze = new int *[this->width];
    for (int i = 0; i < this->width; i++) {
        this->maze[i] = new int[this->width];
        for (int j = 0; j < this->width; j++) {
            this->maze[i][j] = 1;
        }
    }

    for (int i = 1; i < this->width; i += 2) {
        for (int j = 1; j < this->width; j += 2) {
            this->maze[i][j] = 0;
        }
    }

    this->prim();
}

void Maze::print() {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            std::cout<<maze[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
}

void Maze::prim() {
    int count = 0;
    std::vector<std::pair<int, int>> nonConnected;
    std::vector<std::pair<int, int>> connected;
    for (int i = 1; i <= size; i++) {
        for (int j = 1; j <= size; j++) {
            nonConnected.push_back(std::pair<int, int>(i, j));
        }
    }

    std::pair<int, int> point = this->getRandPoint(nonConnected);

    while (!nonConnected.empty()) {
        std::vector<std::pair<int, int>>::iterator pos = std::find(nonConnected.begin(), nonConnected.end(), point);
        nonConnected.erase(pos);
        connected.push_back(point);

        count++;
        point = this->breakWall(point);
        if (count == size * size - 1) {
            break;
        }

        while (this->isFullyConnected(point)) {
            point = this->getRandPoint(connected);
        }
    }
}

std::pair<int, int> Maze::getRandPoint(std::vector<std::pair<int, int>> points) {
    std::random_device seed;
    std::ranlux48 engine(seed());
    std::uniform_int_distribution<> distrib(0, points.size() - 1);
    int index = distrib(engine);
    std::pair point = points[index];
    return point;
}

std::pair<int, int> Maze::breakWall(std::pair<int, int> point) {
    std::random_device seed;
    std::ranlux48 engine(seed());
    std::uniform_int_distribution<> distrib(1, 4);
    int direction = distrib(engine);

    while(!this->canBeBroken(point, direction)) {
        direction = distrib(engine);
    }

    int y = point.first * 2 - 1;
    int x = point.second * 2 - 1;
    std::pair<int, int> nextPoint;

    switch(direction) {
    case 1:
        maze[y - 1][x] = 0;
        nextPoint = std::pair<int, int>(point.first - 1, point.second);
        break;
    case 2:
        maze[y][x + 1] = 0;
        nextPoint = std::pair<int, int>(point.first, point.second + 1);
        break;
    case 3:
        maze[y + 1][x] = 0;
        nextPoint = std::pair<int, int>(point.first + 1, point.second);
        break;
    case 4:
        maze[y][x - 1] = 0;
        nextPoint = std::pair<int, int>(point.first, point.second - 1);
        break;
    }
    return nextPoint;
}

bool Maze::canBeBroken(std::pair<int, int> point, int direction) {
    int y = point.first * 2 - 1;
    int x = point.second * 2 - 1;

    switch(direction) {
    case 1:
        if (y-1 == 0 || this->isConnected(x, y - 2)) {
            return false;
        }
        break;
    case 2:
        if (x + 1 == width - 1 || this->isConnected(x + 2, y)) {
            return false;
        }
        break;
    case 3:
        if (y + 1 == width - 1 || this->isConnected(x, y + 2)) {
            return false;
        }
        break;
    case 4:
        if (x - 1 == 0 || this->isConnected(x - 2, y)) {
            return false;
        }
        break;
    }

    return true;
}

bool Maze::isConnected(std::pair<int, int> point) {
    int y = point.first * 2 - 1;
    int x = point.second * 2 - 1;
    if(maze[y - 1][x] == 0 || maze[y][x + 1] == 0 || maze[y + 1][x] == 0 || maze[y][x - 1] == 0)
    {
        return true;
    }
    return false;
}

bool Maze::isConnected(int x, int y) {
    if(maze[y - 1][x] == 0 || maze[y][x + 1] == 0 || maze[y + 1][x] == 0 || maze[y][x - 1] == 0)
    {
        return true;
    }
    return false;
}

bool Maze::isFullyConnected(std::pair<int, int> point) {
    int y = point.first * 2 - 1;
    int x = point.second * 2 - 1;
    int count = 0;
    if (y > 1) {
       if (this->isConnected(x, y - 2)) {
           count++;
       }
    } else {
        count++;
    }
    if (x < this->width - 2) {
       if (this->isConnected(x + 2, y)) {
           count++;
       }
    } else {
        count++;
    }
    if (y < this->width - 2) {
       if (this->isConnected(x, y + 2)) {
           count++;
       }
    } else {
        count++;
    }
    if (x > 1) {
       if (this->isConnected(x - 2, y)) {
           count++;
       }
    } else {
        count++;
    }

    if(count == 4) {
        return true;
    }
    else {
        return false;
    }
}

bool Maze::isFullyConnected(int x, int y) {
    int connected = maze[y - 1][x] + maze[y][x + 1] + maze[y + 1][x] + maze[y][x - 1];
    if (connected > 1)
    {
        return false;
    }
    return true;
}

int ** Maze::getMaze() {
    return maze;
}

Maze::~Maze() {
    for(int i = 0; i < width; i++) {
        delete maze[i];
    }
    delete maze;
}

void Maze::resize(int size) {
    for(int i = 0; i < width; i++) {
        delete maze[i];
    }
    delete maze;

    this->size = size;
    this->width = size * 2 + 1;
    this->maze = new int *[this->width];
    for (int i = 0; i < this->width; i++) {
        this->maze[i] = new int[this->width];
        for (int j = 0; j < this->width; j++) {
            this->maze[i][j] = 1;
        }
    }

    for (int i = 1; i < this->width; i += 2) {
        for (int j = 1; j < this->width; j += 2) {
            this->maze[i][j] = 0;
        }
    }

    this->print();
    this->prim();
}

int Maze::getSize() {
    return size;
}
