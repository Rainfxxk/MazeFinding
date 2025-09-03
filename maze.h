#ifndef MAZE_H
#define MAZE_H

#include <utility>
#include <vector>

class Maze
{
private:
    int **maze;
    int size;
    int width;

public:
    Maze(int size);
    Maze();
    ~Maze();
    void print();
    void prim();
    std::pair<int, int> getRandPoint(std::vector<std::pair<int, int>> points);
    std::pair<int, int> breakWall(std::pair<int, int> point);
    bool canBeBroken(std::pair<int, int> point, int direction);
    bool isConnected(std::pair<int, int> point);
    bool isConnected(int x, int y);
    bool isFullyConnected(std::pair<int, int> point);
    bool isFullyConnected(int x, int y);
    int** getMaze();
    int getSize();
    void resize(int size);
};

#endif // MAZE_H
