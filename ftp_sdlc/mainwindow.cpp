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
    QComboBox* comb = ui->comboBox;
    fileexplorer fdfgd(comb);
}

MainWindow::~MainWindow()
{
    delete ui;
}

