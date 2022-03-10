#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "camera_loop.h"

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
    void receiveEmitted(int em);

    void on_camera_run_button_clicked();

private:
    Ui::MainWindow *ui;
    void increase_tab_width();
    CameraLoop camera;
};
#endif // MAINWINDOW_H
