#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTabBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    increase_tab_width();
    connect(&camera, SIGNAL(sendValue(int)), this, SLOT(receiveEmitted(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::increase_tab_width(){
    ui->tabWidget->tabBar()->setDocumentMode(true);
    ui->tabWidget->tabBar()->setExpanding(true);
    ui->camera_tabs->tabBar()->setDocumentMode(true);
    ui->camera_tabs->tabBar()->setExpanding(true);
}

void MainWindow::receiveEmitted(int em)
{
    ui->ai_run_button->setText(( QString::number(em)));
}


void MainWindow::on_camera_run_button_clicked()
{
    camera.receiveValue((int)69);
}

