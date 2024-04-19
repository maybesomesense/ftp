#include "client.h"
#include "ui_client.h"
#include "AES.h"

AES aes;

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
    setAcceptDrops(true);
    FileComboBox *fileComboBox = new FileComboBox();
    connect(fileComboBox, &FileComboBox::fileSelected, this, &client::handleFileSelected);
    ui->pushButton_4->setEnabled(false);
}

client::~client()
{
    delete ui;
}

void client::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void client::dropEvent(QDropEvent *event) {
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        if (!urlList.isEmpty()) {
            QString filePath = urlList.first().toLocalFile();
            this->filePath = filePath;
            ui->textEdit->append("Choosen file:" + this->filePath);
            ui->lineEdit_3->setText(this->filePath);
            qDebug() << "Dropped file:" << filePath;
        }
    }
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

    qDebug() << "New connection established";

}

// конфигурация
void client::on_pushButton_clicked()
{

    // включить / отключить очередь файлов
}

// передача файла
void client::on_pushButton_3_clicked()
{
    // 1
    socket.write("UPLOAD");
    socket.waitForBytesWritten();

    if (!socket.waitForReadyRead()) {
        qDebug() << "Failed to receive response from server!";
        return;
    }

    // 2
    QByteArray data = socket.readAll();
    if(data == QByteArray::fromStdString("OK")){
        ui->textEdit->append("Response from server: OK (UPLOAD)");
    } else{
        ui->textEdit->append("Response from server: BAD (UPLOAD)");
        return;
    }
    // 2


    // Открытие файла для чтения
    QString filePath = this->filePath;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        ui->textEdit->append("Unable to open file:" + file.errorString());
        return;
    }

    // получаем имя файла и отправляем его по сокету
    QFileInfo fileInfo(filePath);

    // 3
    socket.write(fileInfo.fileName().toUtf8());
    ui->textEdit->append("Sended file name: " + fileInfo.fileName().toUtf8());

    //    // Ожидание ответа от сервера
    if (!socket.waitForReadyRead()) {
        qDebug() << "Failed to receive response from server!";
        return;
    }

    // 4
    // читаем ответ от сервера
    data = socket.readAll();
    ui->textEdit->append("Respond from server: " + data);
    if(data == QByteArray::fromStdString("OK")){
        ui->textEdit->append("OKok");
    } else{
        ui->textEdit->append("Bad");
        return;
    }


    // Отправка файла по сокету
    while (!file.atEnd()) {

        unsigned char cipher_text[1024];

        QByteArray buffer = file.read(1008); // Чтение данных из файла
        const unsigned char *data = reinterpret_cast<const unsigned char*>(buffer.data());
        aes.Encrypt(data, cipher_text);
        qint64 bytesWritten = socket.write(QByteArray(reinterpret_cast<const char*>(cipher_text), 1024)); // Отправка данных через сокет
        socket.waitForBytesWritten();
        if (bytesWritten == -1) {
            ui->textEdit->append("(DATA WRITE) Error writing to socket:" + socket.errorString());
            break;
        }

        if (!socket.waitForReadyRead()) {
            qDebug() << "Failed to receive response from server! (DATA2)";
            return;
        }

        QByteArray data2 = socket.readAll();
        if(data2 == QByteArray::fromStdString("OK")){
            ui->textEdit->append("Response from server: OK (UPLOAD)");
        } else{
            ui->textEdit->append("Response from server: BAD (UPLOAD)");
            return;
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


// вывод файлов
void client::on_pushButton_5_clicked()
{
    socket.write("LIST");
    socket.waitForBytesWritten();

    if (!socket.waitForReadyRead()) {
        qDebug() << "Failed to receive response from server!";
        return;
    }

    QByteArray data = socket.readAll();
    // Обработка полученного списка файлов

    ui->textEdit->append(data);
    qDebug() << "Received file list from server:" << data;
    // Здесь можно добавить код для отображения списка файлов на клиентском интерфейсе
}

// скачать файл
void client::on_pushButton_6_clicked()
{
    if(ui->lineEdit->text().isEmpty()){
        QMessageBox::warning(nullptr, "Предупреждение", "Пустое поле ввода");
        return;
    }

    // 1: DOWNLOAD
    socket.write("DOWNLOAD");
    socket.waitForBytesWritten(); // w8


    if (!socket.waitForReadyRead()) {
        qDebug() << "Failed to receive response from server!";
        return;
    }
    // 2: OK: DOWNLOAD
    QByteArray data = socket.readAll();

    if(data == QByteArray::fromStdString("OK")){
        ui->textEdit->append("Respond from server: OK (DOWNLOAD)");
    } else{
        ui->textEdit->append("Respond from server: BAD (DOWNLOAD)");
        return;
    }

    // 3: FILENAME
    socket.write(ui->lineEdit->text().toUtf8());
    socket.waitForBytesWritten(); // Дождитесь, пока данные будут записаны в сокет


    if (!socket.waitForReadyRead()) {
        qDebug() << "Failed to receive response from server!";
        return;
    }
    // 3: FILENAME ok send
    data = socket.readAll();

    if(data == QByteArray::fromStdString("OK")){
        ui->textEdit->append("Respond from server: OK (DOWNLOAD)");
    } else{
        ui->textEdit->append("Respond from server: OK (DOWNLOAD)");
        return;
    }

    socket.write("READY");
    socket.waitForBytesWritten(); // Дождитесь, пока данные будут записаны в сокет


    // Открытие файла для записи
    QString filePath = "D:\\BSUIR\\ftp\\ftp_sdlc\\storage\\" + ui->lineEdit->text();
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        ui->textEdit->append("Unable to open file for writing:" + file.errorString());
        return;
    }

//    // receive file
//    if (!socket.waitForReadyRead()) {
//        qDebug() << "Failed to receive response from server!";
//        return;
//    }
//    data = socket.readAll();

//    for(int i = 0; i <= data.size()/1024; i++){
//        unsigned char result[1008];
//        aes.Decrypt(reinterpret_cast<const unsigned char*>(data.mid(i * 1024, (i+1) * 1024).data()), result);

//        // Запись данных в файл
//        qint64 bytesWritten = file.write(QByteArray(reinterpret_cast<const char*>(result), 1008));
//        if (bytesWritten == -1) {
//            ui->textEdit->append("Error writing to file:" + file.errorString());
//            return;
//        }
//    }

    while(1){
        unsigned char result[1008];
        if (!socket.waitForReadyRead()) {
            qDebug() << "Failed to receive response from server! (DATA2)";
            return;
        }
        data = socket.readAll();
        ui->textEdit->append("Data size: " + QString::number(data.size()));
        aes.Decrypt(reinterpret_cast<const unsigned char*>(data.data()), result);

        // Запись данных в файл
        qint64 bytesWritten = file.write(QByteArray(reinterpret_cast<const char*>(result), 1008));
        if (bytesWritten == -1) {
            ui->textEdit->append("Error writing to file:" + file.errorString());
            return;
        }

        socket.write("OK");
        socket.waitForBytesWritten();
    }


    file.close(); // Закрытие файла после записи
    ui->textEdit->append("Data written to file: " + filePath);

    //




}

