#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTabBar>

#include <QThread>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings(new Settings)
{
    ui->setupUi(this);
    increase_tab_width();
    update_ui_settings();
    ui->video_output_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    camera = new CameraLoop(settings->camera_cfg, nullptr);
    connect(camera, SIGNAL(sendFrame(cv::Mat)), this, SLOT(receiveFrame(cv::Mat)));
    connect(camera, SIGNAL(sendStats(long)), this, SLOT(receiveCameraStats(long)));

    camera->run();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_ui_settings()
{
    ui->camera_run_button->setText(QString::fromStdString(settings->ui_cfg["run"]));
    ui->object_detection_run_button->setText(QString::fromStdString(settings->ui_cfg["object detection"]));
    ui->tracking_run_button->setText(QString::fromStdString(settings->ui_cfg["tracking"]));
    ui->bounding_boxes_run_button->setText(QString::fromStdString(settings->ui_cfg["bounding boxes"]));
    ui->camera_stats_run_button->setText(QString::fromStdString(settings->ui_cfg["stats"]));
}

void MainWindow::increase_tab_width()
{
    ui->tabWidget->tabBar()->setDocumentMode(true);
    ui->tabWidget->tabBar()->setExpanding(true);
    ui->camera_tabs->tabBar()->setDocumentMode(true);
    ui->camera_tabs->tabBar()->setExpanding(true);
}


void MainWindow::receiveFrame(cv::Mat image)
{
    QPixmap pix = QPixmap::fromImage(QImage((unsigned char*) image.data, image.cols, image.rows, QImage::Format_RGB888).rgbSwapped());
    ui->video_output_label->setPixmap(pix);
}

void MainWindow::receiveCameraStats(long compute_time)
{
    ui->compute_time_label->setText("us: " + QString::number(compute_time));
}

void MainWindow::on_apply_camera_settings_button_clicked()
{
    camera->toggle_stream = false;
    camera = new CameraLoop(settings->camera_cfg, nullptr);
    camera->run();
}

void MainWindow::on_camera_run_button_clicked()
{
    if (ui->camera_run_button->text() == "||"){
        ui->camera_run_button->setText(">");
        camera->toggle_video = false;
    }else{
        ui->camera_run_button->setText("||");
        camera->toggle_video = true;
    }
}

void MainWindow::on_object_detection_run_button_clicked()
{
    if (ui->object_detection_run_button->text() == "||"){
        ui->object_detection_run_button->setText(">");
        ui->tracking_run_button->setText(">");
        ui->bounding_boxes_run_button->setText(">");
        camera->toggle_object_detection = false;
        camera->toggle_tracking = false;
        camera->toggle_bounding_boxes = false;

        ui->bounding_boxes_run_button->setEnabled(false);
        ui->tracking_run_button->setEnabled(false);
    }else{
        ui->object_detection_run_button->setText("||");
        camera->toggle_object_detection = true;

        ui->bounding_boxes_run_button->setEnabled(true);
        ui->tracking_run_button->setEnabled(true);
    }
}


void MainWindow::on_tracking_run_button_clicked()
{
    if (ui->tracking_run_button->text() == "||" && ui->object_detection_run_button->text() == "||"){
        ui->tracking_run_button->setText(">");
        camera->toggle_tracking = false;
    }else{
        ui->tracking_run_button->setText("||");
        camera->toggle_tracking = true;
    }
}


void MainWindow::on_bounding_boxes_run_button_clicked()
{
    if (ui->bounding_boxes_run_button->text() == "||" && ui->object_detection_run_button->text() == "||"){
        ui->bounding_boxes_run_button->setText(">");
        camera->toggle_bounding_boxes = false;
    }else{
        ui->bounding_boxes_run_button->setText("||");
        camera->toggle_bounding_boxes = true;
    }
}


void MainWindow::on_camera_stats_run_button_clicked()
{
    if (ui->camera_stats_run_button->text() == "||"){
        ui->camera_stats_run_button->setText(">");
        camera->toggle_stats = false;
        ui->compute_time_label->setText("");
        ui->people_inside_label->setText("");
        ui->people_total_label->setText("");
    }else{
        ui->camera_stats_run_button->setText("||");
        camera->toggle_stats = true;
    }
}

