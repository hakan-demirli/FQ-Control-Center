#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTabBar>
#include <QThread>
#include <QLabel>
#include <QPushButton>

#include "camera_loop.h"
#include "settings.h"
#include "gas_sensors.h"
#include "constants.h"

#include <opencv2/plot.hpp>
#include <gnu/libc-version.h>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void receiveFrame(cv::Mat frame);
    void receiveCameraStats(std::vector<unsigned int> stats);
    void updateGasPlots(std::vector<std::vector<float>> g_data);

    void on_camera_run_button_clicked();
    void on_object_detection_run_button_clicked();
    void on_tracking_run_button_clicked();
    void on_bounding_boxes_run_button_clicked();
    void on_camera_stats_run_button_clicked();
    void on_gas_sensor_0_run_button_clicked();
    void on_gas_sensor_1_run_button_clicked();
    void on_gas_sensor_2_run_button_clicked();
    void on_gas_sensor_3_run_button_clicked();
    void on_middle_point_run_button_clicked();

    void on_boundary_line_run_button_clicked();

    void on_pushButton_clicked();

private:
    void increase_tab_width(void);
    void initialize_camera(void);
    void initialize_gas_sensors(void);

    Ui::MainWindow *ui;
    Settings* settings;
    CameraLoop& camera;
    GasSensors* gas_sensors;

    QLabel* gas_sensor_plot_label[constants::NUMBER_OF_GAS_SENSORS];
    QPushButton* gas_sensor_run_button[constants::NUMBER_OF_GAS_SENSORS];
    std::vector<std::vector<double>> gas_plot_data;
    cv::Mat gas_plot_image;

};
#endif // MAINWINDOW_H
