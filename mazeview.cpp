#include "mazeview.h"
#include <QGraphicsLineItem>
#include <QMouseEvent>
#include <stack>
#include <QTime>
#include <QThread>
#include <random>
#include <queue>
#include <vector>
#include <iostream>
#include <cmath>

MazeView::MazeView()
{
    //this->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    this->setStyleSheet("border:none");

    this->cacheMode();

    //设置画笔
    linePen.setColor(QColorConstants::Black);
    linePen.setWidth(1);
    startPen.setColor(QColor(38, 201, 64));
    endPen.setColor(QColor(255, 90, 78));
    startBrush.setColor(QColor(38, 201, 64));
    endBrush.setColor(QColor(255, 90, 78));
    pointPen.setColor(QColor(254, 192, 46));
    start = new QGraphicsEllipseItem;
    end = new QGraphicsEllipseItem;
    connect(this, SIGNAL(updateViewSignal()), this, SLOT(updateView()));
}

void MazeView::mazePaint(int size) {
    int width = size * 2 + 1;
    length = this->width() / 2 / size;
    qDebug() << length;
    if(isShow) {
        maze->resize(size);
        scene->clear();
    } else {
        maze = new Maze(size);
        scene = new QGraphicsScene();
        this->setScene(scene);
    }

    int ** temp = maze->getMaze();
    for (int i = 0; i < width; i+=2) {
        for (int j = 0; j < width; j+=2) {
            if (j + 1 < width && temp[i][j + 1] == 1) {
                QGraphicsLineItem *lineItem = new QGraphicsLineItem();
                lineItem->setLine(QLineF(j * length, i * length, (j + 2) * length, i * length));
                lineItem->setPen(linePen);
                scene->addItem(lineItem);
            }
            if (i + 1 < width && temp[i + 1][j] == 1) {
                QGraphicsLineItem *lineItem = new QGraphicsLineItem();
                lineItem->setLine(QLineF(j * length, i * length, j * length, (i + 2) * length));
                lineItem->setPen(linePen);
                scene->addItem(lineItem);
            }
        }
    }
    qDebug() << this->width() << " " << this->height();
    qDebug() << "length: " << length;
}

int MazeView::getIndex(int x) {
    int result = 1;
    qDebug() << "length: " << length;
    while(x > length * 2) {
        x -= length * 2;
        result++;
        qDebug() << x;
    }
    return result;
}

void MazeView::mousePressEvent(QMouseEvent* event) {
    if(length == 0) {
        return;
    }
    int x = event->position().x();
    int y = event->position().y();
    if (selectState == 0) {
        startX = getIndex(x);
        startY = getIndex(y);
        delete start;
        start = new QGraphicsEllipseItem();
        startPen.setWidth(length / 2);
        start->setPen(startPen);
        start->setBrush(startBrush);
        start->setRect(QRect((startX - 1) * length * 2 + length / 4 * 3,
                             (startY - 1) * length * 2 + length /  4 * 3,
                             length / 2,
                             length / 2));
        selectState = 1;
        scene->addItem(start);
    } else {
        endX = getIndex(x);
        endY = getIndex(y);
        delete end;
        end = new QGraphicsEllipseItem();
        endPen.setWidth(length / 2);
        end->setPen(endPen);
        end->setBrush(startBrush);
        end->setRect(QRect((endX - 1) * length * 2 + length / 4 * 3,
                             (endY - 1) * length * 2 + length / 4 * 3,
                             length / 2,
                             length / 2));
        selectState = 0;
        scene->addItem(end);
    }
    emit setInfo();
}

int MazeView::getSelectState() {
    return selectState;
}

int MazeView::getStartX() {
    return startX;
}

int MazeView::getStartY() {
    return startY;
}

int MazeView::getEndX() {
    return endX;
}

int MazeView::getEndY() {
    return endY;
}

void MazeView::DFS() {
    if(!list.empty()) {
        clearList();
    }

    qDebug() << "DFS";
    int **temp_maze = maze->getMaze();
    std::stack<std::pair<int, int>> stack;
    std::pair<int, int> start(startY, startX);
    stack.push(start);
    int visited[maze->getSize() + 1][maze->getSize() + 1];

    for(int i = 1; i <= maze->getSize(); i++) {
        for (int j = 1; j <= maze->getSize(); j++) {
            visited[i][j] = 0;
        }
    }

    while(!stack.empty()) {
        std::pair<int, int> temp = (std::pair<int, int>)stack.top();
        int y = temp.first;
        int x = temp.second;

        if(x == endX && y == endY){
            break;
        }
        bool flag = false;

        visited[y][x] = 1;
        if(y != 1 && temp_maze[2 * y - 2][2 * x - 1] == 0 && visited[y - 1][x] == 0) {
            std::pair<int, int> tempPoint(y - 1, x);
            stack.push(tempPoint);
            drawPoint(x, y - 1);
            flag = true;
        } else if(x != maze->getSize() && temp_maze[2 * y - 1][2 * x] == 0 && visited[y][x + 1] == 0) {
            std::pair<int, int> tempPoint(y, x + 1);
            stack.push(tempPoint);
            drawPoint(x + 1, y);
            flag = true;
        } else if(y != maze->getSize() && temp_maze[2 * y][2 * x - 1] == 0 && visited[y + 1][x] == 0) {
            std::pair<int, int> tempPoint(y + 1, x);
            stack.push(tempPoint);
            drawPoint(x, y + 1);
            flag = true;
        } else if(x != 1 && temp_maze[2 * y - 1][2 * x - 2] == 0 && visited[y][x - 1] == 0) {
            std::pair<int, int> tempPoint(y, x - 1);
            stack.push(tempPoint);
            drawPoint(x - 1, y);
            flag = true;
        }

        if(!flag) {
            stack.pop();
        }
    }

    qDebug() << "finish";
    wait(1000);
    clearList();
}

void MazeView::BFS(){
    qDebug() << "BFS";
    if (!list.empty()) {
        clearList();
    }

    std::stack<std::pair<int, int>> stack;
    int **tempMaze = maze->getMaze();

    int visited[maze->getSize() + 1][maze->getSize() + 1];
    for (int i = 1; i <= maze->getSize(); i++) {
        for (int j = 1; j <= maze->getSize(); j++) {
            visited[i][j] = 0;
        }
    }

    std::pair<int, int> start(startY, startX);
    stack.push(start);

    while (!stack.empty()) {
        std::pair<int, int> tempPoint = stack.top();
        stack.pop();
        int y = tempPoint.first;
        int x = tempPoint.second;

        if(y == endY && x == endX) {
            break;
        }

        visited[y][x] = 1;

        if( y != 1 && tempMaze[2 * y -2][2 * x - 1] == 0 && visited[y - 1][x] != 1) {
            if(y - 1 == endY && x == endX) {
                break;
            }

            stack.push(std::pair<int, int>(y - 1, x));
            drawPoint(x, y - 1);
        }
        if( x != maze->getSize() && tempMaze[2 * y - 1][2 * x] == 0 && visited[y][x + 1] != 1) {
            stack.push(std::pair<int, int>(y, x + 1));
            drawPoint(x + 1, y);
        }
        if( y != maze->getSize() && tempMaze[2 * y][2 * x - 1] == 0 && visited[y + 1][x] != 1) {
            if(y == endY && x + 1 == endX) {
                break;
            }
            stack.push(std::pair<int, int>(y + 1, x));
            drawPoint(x, y + 1);
        }
        if( x != 1 && tempMaze[2 * y - 1][2 * x - 2] == 0 && visited[y][x - 1] != 1) {
            if(y == endY && x - 1 == endX) {
                break;
            }
            stack.push(std::pair<int, int>(y, x - 1));
            drawPoint(x - 1, y);
        }
    }

    qDebug() << "finish";
    wait(1000);
    clearList();
}

void MazeView::UCS() {
    qDebug() << "UCS";
    int size = maze->getSize();
    int **temp_maze = maze->getMaze();
    int cost[size * 2 + 1][size * 2 + 1];
    std::random_device seed;
    std::ranlux48 engine(seed());
    std::uniform_int_distribution<> distrib(1, 9);
    for (int i = 0; i < size * 2 + 1; i++) {
        for (int j = 0; j < size * 2 + 1; j++) {
            cost[i][j] = distrib(engine);
        }
    }

    for (int i = 1; i < size * 2 + 1; i += 2) {
        for (int j = 1; j < size * 2 + 1; j += 2) {
            cost[i][j] = 0;
        }
    }

    for (int i = 0; i < size * 2 + 1; i ++) {
        for (int j = 0; j < size * 2 + 1; j ++) {
            std::cout << cost[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::vector<std::pair<std::pair<int, int>, int>> priorityStack;

    std::pair<int, int> start(startX, startY);
    priorityStack.push_back(std::pair<std::pair<int, int>, int>(start, 0));

    while(!priorityStack.empty()) {
        std::pair<std::pair<int, int>, int> point_cost = priorityStack[priorityStack.size() - 1];
        priorityStack.pop_back();
        std::pair<int, int> point = point_cost.first;
        int y = point.first;
        int x = point.second;
        cost[y * 2 - 1][x * 2 - 1] = 1;
        int pointCost = point_cost.second;

        if (y != 1 && temp_maze[2 * y - 2][2 * x - 1] == 0 && cost[2 * y - 3][2 * x - 1] == 0) {
            int temp_cost = pointCost + cost[2 * y - 2][2 * x - 1];
            if (priorityStack.empty()) {
                priorityStack.push_back(std::pair<std::pair<int, int>, int>(std::pair<int, int>(y - 1, x), temp_cost));
            } else {
                std::vector<std::pair<std::pair<int, int>, int>>::iterator iter;
                for(iter = priorityStack.end() - 1; iter >= priorityStack.begin(); iter--) {
                    if(temp_cost < iter->second) {
                        break;
                    }
                }
                priorityStack.insert(iter + 1, std::pair<std::pair<int, int>, int>(std::pair<int, int>(y - 1, x), temp_cost));
            }
            drawPoint(x, y - 1);
            if(x == endX && y - 1== endY) break;
        }

        if (x != size && temp_maze[2 * y - 1][2 * x] == 0 && cost[2 * y - 1][2 * x + 1] == 0) {
            int temp_cost = pointCost + cost[2 * y - 1][2 * x];
            if (priorityStack.empty()) {
                priorityStack.push_back(std::pair<std::pair<int, int>, int>(std::pair<int, int>(y , x + 1), temp_cost));
            } else {
                std::vector<std::pair<std::pair<int, int>, int>>::iterator iter;
                for(iter = priorityStack.end() - 1; iter >= priorityStack.begin(); iter--) {
                    if(temp_cost < iter->second) {
                        break;
                    }
                }
                priorityStack.insert(iter + 1, std::pair<std::pair<int, int>, int>(std::pair<int, int>(y, x + 1), temp_cost));
            }
            drawPoint(x + 1, y);
            if(x + 1 == endX && y == endY) break;
        }

        if (y != size && temp_maze[2 * y][2 * x - 1] == 0 && cost[2 * y + 1][2 * x - 1] == 0) {
            int temp_cost = pointCost + cost[2 * y][2 * x - 1];
            if (priorityStack.empty()) {
                priorityStack.push_back(std::pair<std::pair<int, int>, int>(std::pair<int, int>(y + 1, x), temp_cost));
            } else {
                std::vector<std::pair<std::pair<int, int>, int>>::iterator iter;
                for(iter = priorityStack.end() - 1; iter >= priorityStack.begin(); iter--) {
                    if(temp_cost < iter->second) {
                        break;
                    }
                }
                priorityStack.insert(iter + 1, std::pair<std::pair<int, int>, int>(std::pair<int, int>(y + 1, x), temp_cost));
            }
            drawPoint(x, y + 1);
            if(x == endX && y + 1== endY) break;
        }

        if (x != 1 && temp_maze[2 * y - 1][2 * x - 2] == 0 && cost[2 * y - 1][2 * x - 3] == 0) {
            int temp_cost = pointCost + cost[2 * y - 1][2 * x - 2];
            if (priorityStack.empty()) {
                priorityStack.push_back(std::pair<std::pair<int, int>, int>(std::pair<int, int>(y, x - 1), temp_cost));
            } else {
                std::vector<std::pair<std::pair<int, int>, int>>::iterator iter;
                for(iter = priorityStack.end() - 1; iter >= priorityStack.begin(); iter--) {
                    if(temp_cost < iter->second) {
                        break;
                    }
                }
                priorityStack.insert(iter + 1, std::pair<std::pair<int, int>, int>(std::pair<int, int>(y, x - 1), temp_cost));
            }
            drawPoint(x - 1, y);
            if(x - 1 == endX && y == endY) break;
        }
    }

    qDebug() << "finish";
    wait(1000);
    clearList();
}

void MazeView::A() {
    qDebug() << "A";
    int size = maze->getSize();
    int **temp_maze = maze->getMaze();
    int cost[size * 2 + 1][size * 2 + 1];
    std::random_device seed;
    std::ranlux48 engine(seed());
    std::uniform_int_distribution<> distrib(1, 9);
    for (int i = 0; i < size * 2 + 1; i++) {
        for (int j = 0; j < size * 2 + 1; j++) {
            cost[i][j] = distrib(engine);
        }
    }

    for (int i = 0; i < size * 2 + 1; i ++) {
        for (int j = 0; j < size * 2 + 1; j ++) {
            std::cout << cost[i][j] << " ";
        }
        std::cout << std::endl;
    }

    int visited[size + 1][size + 1];
    for(int i = 0; i < size + 1; i++) {
        for(int j = 0; j < size + 1; j++) {
            visited[i][j] = 0;
        }
    }

    std::vector<std::pair<std::pair<int, int>, int>> priorityStack;

    std::pair<int, int> start(startX, startY);
    priorityStack.push_back(std::pair<std::pair<int, int>, int>(start, 0));

    while(!priorityStack.empty()) {
        std::pair<std::pair<int, int>, int> point_cost = priorityStack[priorityStack.size() - 1];
        priorityStack.pop_back();
        std::pair<int, int> point = point_cost.first;
        int y = point.first;
        int x = point.second;
        visited[y][x] = 1;
        int pointCost = point_cost.second;

        if (y != 1 && temp_maze[2 * y - 2][2 * x - 1] == 0 && visited[y - 1][x] == 0) {
            int temp_cost = pointCost + cost[2 * y - 2][2 * x - 1] + cost[2 * y - 3][2 * x - 1];
            if (priorityStack.empty()) {
                priorityStack.push_back(std::pair<std::pair<int, int>, int>(std::pair<int, int>(y - 1, x), temp_cost));
            } else {
                std::vector<std::pair<std::pair<int, int>, int>>::iterator iter;
                for(iter = priorityStack.end() - 1; iter >= priorityStack.begin(); iter--) {
                    if(temp_cost < iter->second) {
                        break;
                    }
                }
                priorityStack.insert(iter + 1, std::pair<std::pair<int, int>, int>(std::pair<int, int>(y - 1, x), temp_cost));
            }
            drawPoint(x, y - 1);
            if(x == endX && y - 1== endY) break;
        }

        if (x != size && temp_maze[2 * y - 1][2 * x] == 0 && visited[y][x + 1] == 0) {
            int temp_cost = pointCost + cost[2 * y - 1][2 * x] + cost[2 * y - 1][2 * x + 1];
            if (priorityStack.empty()) {
                priorityStack.push_back(std::pair<std::pair<int, int>, int>(std::pair<int, int>(y , x + 1), temp_cost));
            } else {
                std::vector<std::pair<std::pair<int, int>, int>>::iterator iter;
                for(iter = priorityStack.end() - 1; iter >= priorityStack.begin(); iter--) {
                    if(temp_cost < iter->second) {
                        break;
                    }
                }
                priorityStack.insert(iter + 1, std::pair<std::pair<int, int>, int>(std::pair<int, int>(y, x + 1), temp_cost));
            }
            drawPoint(x + 1, y);
            if(x + 1 == endX && y == endY) break;
        }

        if (y != size && temp_maze[2 * y][2 * x - 1] == 0 && visited[y + 1][x] == 0) {
            int temp_cost = pointCost + cost[2 * y][2 * x - 1] + cost[2 * y + 1][2 * x - 1];
            if (priorityStack.empty()) {
                priorityStack.push_back(std::pair<std::pair<int, int>, int>(std::pair<int, int>(y + 1, x), temp_cost));
            } else {
                std::vector<std::pair<std::pair<int, int>, int>>::iterator iter;
                for(iter = priorityStack.end() - 1; iter >= priorityStack.begin(); iter--) {
                    if(temp_cost < iter->second) {
                        break;
                    }
                }
                priorityStack.insert(iter + 1, std::pair<std::pair<int, int>, int>(std::pair<int, int>(y + 1, x), temp_cost));
            }
            drawPoint(x, y + 1);
            if(x == endX && y + 1== endY) break;
        }

        if (x != 1 && temp_maze[2 * y - 1][2 * x - 2] == 0 && visited[y][x - 1] == 0) {
            int temp_cost = pointCost + cost[2 * y - 1][2 * x - 2] + cost[2 * y - 1][2 * x - 3];
            if (priorityStack.empty()) {
                priorityStack.push_back(std::pair<std::pair<int, int>, int>(std::pair<int, int>(y, x - 1), temp_cost));
            } else {
                std::vector<std::pair<std::pair<int, int>, int>>::iterator iter;
                for(iter = priorityStack.end() - 1; iter >= priorityStack.begin(); iter--) {
                    if(temp_cost < iter->second) {
                        break;
                    }
                }
                priorityStack.insert(iter + 1, std::pair<std::pair<int, int>, int>(std::pair<int, int>(y, x - 1), temp_cost));
            }
            drawPoint(x - 1, y);
            if(x - 1 == endX && y == endY) break;
        }
    }

    qDebug() << "finish";
    wait(1000);
    clearList();
}

void MazeView::Greedy() {
    qDebug() << "Greedy";
    if (!list.empty()) {
        clearList();
    }

    std::stack<std::pair<int, int>> stack;
    int **tempMaze = maze->getMaze();
    int size = maze->getSize();

    int visited[maze->getSize() + 1][maze->getSize() + 1];
    for (int i = 1; i <= maze->getSize(); i++) {
        for (int j = 1; j <= maze->getSize(); j++) {
            visited[i][j] = 0;
        }
    }

    int cost[size * 2 + 1][size * 2 + 1];
    std::random_device seed;
    std::ranlux48 engine(seed());
    std::uniform_int_distribution<> distrib(1, 9);
    for (int i = 0; i < size * 2 + 1; i++) {
        for (int j = 0; j < size * 2 + 1; j++) {
            cost[i][j] = distrib(engine);
        }
    }

    for (int i = 1; i < size * 2 + 1; i += 2) {
        for (int j = 1; j < size * 2 + 1; j += 2) {
            cost[i][j] = 0;
        }
    }

    for (int i = 0; i < size * 2 + 1; i ++) {
        for (int j = 0; j < size * 2 + 1; j ++) {
            std::cout << cost[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::pair<int, int> start(startY, startX);
    visited[startX][startY] = 1;
    stack.push(start);

    while (!stack.empty()) {
        std::pair<int, int> tempPoint = stack.top();
        int flag = false;
        int y = tempPoint.first;
        int x = tempPoint.second;

        if(y == endY && x == endX) {
            break;
        }

        int temp_cost = 1000;
        std::pair<int, int> temp_point;


        if( y != 1 && tempMaze[2 * y -2][2 * x - 1] == 0 && visited[y - 1][x] != 1) {
            if(y == endY && x + 1 == endX) {
                break;
            }
            if( temp_cost > cost[2 * y - 2][2 * x - 1]) {
                temp_cost = cost[2 * y - 2][2 * x - 1];
                visited[y - 1][x] = 1;
                temp_point.first = y - 1;
                temp_point.second = x;
                flag =true;
            }
        }
        if( x != maze->getSize() && tempMaze[2 * y - 1][2 * x] == 0 && visited[y][x + 1] != 1) {
            if(y == endY && x + 1 == endX) {
                break;
            }
            if( temp_cost > cost[2 * y - 1][2 * x]) {
                temp_cost = cost[2 * y - 1][2 * x];
                visited[y][x + 1] = 1;
                temp_point.first = y;
                temp_point.second = x + 1;
                flag =true;
            }
        }
        if( y != maze->getSize() && tempMaze[2 * y][2 * x - 1] == 0 && visited[y + 1][x] != 1) {
            if(y == endY && x + 1 == endX) {
                break;
            }
            if( temp_cost > cost[2 * y][2 * x - 1]) {
                temp_cost = cost[2 * y][2 * x - 1];
                visited[y + 1][x] = 1;
                temp_point.first = y + 1;
                temp_point.second = x;
                flag =true;
            }

        }
        if( x != 1 && tempMaze[2 * y - 1][2 * x - 2] == 0 && visited[y][x - 1] != 1) {
            if(y == endY && x - 1 == endX) {
                break;
            }
            if( temp_cost > cost[2 * y - 1][2 * x - 2]) {
                temp_cost = cost[2 * y - 1][2 * x - 2];
                visited[y][x - 1] = 1;
                temp_point.first = y + 1;
                temp_point.second = x;
                flag =true;
            }

        }

        if (!flag) {
            stack.pop();
        } else {
            stack.push(temp_point);
            drawPoint(temp_point.second, temp_point.first);
        }
    }

    qDebug() << "finish";
    wait(1000);
    clearList();
}

void MazeView::A_Star() {
    qDebug() << "A_Stat";
    int size = maze->getSize();
    int **temp_maze = maze->getMaze();
    int cost[size * 2 + 1][size * 2 + 1];
    std::random_device seed;
    std::ranlux48 engine(seed());
    std::uniform_int_distribution<> distrib(1, 9);
    for (int i = 0; i < size * 2 + 1; i++) {
        for (int j = 0; j < size * 2 + 1; j++) {
            cost[i][j] = distrib(engine);
        }
    }

    for (int i = 1; i < size * 2 + 1; i += 2) {
        for (int j = 1; j < size * 2 + 1; j += 2) {
            cost[i][j] = abs(i - endY) + abs(j - endX);
        }
    }

    for (int i = 0; i < size * 2 + 1; i ++) {
        for (int j = 0; j < size * 2 + 1; j ++) {
            std::cout << cost[i][j] << " ";
        }
        std::cout << std::endl;
    }

    int visited[size + 1][size + 1];
    for(int i = 0; i < size + 1; i++) {
        for(int j = 0; j < size + 1; j++) {
            visited[i][j] = 0;
        }
    }

    std::vector<std::pair<std::pair<int, int>, int>> priorityStack;

    std::pair<int, int> start(startX, startY);
    priorityStack.push_back(std::pair<std::pair<int, int>, int>(start, 0));

    while(!priorityStack.empty()) {
        std::pair<std::pair<int, int>, int> point_cost = priorityStack[priorityStack.size() - 1];
        priorityStack.pop_back();
        std::pair<int, int> point = point_cost.first;
        int y = point.first;
        int x = point.second;
        visited[y][x] = 1;
        int pointCost = point_cost.second;

        if (y != 1 && temp_maze[2 * y - 2][2 * x - 1] == 0 && visited[y - 1][x] == 0) {
            int temp_cost = pointCost + cost[2 * y - 2][2 * x - 1] + cost[2 * y - 3][2 * x - 1];
            if (priorityStack.empty()) {
                priorityStack.push_back(std::pair<std::pair<int, int>, int>(std::pair<int, int>(y - 1, x), temp_cost));
            } else {
                std::vector<std::pair<std::pair<int, int>, int>>::iterator iter;
                for(iter = priorityStack.end() - 1; iter >= priorityStack.begin(); iter--) {
                    if(temp_cost < iter->second) {
                        break;
                    }
                }
                priorityStack.insert(iter + 1, std::pair<std::pair<int, int>, int>(std::pair<int, int>(y - 1, x), temp_cost));
            }
            drawPoint(x, y - 1);
            if(x == endX && y - 1== endY) break;
        }

        if (x != size && temp_maze[2 * y - 1][2 * x] == 0 && visited[y][x + 1] == 0) {
            int temp_cost = pointCost + cost[2 * y - 1][2 * x] + cost[2 * y - 1][2 * x + 1];
            if (priorityStack.empty()) {
                priorityStack.push_back(std::pair<std::pair<int, int>, int>(std::pair<int, int>(y , x + 1), temp_cost));
            } else {
                std::vector<std::pair<std::pair<int, int>, int>>::iterator iter;
                for(iter = priorityStack.end() - 1; iter >= priorityStack.begin(); iter--) {
                    if(temp_cost < iter->second) {
                        break;
                    }
                }
                priorityStack.insert(iter + 1, std::pair<std::pair<int, int>, int>(std::pair<int, int>(y, x + 1), temp_cost));
            }
            drawPoint(x + 1, y);
            if(x + 1 == endX && y == endY) break;
        }

        if (y != size && temp_maze[2 * y][2 * x - 1] == 0 && visited[y + 1][x] == 0) {
            int temp_cost = pointCost + cost[2 * y][2 * x - 1] + cost[2 * y + 1][2 * x - 1];
            if (priorityStack.empty()) {
                priorityStack.push_back(std::pair<std::pair<int, int>, int>(std::pair<int, int>(y + 1, x), temp_cost));
            } else {
                std::vector<std::pair<std::pair<int, int>, int>>::iterator iter;
                for(iter = priorityStack.end() - 1; iter >= priorityStack.begin(); iter--) {
                    if(temp_cost < iter->second) {
                        break;
                    }
                }
                priorityStack.insert(iter + 1, std::pair<std::pair<int, int>, int>(std::pair<int, int>(y + 1, x), temp_cost));
            }
            drawPoint(x, y + 1);
            if(x == endX && y + 1== endY) break;
        }

        if (x != 1 && temp_maze[2 * y - 1][2 * x - 2] == 0 && visited[y][x - 1] == 0) {
            int temp_cost = pointCost + cost[2 * y - 1][2 * x - 2] + cost[2 * y - 1][2 * x - 3];
            if (priorityStack.empty()) {
                priorityStack.push_back(std::pair<std::pair<int, int>, int>(std::pair<int, int>(y, x - 1), temp_cost));
            } else {
                std::vector<std::pair<std::pair<int, int>, int>>::iterator iter;
                for(iter = priorityStack.end() - 1; iter >= priorityStack.begin(); iter--) {
                    if(temp_cost < iter->second) {
                        break;
                    }
                }
                priorityStack.insert(iter + 1, std::pair<std::pair<int, int>, int>(std::pair<int, int>(y, x - 1), temp_cost));
            }
            drawPoint(x - 1, y);
            if(x - 1 == endX && y == endY) break;
        }
    }

    qDebug() << "finish";
    wait(1000);
    clearList();
}

void MazeView::drawPoint(int x, int y) {
    wait(200);

    QGraphicsEllipseItem *point = new QGraphicsEllipseItem();
    pointPen.setWidth(length / 4);
    point->setPen(pointPen);
    point->setBrush(startBrush);
    point->setRect(QRect((x - 1) * length * 2 + length / 8 * 7,
                         (y - 1) * length * 2 + length /  8 * 7,
                         length / 4,
                         length / 4));
    list.insert(list.end(), point);
    scene->addItem(point);
    updateView();
}

void MazeView::clearList() {
    for(int i = 0; i < list.size(); i++) {
        delete list[i];
    }
    while(!list.empty()) {
        list.pop_back();
    }
}

void MazeView::updateView() {
    this->repaint();
}

void MazeView::wait(int time) {
    QTime reachTime = QTime::currentTime().addMSecs(time);
    while(QTime::currentTime()<reachTime){
    }
}
