#include <QDebug>

#include "main.h"
#include "ReadyPoster.h"
#include "GameField.h"

GameField::GameField(QObject *parent) : QGraphicsScene(parent)
{
    connect(&updateWorldTimer, SIGNAL(timeout()), this, SLOT(updateWorld()));

    this->setSceneRect(0.0, 0.0, 10.0, 10.0);

    // Инициализация физического движка
    b2Vec2 gravity(0.0, 1.0); // Сила гравитации на игровом поле
    physicsWorld=new b2World(gravity);

    contactListener=new ContactListener;
    contactListener->setBall(&ball);
    contactListener->setBricks(&bricks);
    physicsWorld->SetContactListener(contactListener);

    // Определитель положения ракетки пришлось размещать здесь, так как GameField наследуется от QObject и здесь работает connect
    // Основной цикл определителя положения ракетки будет запускаться при старте треда, в который его помещают
    connect(&moveDetectorThread, &QThread::started, &moveDetector, &MoveDetector::run);
    
    // Соединения для корректного завершения потока
    connect(&moveDetector, SIGNAL(finished()), &moveDetectorThread, SLOT(quit()));
    
    moveDetector.moveToThread(&moveDetectorThread); // Определитель положения ракетки переносится в тред
    moveDetectorThread.start(); // Тред запускается, при этом в нем автоматически будет запущен объект moveDetector

    level=0;
}

GameField::~GameField()
{
    clearLevel();
    delete contactListener;
    delete physicsWorld;

    moveDetector.doExit();
    moveDetectorThread.quit();
    moveDetectorThread.wait();
    while(!moveDetectorThread.isFinished()) {
        qDebug() << "Wait finished move detector...";
    }
    qDebug() << "Success move detector finished.";
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
    ball.setPos(5.0, 6.0);
}


void GameField::loadLevel(int levelNum)
{
    clearLevel();

    if(levelNum==1){

        score=0;
        emit setScore(105);
        lives=5;
        emit setLives(lives);

        // Создание препятствий
        QPolygonF polygon;

        // Левая стена
        Barrier *barrierLeft=new Barrier();
        polygon << QPointF(0.0, 0.0) << QPointF(0.0, 10.0) << QPointF(0.05, 10.0) << QPointF(0.05, 0.0);
        barrierLeft->setPolygon(polygon);
        barrierLeft->setPos(0.0, 0.0);
        barriers.append( barrierLeft ); // Запоминается указатель на препятствие
        this->addItem(barrierLeft); // Препятствие кладется на поле
        barrierLeft->setPhysicsWorld(physicsWorld);

        // Правая стена
        Barrier *barrierRight=new Barrier();
        barrierRight->setPolygon(polygon);
        barrierRight->setPos(10.0-0.049, 0.0);
        barriers.append( barrierRight );
        this->addItem(barrierRight);
        barrierRight->setPhysicsWorld(physicsWorld);

        // Верхняя стена
        Barrier *barrierTop=new Barrier();
        polygon.clear();
        polygon << QPointF(0.0, 0.0) << QPointF(0.0, 0.05) << QPointF(10.0, 0.05) << QPointF(10.0, 0.0);
        barrierTop->setPolygon(polygon);
        barrierTop->setPos(0.0, 0.0);
        barriers.append( barrierTop );
        this->addItem(barrierTop);
        barrierTop->setPhysicsWorld(physicsWorld);

        // Временная балка снизу
        /*
        Barrier *barrierBottom=new Barrier();
        polygon.clear();
        polygon << QPointF(0.0, 0.0) << QPointF(0.0, 0.15) << QPointF(8.0, 0.15) << QPointF(8.0, 0.0);
        barrierBottom->setRotation(-10.0);
        barrierBottom->setPolygon(polygon);
        barrierBottom->setPos(1.5, 9.8);
        barriers.append( barrierBottom );
        this->addItem(barrierBottom);
        barrierBottom->setPhysicsWorld(physicsWorld);
        */

        // Создание кирпичей
        for(int i=0; i<5; i++) {
            qreal x=i*2+1.0;
            qreal y=4.0;
            createBrick(x,y);
        }
        for(int i=0; i<5; i++) {
            qreal x=i*2+1.5;
            qreal y=3.0;
            createBrick(x,y);
        }
        for(int i=0; i<5; i++) {
            qreal x=i*2+1.0;
            qreal y=2.0;
            createBrick(x,y);
        }

        // Установки мячика
        ball.setRadius(0.15);
        // ball.setPos(7.0, 2.0);
        ball.setPos(MOVE_NOID_START_BALL_POS_X, MOVE_NOID_START_BALL_POS_Y);
        this->addItem(&ball); // Мячик кладется на поле
        ball.setPhysicsWorld(physicsWorld);


        // Установки ракетки
        rocketBit.setPos(5.0, 8.0);
        this->addItem(&rocketBit); // Ракетка кладется на поле
        rocketBit.setPhysicsWorld(physicsWorld);
        rocketBit.setMoveDetector(&moveDetector);
    }
}


void GameField::createBrick(qreal x, qreal y)
{
    Brick *brick=new Brick;
    brick->setPos(x, y);
    bricks.append( brick ); // Запоминается указатель на кирпич
    this->addItem(brick); // Кирпич кладется на поле
    brick->setPhysicsWorld(physicsWorld);
}


// Метод удаляет кирпичи, с которыми столкнулся мячик
void GameField::destroyBricks()
{
    // Перебор кирпичей и удаление тех, с которым столкнулся мячик
    QList<Brick*>::iterator i = bricks.begin();
    while (i != bricks.end()) {
        Brick* brick=*i;

        if( brick->isToRemove() ) {
            // this->removeItem(brick); // Кирпич убирается с графического игрового поля
            physicsWorld->DestroyBody( brick->getPhysicsBody() ); // Кирпич удаяется из физического мира
            delete brick; // Кирпич удаляется как объект
            i = bricks.erase(i); // Кирпич удаляется из списка кирпичей

            emit scoreUp(10);
        }
        else
            ++i;
    }
}


void GameField::runGame()
{
    level=1;
    loadLevel(level);

    updateWorldTimer.start(1000/60);
}


// Слот, срабатывающий по таймеру updateWorldTimer
void GameField::updateWorld()
{
    physicsWorld->Step(1.0/60.0, 6, 2);

    destroyBricks();

    ball.updatePosByPhysicsWorld();
    rocketBit.updatePosByMovieDetector();

    checkBallPosition();

    // Обновляется сцена
    this->update();
}


// Проверка местоположения мяча
void GameField::checkBallPosition()
{
    // Если мячь улетел
    if(ball.y()>10.0) {
        emit livesDn();

        ReadyPoster readyPoster;
        readyPoster.exec();

        ball.moveToDefaultPos();
    }
}

