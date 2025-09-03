#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mazeview.h"
#include <QMainWindow>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsView>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QGraphicsDropShadowEffect *windowShadow;
    void init();

    enum {AT_LEFT = 1, AT_TOP = 2,  AT_RIGHT = 4, AT_BOTTOM = 8,
          AT_TOP_LEFT = 3, AT_TOP_RIGHT = 6, AT_BOTTOM_LEFT = 9, AT_BOTTOM_RIGHT = 12};
    bool mousePressed = false;
    int mouseState;
    QPoint lastPos;
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

    bool maximized = false;
    QRect lastGeometry;
    void controlWindowScale();

    MazeView *mazeView;
    void mazePaint();

    void DFS();
    void BFS();
    void UCS();
    void A();
    void A_Star();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void setSorEInfo();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
