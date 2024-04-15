#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"
#include <QMessageBox>
#include <QFile>


QTcpServer *server;
QTcpSocket *socket;


// Слот для нового соединения
//void MainWindow::on_newConnection()
//{
//    socket = server->nextPendingConnection();


//    socket->waitForBytesWritten(3000);
//}

// Слот для нового соединения
void MainWindow::on_newConnection()
{
    QTcpSocket *socket = server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::on_readyRead);
}

// Слот для чтения данных от клиента
void MainWindow::on_readyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket)
        return;

    // Ожидание ответа от клиента
//    if (!socket->waitForReadyRead()) {
//        qDebug() << "Failed to receive response from server!";
//        return;
//    }


    // чтение имени файла
    QByteArray data = socket->readAll();
    socket->flush();
    ui->textEdit->append("Read file name:" + data);
    QString fileName = data;
    // отправление статуса OK
    socket->write("OK");
    socket->waitForBytesWritten(); // Дождитесь, пока данные будут записаны в сокет
    socket->flush();

    // Чтение данных из сокета
    ui->textEdit->append("Read data from client");

    // Ожидание ответа от сервера
    if (!socket->waitForReadyRead()) {
        qDebug() << "Failed to receive response from server!";
        return;
    }
    data = socket->readAll();

    // Здесь вы можете сохранить принятые данные в файл
    // Открытие файла для записи
    QString filePath = "D:\\My shit)\\ftp_server\\storage\\" + fileName;
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        ui->textEdit->append("Unable to open file for writing:" + file.errorString());
        return;
    }



    // Запись данных в файл
    qint64 bytesWritten = file.write(data);
    if (bytesWritten == -1) {
        ui->textEdit->append("Error writing to file:" + file.errorString());
        return;
    }


    file.close(); // Закрытие файла после записи
    ui->textEdit->append("Data written to file: " + filePath);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEdit->setReadOnly(true);
    ui->pushButton_2->setEnabled(false);
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &MainWindow::on_newConnection);

}



MainWindow::~MainWindow()
{
    delete ui;
}

// включение сервера
void MainWindow::on_pushButton_clicked()
{
    ui->pushButton->setEnabled(false);
    if (!server->listen(QHostAddress::LocalHost, 1234)) {
        ui->textEdit->append("Unable to start the server:");
        ui->pushButton->setEnabled(true);
        return;
    }
    ui->pushButton_2->setEnabled(true);
    ui->textEdit->append("Server started");
}


void MainWindow::on_pushButton_2_clicked()
{
    server->close();
    ui->textEdit->append("Server succesfully closed");
    ui->pushButton_2->setEnabled(false);
    ui->pushButton->setEnabled(true);
}

