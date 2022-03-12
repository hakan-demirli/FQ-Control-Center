#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTabBar>
#include <QThread>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings(new Settings)
{
    ui->setupUi(this);
    increase_tab_width();
    //update_ui_settings();
    ui->video_output_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    camera = new CameraLoop(settings->camera_cfg, nullptr);
    connect(camera, SIGNAL(sendFrame(cv::Mat,long)), this, SLOT(receiveFrame(cv::Mat,long)));

    camera->run();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_ui_settings()
{
    ui->camera_run_button->setText(QString::fromStdString(settings->ui_cfg["run"]));
    ui->camera_run_button->setText(QString::fromStdString(settings->ui_cfg["tracking"]));
    ui->camera_run_button->setText(QString::fromStdString(settings->ui_cfg["object detection"]));
    ui->camera_run_button->setText(QString::fromStdString(settings->ui_cfg["stats"]));
}

void MainWindow::increase_tab_width()
{
    ui->tabWidget->tabBar()->setDocumentMode(true);
    ui->tabWidget->tabBar()->setExpanding(true);
    ui->camera_tabs->tabBar()->setDocumentMode(true);
    ui->camera_tabs->tabBar()->setExpanding(true);
}

void MainWindow::on_camera_run_button_clicked()
{
    if (ui->camera_run_button->text() == ">"){
        ui->camera_run_button->setText("||");
    }else{
        ui->camera_run_button->setText(">");
    }
    //camera->receiveValue((int)69);
}

void MainWindow::receiveFrame(cv::Mat image, long inference_time)
{
    QPixmap pix = QPixmap::fromImage(QImage((unsigned char*) image.data, image.cols, image.rows, QImage::Format_RGB888).rgbSwapped());
    ui->video_output_label->setPixmap(pix);
    ui->compute_time_label->setText("us: " + QString::number(inference_time));
}

void MainWindow::on_apply_camera_settings_button_clicked()
{
    camera->toggle_stream = false;
    camera = new CameraLoop(settings->camera_cfg, nullptr);
    camera->run();
}


