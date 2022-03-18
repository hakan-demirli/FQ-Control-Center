#include "mainwindow.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<std::string>("std::string");

    //std::cout << "OpenCV version : " << CV_VERSION << std::endl;
    //std::cout << "Major version : " << CV_MAJOR_VERSION << std::endl;
    //std::cout << "Minor version : " << CV_MINOR_VERSION << std::endl;
    //std::cout << "Subminor version : " << CV_SUBMINOR_VERSION << std::endl;

    w.show();
    return a.exec();
}
