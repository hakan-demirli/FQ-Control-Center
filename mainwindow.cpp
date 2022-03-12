#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTabBar>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    increase_tab_width();
    ui->video_output_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    camera = new CameraLoop;

    connect(camera, SIGNAL(sendValue(int)), this, SLOT(receiveEmitted(int)));
    connect(camera, SIGNAL(sendFrame(cv::Mat, long)), this, SLOT(receiveFrame(cv::Mat, long)));
    camera->run();
}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::increase_tab_width(){
    ui->tabWidget->tabBar()->setDocumentMode(true);
    ui->tabWidget->tabBar()->setExpanding(true);
    ui->camera_tabs->tabBar()->setDocumentMode(true);
    ui->camera_tabs->tabBar()->setExpanding(true);
}

void MainWindow::receiveEmitted(int em)
{
    ui->ai_run_button->setText(( QString::number(em)));
}


void MainWindow::on_camera_run_button_clicked()
{
    camera->receiveValue((int)69);
}

void MainWindow::receiveFrame(cv::Mat image, long inference_time){
    QPixmap pix = QPixmap::fromImage(QImage((unsigned char*) image.data, image.cols, image.rows, QImage::Format_RGB888).rgbSwapped());
    ui->video_output_label->setPixmap(pix);
    ui->compute_time_label->setText("us: " + QString::number(inference_time));
}
