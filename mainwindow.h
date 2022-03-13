#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTabBar>
#include <QThread>

#include "camera_loop.h"
#include "settings.h"

#include "jkqtfastplotter/jkqtfastplotter.h"

#define N1 20
#define XMAX 10.0
#define IMAGE_N 300


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

    void on_camera_run_button_clicked();
    void on_apply_camera_settings_button_clicked();
    void on_object_detection_run_button_clicked();
    void on_tracking_run_button_clicked();
    void on_bounding_boxes_run_button_clicked();
    void on_camera_stats_run_button_clicked();

private:
    void increase_tab_width();
    void update_ui_settings();
    void initialize_camera();

    Ui::MainWindow *ui;
    Settings* settings;
    CameraLoop* camera;


    double* x;
    double* y1;
    double* y2;
    double* y3;
    QVector<double> xx;
    QVector<double> yy;

};
#endif // MAINWINDOW_H
