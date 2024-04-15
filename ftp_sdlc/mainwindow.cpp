#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#define login "admin"
#define password "123"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    clientForm = new client();
    connect(clientForm, &client::clientForm, this, &MainWindow::show);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onFileSelected(const QString &filePath)
{
    qDebug() << "Selected File: " << filePath;
}


void MainWindow::on_pushButton_clicked()
{
    if(ui->lineEdit->text() == login && ui->lineEdit_2->text() == password){
            clientForm->show();
            this->close();
        } else{
            QMessageBox::warning(nullptr, "Предупреждение", "Это предупреждение!");
            ui->lineEdit->clear();
            ui->lineEdit_2->clear();
            return;
        }
}

