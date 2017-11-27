#include <QDebug>

#include "main.h"
#include "RocketBit.h"

RocketBit::RocketBit(QGraphicsItem *parent)
{
    QGraphicsItem::setParentItem(parent);

    width=1.1;
    height=0.45;
    color=QColor(31, 56, 94);
}


QRectF RocketBit::boundingRect() const
{
    qreal halhWidth=width/2.0;
    qreal halhHeight=height/2.0;
    return QRectF(-halhWidth, -halhHeight, width, height);
}


QPainterPath RocketBit::shape() const
{
    QPainterPath path;
    path.addRect( boundingRect() );
    return path;
}


void RocketBit::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QPen pen; // Обводка

    // Корпус ракетки
    painter->setBrush(color); // Заливка
    pen.setWidth(0.1);
    pen.setBrush(Qt::white);
    painter->setPen(pen);
    painter->drawRect( boundingRect() );


    // Огонь из левой дюзы
    painter->setBrush(Qt::red); // Заливка
    pen.setWidth(0.2);
    pen.setBrush(Qt::yellow);

    QPolygonF polygon;
    qreal flameWidth=0.2;
    qreal flameHeight=0.25;
    polygon << QPointF(-width/2.0*0.8, height/2+0.1)
            << QPointF(-width/2.0*0.8-flameWidth/2.0, height/2+0.1+flameHeight)
            << QPointF(-width/2.0*0.8+flameWidth/2.0, height/2+0.1+flameHeight);
    painter->drawPolygon( polygon );

    // Огонь из правой дюзы
    polygon.translate(width/2.0*0.8 * 2, 0.0);
    painter->drawPolygon( polygon );
}


void RocketBit::putToPhysicsWorld()
{
    b2BodyDef bodyDef;
    bodyDef.type=b2_staticBody;
    bodyDef.position.Set(this->x(), this->y());
    b2Body *body=physicsWorld->CreateBody(&bodyDef);

    b2PolygonShape polygonShape;
    polygonShape.SetAsBox(width/2.0, height/2.0);

    body->CreateFixture(&polygonShape, 0.0);

    // Запоминается настроенное тело
    physicsBody=body;
}


void RocketBit::updatePosByMovieDetector()
{
    this->setPos( moveDetector.getRocketBitPos() );
    this->setRotation( radToDeg(moveDetector.getRocketBitAngle()) );

    // qDebug() << "RocketBit coordinats 1: " << moveDetector.getRocketBitPos();
    qDebug() << "RocketBit coordinats 2: " << this->x() << this->y();
}
