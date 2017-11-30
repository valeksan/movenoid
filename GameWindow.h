#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QGraphicsScene>
#include <QDialog>

#include "GameField.h"

namespace Ui {
class GameWindow;
}

class GameWindow : public QDialog
{
    Q_OBJECT

public slots:
    void onSetScore(int iScore);
    void onScoreUp(int iScore);
    void onSetLives(int iLives);
    void onLivesUp();
    void onLivesDn();
    void reject();

public:
    explicit GameWindow(QWidget *parent = 0);
    ~GameWindow();

private:
    Ui::GameWindow *ui;

    GameField *gameField;
};

#endif // GAMEWINDOW_H
