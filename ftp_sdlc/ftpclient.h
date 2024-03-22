#include <QTcpSocket>
#include <QCoreApplication>
#include <QTextStream>




class ftpclient : public QObject {
    Q_OBJECT
public:
    explicit ftpclient(QObject *parent = nullptr) : QObject(parent) {
        connect(&client, &QTcpSocket::connected, this, &ftpclient::onConnected);
        connect(&client, &QTcpSocket::readyRead, this, &ftpclient::onReadyRead);
        connect(&client, &QTcpSocket::disconnected, this, &ftpclient::onDisconnected);
    }

    void connectToHost(const QString &hostName, quint16 port) {
        client.connectToHost(hostName, port);
    }

    void getFile(const QString &filePath) {
        QTextStream out(&client);
        out << "GET " + filePath + "\r\n";
    }

private slots:
    void onConnected() {
        qDebug() << "Connected to FTP server";
    }

    void onReadyRead() {
        QTextStream in(&client);
        qDebug() << "Response from server:";
        qDebug() << in.readAll();
    }

    void onDisconnected() {
        qDebug() << "Disconnected from FTP server";
    }

private:
    QTcpSocket client;
};
