#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QPainter>
#include <QGraphicsLineItem>
#include <QString>
#include "mazeview.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->sizeButton, &QPushButton::clicked, this, [=](){controlWindowScale();});
    connect(ui->createButton, &QPushButton::clicked, ui->createButton, [=] () {mazePaint();});
    init();
}

void MainWindow::init() {
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    QPainterPath path;
    //path.addRoundedRect(ui->mainWidget->rect(), cornerRadius - 1, cornerRadius - 1);
//    QRegion mask(path.toFillPolygon().toPolygon());
//    ui->mainWidget->setMask(mask);

    // 设置阴影
    windowShadow = new QGraphicsDropShadowEffect(this);
    windowShadow->setBlurRadius(30);
    windowShadow->setColor(QColor(0, 0, 0));
    windowShadow->setOffset(0, 0);
    ui->mainWidget->setGraphicsEffect(windowShadow);

    // 添加画板
    mazeView = new MazeView();
    ui->mazeLayout->addWidget(mazeView);
    connect(mazeView, SIGNAL(setInfo()), this, SLOT(setSorEInfo()));

    connect(ui->DFS, &QPushButton::clicked, this, [=](){DFS();});
    connect(ui->BFS, &QPushButton::clicked, this, [=](){BFS();});
    connect(ui->UCS, &QPushButton::clicked, this, [=](){UCS();});
    connect(ui->A, &QPushButton::clicked, this, [=](){A();});
    connect(ui->AStar, &QPushButton::clicked, this, [=](){A_Star();});
}

void MainWindow::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        mousePressed = true;
        lastPos = event->globalPosition().toPoint() - this->frameGeometry().topLeft();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    if(event->buttons() == Qt::NoButton)
        mousePressed = false;
    if(!mousePressed){
        mouseState = 0;
        qDebug() << event->pos().x() << " " << event->pos().y();
        if(!maximized && abs(event->pos().x() - ui->mainWidget->pos().x()) < 5)
            mouseState |= AT_LEFT;
        if(!maximized && abs(event->pos().y() - ui->mainWidget->pos().y()) < 5)
            mouseState |= AT_TOP;
        if(!maximized && abs(event->pos().x() - ui->mainWidget->pos().x() - ui->mainWidget->width()) < 5)
            mouseState |= AT_RIGHT;
        if(!maximized && abs(event->pos().y() - ui->mainWidget->pos().y() - ui->mainWidget->height()) < 5)
            mouseState |= AT_BOTTOM;
        if(mouseState == AT_TOP_LEFT  || mouseState == AT_BOTTOM_RIGHT)
            setCursor(Qt::SizeFDiagCursor);
        else if(mouseState == AT_TOP_RIGHT || mouseState == AT_BOTTOM_LEFT)
            setCursor(Qt::SizeBDiagCursor);
        else if(mouseState & (AT_LEFT | AT_RIGHT))
            setCursor(Qt::SizeHorCursor);
        else if(mouseState & (AT_TOP | AT_BOTTOM))
            setCursor(Qt::SizeVerCursor);
        else
            unsetCursor();
    }
    else{
        if(mouseState == 0){
            if(maximized){
                qreal wRatio = (double)event->pos().x() / (double)ui->mainWidget->width();
                controlWindowScale();
                this->move(QPoint(event->globalPosition().x() - ui->mainWidget->width() * wRatio, -30));
                lastPos = QPoint(ui->mainWidget->width() * wRatio, event->pos().y());
            }
            else
                this->move(event->globalPosition().toPoint() - lastPos);
        }
        else{
            QPoint d = event->globalPosition().toPoint() - frameGeometry().topLeft() - lastPos;
            if(mouseState & AT_LEFT){
                this->move(this->frameGeometry().x() + d.x(), this->frameGeometry().y());
                this->resize(this->width() - d.x(), this->height());
            }
            if(mouseState & AT_RIGHT){
                this->resize(this->width() + d.x(), this->height());
            }
            if(mouseState & AT_TOP){
                this->move(this->frameGeometry().x(), this->frameGeometry().y() + d.y());
                this->resize(this->width(), this->height() - d.y());
            }
            if(mouseState & AT_BOTTOM){
                this->resize(this->width(), this->height() + d.y());
            }
        }
        lastPos = event->globalPosition().toPoint() - this->frameGeometry().topLeft();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    mousePressed = false;
    if(event->globalPosition().y() < 2) {
        controlWindowScale();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event){
    //Resize mask
    QPainterPath path;
    //path.addRoundedRect(ui->mainWidget->rect(), cornerRadius - 1, cornerRadius - 1);
    QRegion mask(path.toFillPolygon().toPolygon());
    ui->mainWidget->setMask(mask);

    //Resize all pages
//    for(int i = 0; i < pageList.size(); i++){
//        pageList[i]->resize(ui->mainWidget->width() * 0.3 < pageList[i]->preferWidth ? pageList[i]->preferWidth - 1 : ui->mainWidget->width() * 0.3 - 1, ui->mainWidget->height());
//        pageList[i]->resize(pageList[i]->width() + 1, pageList[i]->height());
//    }
}

void MainWindow::controlWindowScale(){
    if(!maximized){
        qDebug() << ui->mainWidget->rect();
        lastGeometry = this->frameGeometry();
        windowShadow->setEnabled(false);
        ui->verticalLayout->setContentsMargins(0, 0, 0, 0);
        QString mainStyle = "QWidget#mainWidget{background-color: rgb(248, 248, 248);border-radius:0px;}";
        ui->mainWidget->setStyleSheet(mainStyle);
        this->showMaximized();
        qDebug() << this->rect();
        qDebug() << ui->verticalLayout;
        qDebug() << ui->verticalLayout->contentsMargins();
        qDebug() << ui->mainWidget->rect();
        maximized = true;
        ui->mainWidget->resize(this->rect().width(), this->rect().height());
        QPainterPath path;
        path.addRect(ui->mainWidget->rect());
        QRegion mask(path.toFillPolygon().toPolygon());
        ui->mainWidget->setMask(mask);
        qDebug() << ui->mainWidget->x() << " " << ui->mainWidget->y();
    }
    else{
        ui->verticalLayout->setContentsMargins(30, 30, 30, 30);
        this->showNormal();
        QString mainStyle = "QWidget#mainWidget{background-color: rgb(248, 248, 248);border-radius:20px;}";
        ui->mainWidget->setStyleSheet(mainStyle);
        QPainterPath path;
        QRegion mask(path.toFillPolygon().toPolygon());
        ui->mainWidget->setMask(mask);
        windowShadow->setEnabled(true);
        this->resize(lastGeometry.width(), lastGeometry.height());
        this->move(lastGeometry.x(), lastGeometry.y());
        maximized = false;
    }
}

void MainWindow::setSorEInfo() {
    int state = mazeView->getSelectState();
    if (state == 1) {
        int x = mazeView->getStartX();
        int y = mazeView->getStartY();
        ui->startEdit->setText(QString::asprintf("(%d,%d)", x, y));
    } else {
        int x = mazeView->getEndX();
        int y = mazeView->getEndY();
        ui->endEdit->setText(QString::asprintf("(%d,%d)", x, y));
    }
}

void MainWindow::mazePaint() {
    int size = ui->sizeEdit->text().toInt();
    mazeView->mazePaint(size);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::DFS(){
    mazeView->DFS();
}

void MainWindow::BFS() {
    mazeView->BFS();
}

void MainWindow::UCS() {
    mazeView->UCS();
}

void MainWindow::A() {
    mazeView->A();
}

void MainWindow::A_Star() {
    mazeView->A_Star();
}

