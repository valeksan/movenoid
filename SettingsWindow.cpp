#include <QDebug>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QLabel>
#include <QProgressDialog>
#include <QDateTime>

#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"
#include "AppConfig.h"
#include "main.h"

extern AppConfig appConfig;

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    init();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}


void SettingsWindow::init(void)
{
    // На экране устанавливается значение файла устрофства захвата, считанное из конфига
    ui->captureDeviceFileName->setText(appConfig.getParameter("captureDeviceFileName"));

    // На экране устанавливается положение движка границы отсечения яркости
    ui->brigthnessTrashholdSlider->setMinimum(0);
    ui->brigthnessTrashholdSlider->setMaximum(100);
    ui->brigthnessTrashholdSlider->setSliderPosition(appConfig.getParameter("brigthnessTrashhold").toInt());

    initCaptureDevice();

    /*
    QGraphicsScene *scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setSceneRect(QRect(0,0, sizeOfFrame.width, sizeOfFrame.height));
    QGraphicsItem * sceneItem = scene->addPixmap(QPixmap::fromImage( img ));
    sceneItem->setPos(QPoint(10, 10));
    */

    /*
    ui->graphicsPixmapLabel->setMinimumSize(sizeOfFrame.width, sizeOfFrame.height);
    ui->graphicsPixmapLabel->setPixmap(QPixmap::fromImage( img ));

    this->setMinimumSize(sizeOfFrame.width, sizeOfFrame.height);
    this->adjustSize();
    */

    // int enabledWidth=ui->graphicsPixmapLabel->width();
    // int enabledWidth=ui->graphicsPixmapLabel->sizeHint().width();
    // int enabledWidth=ui->graphicsPixmapLabel->geometry().width();
    int enabledWidth=ui->mainVerticalLayout->sizeHint().width();
    qDebug() << "Enabled width label: " << enabledWidth;
    ui->graphicsPixmapLabel->setPixmap(QPixmap::fromImage( getCurrentImage() ).scaledToWidth(enabledWidth));
}


void SettingsWindow::initCaptureDevice(void)
{
    qDebug() << "Start init time: "<< QDateTime::currentDateTime();

    if(ui->captureDeviceFileName->text()=="default") {
        captureDevice.open(0); // Default камера
        if(!captureDevice.isOpened()) {
            QMessageBox msgBox;
            msgBox.setText("Can't open default camera");
            msgBox.exec();
            return;
        }
    } else {
        captureDevice.open( ui->captureDeviceFileName->text().toLocal8Bit().constData() );
        if(!captureDevice.isOpened()) {
            QMessageBox msgBox;
            msgBox.setText("Can't open camera device file: "+ui->captureDeviceFileName->text());
            msgBox.exec();
            return;
        }
    }

    qDebug() << "Finish init time: "<< QDateTime::currentDateTime();
}


QImage SettingsWindow::getCurrentImage(void)
{
    qDebug() << "Start getCurrentImage time: "<< QDateTime::currentDateTime();

    // Из потока берется один кадр
    captureDevice >> currentFrame;
    if (currentFrame.empty()) {
        QMessageBox msgBox;
        msgBox.setText("Error. Blank frame grabbed.");
        msgBox.exec();
    }
    // cv::imshow("Source frame", src);

    cv::Size sizeOfFrame=currentFrame.size();
    qDebug() << "Capture W: " << sizeOfFrame.width << " H: " << sizeOfFrame.height;

    QImage img((uchar*)currentFrame.data, currentFrame.cols, currentFrame.rows, currentFrame.step, QImage::Format_RGB888);
    // img=img.rgbSwapped(); // Преобразование цветов из BGR (OpenCV) в RGB (Qt)

    qDebug() << "Finish getCurrentImage time: "<< QDateTime::currentDateTime();

    return img;
}


// Клик по кнопке Reset to default
void SettingsWindow::onCaptureDeviceDefaultButtonClicked()
{
    ui->captureDeviceFileName->setText("default");
    appConfig.setParameter("captureDeviceFileName", "default");

}


// Клик по кнопке Apply
void SettingsWindow::onCaptureDeviceApplyButtonClicked()
{

}
