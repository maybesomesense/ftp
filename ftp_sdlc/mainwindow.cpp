#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDir>
#include "fileexplorer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->comboBox->show();

    fileexplorer fdfgd(ui->comboBox);
}

MainWindow::~MainWindow()
{
    delete ui;
}
