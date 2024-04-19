#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include "AES.h"

AES aes;

QTcpServer *server;
QTcpSocket *socket;

// Слот для нового соединения
void MainWindow::on_newConnection()
{
    QTcpSocket *socket = server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::on_readyRead);
}

// Функция для отправки списка файлов клиенту
void MainWindow::listFiles(QTcpSocket *socket)
{
    // Получение списка файлов из директории
    QDir storageDir("D:\\My shit)\\ftp_server\\storage");
    QStringList files = storageDir.entryList(QDir::Files);
    socket->flush();
    // Отправка списка файлов клиенту==
    QString fileList = files.join("\n"); // Преобразование списка файлов в строку, разделенную символом новой строки
    ui->textEdit->append(fileList);
    socket->write(fileList.toUtf8()); // Отправка списка файлов в виде UTF-8 строки
    socket->waitForBytesWritten();
    socket->flush();
    ui->textEdit->append("Sent file list to client");
}

// Функция для отправки списка файлов клиенту
void MainWindow::upload(QTcpSocket *socket)
{
    // 2: UPLOAD
    socket->write("OK");
    socket->waitForBytesWritten();

    //
    if (!socket->waitForReadyRead()) {
        qDebug() << "Failed to receive response from server!";
        return;
    }

    QByteArray data = socket->readAll();
    QString fileName = data;

    ui->textEdit->append("Read file name:" + data);

    // 4: отправление статуса OK (FILENAME)
    socket->write("OK");
    socket->waitForBytesWritten(); // Дождитесь, пока данные будут записаны в сокет

    // Чтение данных из сокета
    ui->textEdit->append("Read data from client");

    // Здесь вы можете сохранить принятые данные в файл
    // Открытие файла для записи
    QString filePath = "D:\\My shit)\\ftp_server\\storage\\" + fileName;
    ui->textEdit->append("File path: " + filePath);
    qDebug() <<  "File path: " + filePath;
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        ui->textEdit->append("Unable to open file for writing:" + file.errorString());
        return;
    }

    while(1){
        unsigned char result[1008];
        if (!socket->waitForReadyRead()) {
            qDebug() << "Failed to receive response from server! (DATA2)";
            return;
        }
        data = socket->readAll();
        ui->textEdit->append("Data size: " + QString::number(data.size()));
        aes.Decrypt(reinterpret_cast<const unsigned char*>(data.data()), result);

        // Запись данных в файл
        qint64 bytesWritten = file.write(QByteArray(reinterpret_cast<const char*>(result), 1008));
        if (bytesWritten == -1) {
            ui->textEdit->append("Error writing to file:" + file.errorString());
            return;
        }

        socket->write("OK");
        socket->waitForBytesWritten();

    }



    file.close(); // Закрытие файла после записи
    ui->textEdit->append("Data written to file: " + filePath);
}

// скачивание с сервера
void MainWindow::download(QTcpSocket *socket){
    // 2: DOWNLOAD: OK
    socket->write("OK");
    socket->waitForBytesWritten(); // Дождитесь, пока данные будут записаны в сокет

    //
    if (!socket->waitForReadyRead()) {
        qDebug() << "Failed to receive response from server!";
        return;
    }

    // 3: FILENAME
    QByteArray data = socket->readAll();
    QString fileName = data;
    ui->textEdit->append("Filename from client: " + fileName);

    // 3: FILENAME received
    socket->write("OK");
    socket->waitForBytesWritten(); // Дождитесь, пока данные будут записаны в сокет


    if (!socket->waitForReadyRead()) {
        qDebug() << "Failed to receive response from server!";
        return;
    }
    // 4: receive signal READY
    data = socket->readAll();

    if(!(data == QByteArray::fromStdString("READY"))){
        ui->textEdit->append("not ready");
        return;
    }
    ui->textEdit->append("Server is ready to send file data");

    // Открытие файла для записи
    QString filePath = "D:\\My shit)\\ftp_server\\storage\\" + fileName;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        ui->textEdit->append("Unable to open file for reading:" + file.errorString());
        return;
    }

    // Отправка файла по сокету
    while (!file.atEnd()) {

        unsigned char cipher_text[1024];

        QByteArray buffer = file.read(1008); // Чтение данных из файла
        const unsigned char *data = reinterpret_cast<const unsigned char*>(buffer.data());
        aes.Encrypt(data, cipher_text);
        qint64 bytesWritten = socket->write(QByteArray(reinterpret_cast<const char*>(cipher_text), 1024)); // Отправка данных через сокет
        socket->waitForBytesWritten();
        if (bytesWritten == -1) {
            ui->textEdit->append("(DATA WRITE) Error writing to socket:" + socket->errorString());
            break;
        }

        if (!socket->waitForReadyRead()) {
            qDebug() << "Failed to receive response from server! (DATA2)";
            return;
        }

        QByteArray data2 = socket->readAll();
        if(data2 == QByteArray::fromStdString("OK")){
            ui->textEdit->append("Response from server: OK (UPLOAD)");
        } else{
            ui->textEdit->append("Response from server: BAD (UPLOAD)");
            return;
        }

    }

    file.close(); // Закрытие файла после передачи
    //socket->flush();

}

// Слот для чтения данных от клиента
void MainWindow::on_readyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket)
        return;

    QByteArray data = socket->readAll();
    QString command = QString::fromUtf8(data).trimmed(); // Преобразование к строке и удаление лишних пробелов

    // Обработка команд от клиента
    if (command == "LIST") {
        listFiles(socket);
        return;
    } else if(command == "UPLOAD"){
        upload(socket);
        return;
    }else if(command == "DOWNLOAD"){
        download(socket);
        return;
    } else {
        ui->textEdit->append("Unknown command received: " + command);
        return;
    }

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

