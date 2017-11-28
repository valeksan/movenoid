#include <QGraphicsScene>
#include <QLinearGradient>
#include <QBrush>

#include "GameWindow.h"
#include "ui_GameWindow.h"

GameWindow::GameWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameWindow)
{
    ui->setupUi(this);

    this->showFullScreen();

    // Устанавливается градиент заднего плана
    QLinearGradient gradient(0, 0, 0, ui->graphicsView->size().height());
    gradient.setColorAt(0, QColor::fromRgbF(0.87, 0.89, 0.94, 1));
    gradient.setColorAt(1, QColor::fromRgbF(0.69, 0.79, 0.97, 1));
    QBrush brush(gradient);
    ui->graphicsView->setBackgroundBrush(brush);

    // Задается сцена в виде игрового поля
    gameField=new GameField();
    gameField->setSceneRect(0.0, 0.0, 10.0, 10.0);
    // gameField->addText("Start game");

    ui->graphicsView->setScene(gameField);
    // ui->graphicsView->ensureVisible(0.0, 0.0, 10.0, 10.0, 0, 0);

    // ui->graphicsView->centerOn(2.5, 2.5);

    // ui->graphicsView->ensureVisible(0.0, 0.0, 10.0, 10.0, 0, 0);
    // ui->graphicsView->centerOn(5.0, 5.0);

    ui->graphicsView->fitInView(0.0, 0.0, 5.0, 5.0, Qt::KeepAspectRatio);
    // ui->graphicsView->fitInView(0.0, 0.0, 1.0, 1.0, Qt::KeepAspectRatio);

    gameField->runGame();

}

GameWindow::~GameWindow()
{
    delete gameField;
    delete ui;
}
