#ifndef MAZEVIEW_H
#define MAZEVIEW_H

#include "maze.h"
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QThread>

class MazeView : public QGraphicsView
{
    Q_OBJECT

private:
    QGraphicsScene *scene;

    // 绘制迷宫
    Maze *maze;
    bool isShow = false;
    QPen linePen;

    int selectState = 0;
    int length = 0;
    int getIndex(int x);
    int startX;
    int startY;
    int endX;
    int endY;
    QGraphicsEllipseItem *start;
    QGraphicsEllipseItem *end;
    QPen startPen;
    QBrush startBrush;
    QPen endPen;
    QBrush endBrush;
    QPen pointPen;
    std::vector<QGraphicsEllipseItem *> list;
    void drawPoint(int x, int y);
    void clearList();
    void wait(int time);

signals:
    void setInfo();
    void updateViewSignal();

public slots:
    void updateView();

public:
    MazeView();
    void mazePaint(int size);
    int getSelectState();
    int getStartX();
    int getStartY();
    int getEndX();
    int getEndY();
    void DFS();
    void BFS();
    void UCS();
    void A();
    void Greedy();
    void A_Star();
    void mousePressEvent(QMouseEvent* event);
};

class DrawThread : public QThread {

};

#endif // MAZEVIEW_H
