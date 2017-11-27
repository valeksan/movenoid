#include <QPoint>
#include <QDebug>

#include "MoveDetector.h"
#include "AppConfig.h"

extern AppConfig appConfig;

MoveDetector::MoveDetector()
{
    // Инициализируется устройство захвата изображения
    captureDevice.init( appConfig.getParameter("captureDeviceFileName") );
    captureDevice.setBrigthnessThreshold( appConfig.getParameter("brigthnessThreshold").toInt() );
}


Marker MoveDetector::detectMarker()
{

}


// Координаты ракетки
QPointF MoveDetector::getRocketBitPos()
{
    return getFakeRocketBitPos();

}


// Наклон ракетки, в радианах
qreal MoveDetector::getRocketBitAngle()
{
    return getFakeRocketBitAngle();
}


QPointF MoveDetector::getFakeRocketBitPos()
{
    static qreal angleShiftX=0.0;
    static qreal angleShiftY=0.0;
    static qreal anglePhase=0.0;

    angleShiftX+=0.025;
    angleShiftY+=0.015;
    anglePhase+=0.0005;

    qreal x=5.0+sin(angleShiftX+anglePhase)*4.0;
    qreal y=8.0+cos(angleShiftX+anglePhase)*1.5;

    // qDebug() << "RocketBit fake coordinats: " << x << y;

    return QPointF(x, y);
}


qreal MoveDetector::getFakeRocketBitAngle()
{
    static qreal angle=0.0;
    static int direction=1;
    qreal delta=0.01;
    qreal maxAngle=0.5;

    angle=angle+(qreal)direction*delta;

    if(fabs(angle)>maxAngle)
        direction=-direction;

    return angle;
}

