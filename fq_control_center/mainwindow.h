#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTabBar>
#include <QThread>

#include "camera_loop.h"
#include "settings.h"
#include "gas_sensors.h"

#include <opencv2/plot.hpp>


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
    void receiveCameraStats(long compute_time);
    void updateGasPlots(std::vector<float> g_data);

    void on_camera_run_button_clicked();
    void on_apply_camera_settings_button_clicked();
    void on_object_detection_run_button_clicked();
    void on_tracking_run_button_clicked();
    void on_bounding_boxes_run_button_clicked();
    void on_camera_stats_run_button_clicked();

private:
    void increase_tab_width(void);
    void update_ui_settings(void);
    void initialize_camera(void);
    void initialize_gas_sensors(void);

    Ui::MainWindow *ui;
    Settings* settings;
    CameraLoop* camera;
    GasSensors* gas_sensors;

    std::vector<float> gas_plot_1;
    cv::Mat gas_plot_1_image;

};
#endif // MAINWINDOW_H
