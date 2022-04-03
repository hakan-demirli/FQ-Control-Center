#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    pg(PlayGround::getInstance(nullptr))
{
    ui->setupUi(this);
    pg.run();
    qDebug() << "MainWindow::MainWindow thread id:" << QThread::currentThreadId();
}

MainWindow::~MainWindow()
{
    pg.keep_running.clear();
    delete ui;
}


void MainWindow::on_cb_0_clicked()
{

}


void MainWindow::on_cb_1_clicked()
{

}


void MainWindow::on_cb_2_clicked()
{

}


void MainWindow::on_cb_3_clicked()
{

}

