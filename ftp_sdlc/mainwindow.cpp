#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QDir"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->comboBox->show();

    QDir directory("D:\\My shit)\\untitled13"); // Замените "/path/to/your/directory" на путь к вашей директории
    QStringList files = directory.entryList(QDir::Files); // Получаем список всех файлов в директории

    foreach(QString file, files) {
        ui->comboBox->addItem(file); // Добавляем файлы в выпадающий список
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
