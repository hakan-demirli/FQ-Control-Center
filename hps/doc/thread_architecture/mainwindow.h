#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "play_ground.h"

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
    void on_cb_0_clicked();

    void on_cb_1_clicked();

    void on_cb_2_clicked();

    void on_cb_3_clicked();

private:
    Ui::MainWindow *ui;
    PlayGround& pg;
};
#endif // MAINWINDOW_H
