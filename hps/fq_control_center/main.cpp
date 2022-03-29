#include "mainwindow.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.showFullScreen();
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<std::vector<unsigned int> >("std::vector<unsigned int>");
    qRegisterMetaType<std::vector<std::vector<float>>>("std::vector<std::vector<float>>");

    qDebug() << "OpenCV version : " << CV_VERSION;
    qDebug() << "Major version : " << CV_MAJOR_VERSION;
    qDebug() << "Minor version : " << CV_MINOR_VERSION;
    qDebug() << "Subminor version : " << CV_SUBMINOR_VERSION;
    qDebug() << "GNU libc version: " << gnu_get_libc_version();

    w.show();
    return a.exec();
}
