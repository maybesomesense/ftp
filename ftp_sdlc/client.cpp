#include "client.h"
#include "ui_client.h"

client::client(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::client)
{
    ui->setupUi(this);

    QVBoxLayout *layout = new QVBoxLayout(parent);

    fileComboBox = new FileComboBox(this);
    layout->addWidget(fileComboBox);

    int x = 10;
    int y = 100;
    fileComboBox->move(x, y);

    connect(fileComboBox, &FileComboBox::fileSelected, this, &client::onFileSelected);

    FileComboBox *fileComboBox = new FileComboBox();
    connect(fileComboBox, &FileComboBox::fileSelected, this, &client::handleFileSelected);
    ui->pushButton_4->setEnabled(false);
}

client::~client()
{
    delete ui;
}

void client::onFileSelected(const QString &filePath)
{
    this->filePath = filePath;
    ui->lineEdit_3->setText(this->filePath);
}


// подключение
void client::on_pushButton_2_clicked()
{
    // Создание сокета

    ui->textEdit->append("Try to connect to the server");
    // Подключение к серверу
    //QHostAddress ipAddress("127.0.0.1");
    socket.connectToHost("127.0.0.1", 1234); // Подключение к localhost на порту 1234
    if (!socket.waitForConnected(3000)) {
        ui->textEdit->append("Failed to connect to server!");
        qDebug() << "";
        ui->pushButton_2->setEnabled(true);
        return;
    }
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_4->setEnabled(true);
    ui->textEdit->append("Succesfully connected");

    // Отправка данных
//    socket.write("Hello, server!");

    qDebug() << "New connection established";
    // Открытие файла для чтения
//    QString filePath = this->filePath; //"D:\\My shit)\\ftp_server\\1.txt";
//    QFile file(filePath);
//    if (!file.open(QIODevice::ReadOnly)) {
//        ui->textEdit->append("Unable to open file:" + file.errorString());
//        return;
//    }

//    // Отправка файла по сокету
//    while (!file.atEnd()) {
//        QByteArray buffer = file.read(1024); // Чтение данных из файла
//        qint64 bytesWritten = socket.write(buffer); // Отправка данных через сокет
//        if (bytesWritten == -1) {
//            ui->textEdit->append("Error writing to socket:" + socket.errorString());
//            break;
//        }
//    }

//    file.close(); // Закрытие файла после передачи
//    socket.flush();

//    // Чтение ответа
//    if (!socket.waitForReadyRead()) {
//        qDebug() << "Failed to read from server!";
//        return;
//    }

    // Отключение
    //socket.disconnectFromHost();
}

// конфигурация
void client::on_pushButton_clicked()
{

    // включить / отключить очередь файлов
}

// передача файла
void client::on_pushButton_3_clicked()
{
    // Открытие файла для чтения
    QString filePath = this->filePath;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        ui->textEdit->append("Unable to open file:" + file.errorString());
        return;
    }

    // получаем имя файла и отправляем его по сокету
    QFileInfo fileInfo(filePath);

    socket.write(fileInfo.fileName().toUtf8());
    ui->textEdit->append("Sended file name: " + fileInfo.fileName());

    //    // Ожидание ответа от сервера
    if (!socket.waitForReadyRead()) {
        qDebug() << "Failed to receive response from server!";
        return;
    }

    // читаем ответ от сервера
    QByteArray data = socket.readAll();


    ui->textEdit->append("Respond from server: " + data);
    if(data == QByteArray::fromStdString("OK")){
        ui->textEdit->append("OKok");
    } else{
        ui->textEdit->append("Bad");
        return;
    }

    // Отправка файла по сокету
    while (!file.atEnd()) {
        QByteArray buffer = file.read(1024); // Чтение данных из файла
        qint64 bytesWritten = socket.write(buffer); // Отправка данных через сокет
        if (bytesWritten == -1) {
            ui->textEdit->append("Error writing to socket:" + socket.errorString());
            break;
        }
    }

    file.close(); // Закрытие файла после передачи
    socket.flush();
}

void client::handleFileSelected(const QString &filePath) {
    qDebug() << "Selected file: " << filePath;
}

// конец сессии
void client::on_pushButton_4_clicked()
{
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_4->setEnabled(false);
    socket.disconnectFromHost();
}

