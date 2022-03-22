#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings(new Settings),
    camera(CameraLoop::getInstance(settings->camera_cfg, nullptr)),
    gas_plot_data(constants::NUMBER_OF_GAS_SENSORS, std::vector<double>(1000,1))
{
    ui->setupUi(this);
    increase_tab_width();
    initialize_camera();
    initialize_gas_sensors();
}

MainWindow::~MainWindow()
{
    camera.keep_running = false;
    delete ui;
}

void MainWindow::initialize_gas_sensors()
{
    //create arrays for easy handling
    gas_sensor_plot_label[0] = ui->gas_sensor_0_plot_label;
    gas_sensor_plot_label[1] = ui->gas_sensor_1_plot_label;
    gas_sensor_plot_label[2] = ui->gas_sensor_2_plot_label;
    gas_sensor_plot_label[3] = ui->gas_sensor_3_plot_label;

    gas_sensor_run_button[0] = ui->gas_sensor_0_run_button;
    gas_sensor_run_button[1] = ui->gas_sensor_1_run_button;
    gas_sensor_run_button[2] = ui->gas_sensor_2_run_button;
    gas_sensor_run_button[3] = ui->gas_sensor_3_run_button;

    for(unsigned i = 0; i < gas_plot_data.size(); ++i) {
        gas_sensor_plot_label[i]->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        gas_sensor_run_button[i]->setText(QString::fromStdString(settings->gas_sensors_cfg["Gas Sensor Plots"][i]));
    }

    gas_sensors = new GasSensors(settings->gas_sensors_cfg, nullptr);

    connect(gas_sensors, SIGNAL(sendGasData(std::vector<std::vector<float> >)), this, SLOT(updateGasPlots(std::vector<std::vector<float> >)));
    gas_sensors->run();
}

void MainWindow::initialize_camera()
{
    ui->video_output_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    ui->camera_run_button->setText(QString::fromStdString(settings->camera_cfg["Run"]));
    ui->boundary_line_run_button->setText(QString::fromStdString(settings->camera_cfg["Boundary Line"]));
    ui->object_detection_run_button->setText(QString::fromStdString(settings->camera_cfg["Object Detection"]));
    ui->tracking_run_button->setText(QString::fromStdString(settings->camera_cfg["Object Tracking"]));
    ui->middle_point_run_button->setText(QString::fromStdString(settings->camera_cfg["Middle Point"]));
    ui->bounding_boxes_run_button->setText(QString::fromStdString(settings->camera_cfg["Bounding Boxes"]));
    ui->camera_stats_run_button->setText(QString::fromStdString(settings->camera_cfg["Stats"]));

    if(ui->camera_run_button->text() == ">"){
        ui->boundary_line_run_button->setEnabled(false);
    }
    if (ui->object_detection_run_button->text() == ">"){
        ui->tracking_run_button->setEnabled(false);
        ui->middle_point_run_button->setEnabled(false);
        ui->bounding_boxes_run_button->setEnabled(false);

    }else if(ui->tracking_run_button->text() == ">"){
        ui->middle_point_run_button->setEnabled(false);
        ui->bounding_boxes_run_button->setEnabled(false);
    }

    connect(&camera.object_tracker, SIGNAL(sendFrame(cv::Mat)), this, SLOT(receiveFrame(cv::Mat)));
    connect(&camera.object_tracker, SIGNAL(sendStats(std::vector<unsigned int>)), this, SLOT(receiveCameraStats(std::vector<unsigned int>)));
    camera.run();
}

void MainWindow::increase_tab_width()
{
    ui->tabWidget->tabBar()->setDocumentMode(true);
    ui->tabWidget->tabBar()->setExpanding(true);
    ui->camera_tabs->tabBar()->setDocumentMode(true);
    ui->camera_tabs->tabBar()->setExpanding(true);
    ui->gas_sensors_tabs->tabBar()->setDocumentMode(true);
    ui->gas_sensors_tabs->tabBar()->setExpanding(true);
}

void MainWindow::receiveFrame(cv::Mat image)
{
    QPixmap pix = QPixmap::fromImage(QImage((unsigned char*) image.data, image.cols, image.rows, QImage::Format_RGB888).rgbSwapped());
    ui->video_output_label->setPixmap(pix);
}

void MainWindow::updateGasPlots(std::vector<std::vector<float>> g_data)
{
    for(unsigned i = 0; i < gas_plot_data.size(); ++i) {
        if(gas_sensor_run_button[i]->text() == ">"){
            // don't plot if it is stopped
        }else{
            if (!gas_plot_data[i].empty() || g_data[i].size() != 0)
                gas_plot_data[i].erase(gas_plot_data[i].begin(), gas_plot_data[i].begin()+g_data[i].size());
            gas_plot_data[i].insert(std::end(gas_plot_data[i]), std::begin(g_data[i]), std::end(g_data[i]));
            cv::Mat data(gas_plot_data[i]);
            cv::Ptr<cv::plot::Plot2d> plot = cv::plot::Plot2d::create(data);
            // plot width has to be multiple of 4 if it is bigger than 400 pixels.
            // otherwise plot is distorted.
            // I don't know why and I don't have time to investigate further
            unsigned int plot_width = gas_sensor_plot_label[i]->width();
            plot_width = plot_width - plot_width%4;
            plot->setPlotSize(plot_width,gas_sensor_plot_label[i]->height()); // disorts the image
            //qDebug() << ui->gas_sensor_1_label->width() << " " << ui->gas_sensor_1_label->height();
            plot->setShowText(true);
            plot->setInvertOrientation(true);
            plot->setMaxX(1000);
            //plot->setMaxY(1000);
            plot->setPlotAxisColor(cv::Scalar(0, 0, 255));
            plot->render(gas_plot_image);
            QPixmap pix = QPixmap::fromImage(QImage((unsigned char*)gas_plot_image.data, gas_plot_image.cols, gas_plot_image.rows, QImage::Format_RGB888));
            gas_sensor_plot_label[i]->setPixmap(pix);
            //cv::imshow( "plot", gas_plot_image );
        }
    }
}

void MainWindow::receiveCameraStats(std::vector<unsigned int> stats)
{
    ui->compute_time_label->setText("us: " + QString::number(stats[1]));
}

void MainWindow::on_camera_run_button_clicked()
{
    if (ui->camera_run_button->text() == "||"){
        ui->camera_run_button->setText(">");
        ui->boundary_line_run_button->setText(">");

        camera.object_tracker.toggle_video = false;

        ui->boundary_line_run_button->setEnabled(false);
    }else{
        ui->camera_run_button->setText("||");

        camera.object_tracker.toggle_video = true;

        ui->boundary_line_run_button->setEnabled(true);
    }
}

void MainWindow::on_boundary_line_run_button_clicked()
{
    if (ui->boundary_line_run_button->text() == "||"){
        ui->boundary_line_run_button->setText(">");

        camera.object_tracker.toggle_boundary_line = false;
    }else{
        ui->boundary_line_run_button->setText("||");

        camera.object_tracker.toggle_boundary_line = true;
    }
}

void MainWindow::on_object_detection_run_button_clicked()
{
    if (ui->object_detection_run_button->text() == "||"){
        ui->object_detection_run_button->setText(">");
        ui->tracking_run_button->setText(">");
        ui->middle_point_run_button->setText(">");
        ui->bounding_boxes_run_button->setText(">");

        camera.object_detector.toggle_object_detection = false;
        camera.object_tracker.toggle_object_tracking = false;
        camera.object_tracker.toggle_middle_point = false;
        camera.object_tracker.toggle_bounding_boxes = false;

        ui->tracking_run_button->setEnabled(false);
        ui->middle_point_run_button->setEnabled(false);
        ui->bounding_boxes_run_button->setEnabled(false);

    }else{
        ui->object_detection_run_button->setText("||");

        camera.object_detector.toggle_object_detection = true;

        ui->tracking_run_button->setEnabled(true);
        ui->middle_point_run_button->setEnabled(true);
        ui->bounding_boxes_run_button->setEnabled(true);
    }
}

void MainWindow::on_tracking_run_button_clicked()
{
    if (ui->tracking_run_button->text() == "||"){
        ui->tracking_run_button->setText(">");
        ui->middle_point_run_button->setText(">");
        ui->bounding_boxes_run_button->setText(">");
        camera.object_tracker.toggle_object_tracking = false;
        camera.object_tracker.toggle_middle_point = false;
        camera.object_tracker.toggle_bounding_boxes = false;

        ui->middle_point_run_button->setEnabled(false);
        ui->bounding_boxes_run_button->setEnabled(false);
    }else{
        ui->tracking_run_button->setText("||");
        camera.object_tracker.toggle_object_tracking = true;

        ui->middle_point_run_button->setEnabled(true);
        ui->bounding_boxes_run_button->setEnabled(true);
    }
}

void MainWindow::on_middle_point_run_button_clicked()
{
    if (ui->middle_point_run_button->text() == "||"){
        ui->middle_point_run_button->setText(">");
        camera.object_tracker.toggle_middle_point = false;
    }else{
        ui->middle_point_run_button->setText("||");
        camera.object_tracker.toggle_middle_point = true;
    }
}

void MainWindow::on_bounding_boxes_run_button_clicked()
{
    if (ui->bounding_boxes_run_button->text() == "||"){
        ui->bounding_boxes_run_button->setText(">");
        camera.object_tracker.toggle_bounding_boxes = false;
    }else{
        ui->bounding_boxes_run_button->setText("||");
        camera.object_tracker.toggle_bounding_boxes = true;
    }
}

void MainWindow::on_camera_stats_run_button_clicked()
{
    if (ui->camera_stats_run_button->text() == "||"){
        ui->camera_stats_run_button->setText(">");
        camera.object_tracker.toggle_stats = false;
        ui->compute_time_label->setText("");
        ui->people_inside_label->setText("");
        ui->people_total_label->setText("");
    }else{
        ui->camera_stats_run_button->setText("||");
        camera.object_tracker.toggle_stats = true;
    }
}

void MainWindow::on_gas_sensor_0_run_button_clicked()
{
    if (gas_sensor_run_button[0]->text() == "||"){
        gas_sensor_run_button[0]->setText(">");
    }else{
        gas_sensor_run_button[0]->setText("||");
    }
}

void MainWindow::on_gas_sensor_1_run_button_clicked()
{
    if (gas_sensor_run_button[1]->text() == "||"){
        gas_sensor_run_button[1]->setText(">");
    }else{
        gas_sensor_run_button[1]->setText("||");
    }
}

void MainWindow::on_gas_sensor_2_run_button_clicked()
{
    if (gas_sensor_run_button[2]->text() == "||"){
        gas_sensor_run_button[2]->setText(">");
    }else{
        gas_sensor_run_button[2]->setText("||");
    }
}

void MainWindow::on_gas_sensor_3_run_button_clicked()
{
    if (gas_sensor_run_button[3]->text() == "||"){
        gas_sensor_run_button[3]->setText(">");
    }else{
        gas_sensor_run_button[3]->setText("||");
    }
}
