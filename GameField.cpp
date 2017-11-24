#include "GameField.h"

GameField::GameField()
{
    this->setSceneRect(0.0, 0.0, 10.0, 10.0);

    b2Vec2 gravity(0.0, 5.0);
    physicsWorld=new b2World(gravity);

    ball=NULL;

    loadLevel(1);
}

GameField::~GameField()
{
    clearLevel();
    delete physicsWorld;
}


void GameField::clearLevel()
{
    // Удаляются препятствия
    foreach( Barrier *currentBarrier, barriers ) {
        this->removeItem(currentBarrier);
        delete currentBarrier;
    }
    barriers.clear();

    // Удаляются кирпичи
    foreach( Brick *currentBrick, bricks ) {
        this->removeItem(currentBrick);
        delete currentBrick;
    }
    bricks.clear();

    rocketBit.setPos(5.0, 8.0);

    // ball.setPos(5.0, 6.0);
    delete ball;
}


void GameField::loadLevel(int levelNum)
{
    clearLevel();

    if(levelNum==1){

        // Создание препятствий
        QPolygonF polygon;

        // Левая стена
        Barrier *barrierLeft=new Barrier();
        polygon << QPointF(0.0, 0.0) << QPointF(0.0, 10.0) << QPointF(0.05, 10.0) << QPointF(0.05, 0.0);
        barrierLeft->setPolygon(polygon);
        barrierLeft->setPos(0.0, 0.0);
        barriers.append( barrierLeft ); // Запоминается указатель на препятствие
        this->addItem(barrierLeft); // Препятствие кладется на поле

        // Правая стена
        Barrier *barrierRight=new Barrier();
        barrierRight->setPolygon(polygon);
        barrierRight->setPos(10.0-0.049, 0.0);
        barriers.append( barrierRight );
        this->addItem(barrierRight);

        // Верхняя стена
        Barrier *barrierTop=new Barrier();
        polygon.clear();
        polygon << QPointF(0.0, 0.0) << QPointF(0.0, 0.05) << QPointF(10.0, 0.05) << QPointF(10.0, 0.0);
        barrierTop->setPolygon(polygon);
        barrierTop->setPos(0.0, 0.0);
        barriers.append( barrierTop );
        this->addItem(barrierTop);

        // Просто квадратик
        Barrier *barrierSquare=new Barrier();
        polygon.clear();
        polygon << QPointF(0.0, 0.0) << QPointF(0.0, 0.15) << QPointF(0.15, 0.15) << QPointF(0.15, 0.0);
        barrierSquare->setPolygon(polygon);
        barrierSquare->setPos(5.5, 7.5);
        barriers.append( barrierSquare );
        this->addItem(barrierSquare);

        // Создание кирпичей
        /*
        Brick *brick=new Brick;
        brick->setPos(3.0, 4.0);
        bricks.append( brick ); // Запоминается указатель на кирпич
        this->addItem(brick); // Кирпич кладется на поле
        */


        // Установки мячика
        Ball *ball=new Ball();
        ball->setRadius(0.15);
        ball->setPos(4.0, 7.0);
        this->addItem(ball); // Мячик кладется на поле


    }
}
