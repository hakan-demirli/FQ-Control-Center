#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings(new Settings)
{
    ui->setupUi(this);
    increase_tab_width();
    update_ui_settings();
    ui->video_output_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    initialize_camera();

    x=(double*)calloc(N1, sizeof(double));
    y1=(double*)calloc(N1, sizeof(double));
    y2=(double*)calloc(N1, sizeof(double));
    y3=(double*)calloc(N1, sizeof(double));

    //auto image=(double*)calloc(IMAGE_N*IMAGE_N, sizeof(double));

    xx.clear();
    yy.clear();
    for (int i=0; i<N1; i++) {
        x[i]=(i+1)*XMAX/(double)N1;
        xx.push_back(x[i]);
        yy.push_back(sin(0.5*M_PI*x[i])+2.0);
        std::cout<<xx[i]<<", "<<yy[i]<<std::endl;
        y1[i]=i*XMAX/(double)N1;
        y2[i]=log(x[i]);
        y3[i]=log10(x[i]);
    }

    JKQTFPVBarPlot* p1=new JKQTFPVBarPlot(ui->gas_sensor_1_plotter, N1, x, y1);
    JKQTFPLinePlot* p2=new JKQTFPLinePlot(ui->gas_sensor_1_plotter, N1, x, y2, QColor("blue"));
    JKQTFPLinePlot* p3=new JKQTFPLinePlot(ui->gas_sensor_1_plotter, N1, x, y3, QColor("darkgreen"));
    JKQTFPLinePlot* pv=new JKQTFPLinePlot(ui->gas_sensor_1_plotter, &xx, &yy, QColor("black"), Qt::SolidLine, 3);

    //JKQTFPimagePlot* p5=new JKQTFPimagePlot(ui->gas_sensor_1_plotter, image, JKQTFP_double, IMAGE_N, IMAGE_N, 0, 10, 0, 10, JKQTFP_GRAY);

    ///JKQTFPXRangePlot* p6 = new JKQTFPXRangePlot(ui->gas_sensor_1_plotter, 2.25, 7.75);
    //p6->setFillStyle(Qt::SolidPattern);

    ui->gas_sensor_1_plotter->addPlot(p1);
    ui->gas_sensor_1_plotter->addPlot(p2);
    ui->gas_sensor_1_plotter->addPlot(p3);
    //ui->gas_sensor_1_plotter->addPlot(p5);
    ui->gas_sensor_1_plotter->addPlot(pv);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialize_camera(){
    camera = new CameraLoop(settings->camera_cfg, nullptr);
    connect(camera, SIGNAL(sendFrame(cv::Mat)), this, SLOT(receiveFrame(cv::Mat)));
    connect(camera, SIGNAL(sendStats(long)), this, SLOT(receiveCameraStats(long)));
    camera->run();
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

