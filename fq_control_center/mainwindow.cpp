#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings(new Settings),
    gas_plot(constants::NUMBER_OF_GAS_SENSORS, std::vector<double>(1000,1))
{
    ui->setupUi(this);

    //create an array of plot labels for easy handling
    gas_sensor_label[0] = ui->gas_sensor_0_label;
    gas_sensor_label[1] = ui->gas_sensor_1_label;
    gas_sensor_label[2] = ui->gas_sensor_2_label;
    gas_sensor_label[3] = ui->gas_sensor_3_label;
    for(unsigned i = 0; i < gas_plot.size(); ++i) {
        gas_sensor_label[i]->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    increase_tab_width();
    update_ui_settings();
    ui->video_output_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    initialize_camera();
    initialize_gas_sensors();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialize_gas_sensors()
{
    gas_sensors = new GasSensors(settings->gas_sensors_cfg, nullptr);
    connect(gas_sensors, SIGNAL(sendGasData(std::vector<std::vector<float> >)), this, SLOT(updateGasPlots(std::vector<std::vector<float> >)));
    gas_sensors->run();
}

void MainWindow::initialize_camera(){
    camera = new CameraLoop(settings->camera_cfg, nullptr);
    connect(camera, SIGNAL(sendFrame(cv::Mat)), this, SLOT(receiveFrame(cv::Mat)));
    connect(camera, SIGNAL(sendStats(long)), this, SLOT(receiveCameraStats(long)));
    camera->run();
}

void MainWindow::update_ui_settings()
{
    ui->camera_run_button->setText(QString::fromStdString(settings->camera_cfg["run"]));
    ui->object_detection_run_button->setText(QString::fromStdString(settings->camera_cfg["object detection"]));
    ui->tracking_run_button->setText(QString::fromStdString(settings->camera_cfg["tracking"]));
    ui->bounding_boxes_run_button->setText(QString::fromStdString(settings->camera_cfg["bounding boxes"]));
    ui->camera_stats_run_button->setText(QString::fromStdString(settings->camera_cfg["stats"]));
    if (ui->object_detection_run_button->text() == ">"){
        ui->bounding_boxes_run_button->setEnabled(false);
        ui->tracking_run_button->setEnabled(false);
    }else if(ui->bounding_boxes_run_button->text() == ">")
        ui->tracking_run_button->setEnabled(false);
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

void MainWindow::updateGasPlots(std::vector<std::vector<float>> g_data)
{
    for(unsigned i = 0; i < gas_plot.size(); ++i) {
        if (!gas_plot[i].empty() || g_data[i].size() != 0)
            gas_plot[i].erase(gas_plot[i].begin(), gas_plot[i].begin()+g_data[i].size());
        gas_plot[i].insert(std::end(gas_plot[i]), std::begin(g_data[i]), std::end(g_data[i]));
        cv::Mat data(gas_plot[i]);
        cv::Ptr<cv::plot::Plot2d> plot = cv::plot::Plot2d::create(data);
        // plot width has to be multiple of 4 if it is bigger than 400 pixels.
        // otherwise plot is disorted.
        // I don't know why and I don't have time to investigate further
        unsigned int plot_width = gas_sensor_label[i]->width();
        plot_width = plot_width - plot_width%4;
        plot->setPlotSize(plot_width,gas_sensor_label[i]->height()); // disorts the image
        //qDebug() << ui->gas_sensor_1_label->width() << " " << ui->gas_sensor_1_label->height();
        plot->setShowText(true);
        plot->setInvertOrientation(true);
        plot->setMaxX(1000);
        //plot->setMaxY(1000);
        plot->setPlotAxisColor(cv::Scalar(0, 0, 255));
        plot->render(gas_plot_image);
        QPixmap pix = QPixmap::fromImage(QImage((unsigned char*)gas_plot_image.data, gas_plot_image.cols, gas_plot_image.rows, QImage::Format_RGB888));
        gas_sensor_label[i]->setPixmap(pix);
        //cv::imshow( "plot", gas_plot_image );
    }
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
        ui->tracking_run_button->setText(">");
        camera->toggle_bounding_boxes = false;
        camera->toggle_tracking = false;

        ui->tracking_run_button->setEnabled(false);
    }else{
        ui->bounding_boxes_run_button->setText("||");
        camera->toggle_bounding_boxes = true;

        ui->tracking_run_button->setEnabled(true);
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

