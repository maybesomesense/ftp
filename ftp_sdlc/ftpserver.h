#include <QTcpServer>
#include <QTcpSocket>
#include <QDir>
#include <QFile>

#ifndef FTPSERVER_H
#define FTPSERVER_H

//class ftpserver
//{
//public:
//    ftpserver();
//};

class ftpserver : public QTcpServer {
    Q_OBJECT
public:
    explicit ftpserver(QObject *parent = nullptr) : QTcpServer(parent) {}

protected:
    void incomingConnection(qintptr socketDescriptor) override {
        QTcpSocket *clientConnection = new QTcpSocket(this);
        clientConnection->setSocketDescriptor(socketDescriptor);

        connect(clientConnection, &QAbstractSocket::disconnected, clientConnection, &QObject::deleteLater);

        connect(clientConnection, &QIODevice::readyRead, [=]() {
            QTextStream in(clientConnection);
            QString request = in.readLine().trimmed();

            if (request.startsWith("GET")) {
                QStringList parts = request.split(" ");
                if (parts.length() == 2) {
                    QString filePath = parts[1];
                    QFile file(filePath);
                    if (file.open(QIODevice::ReadOnly)) {
                        QTextStream out(clientConnection);
                        out << "OK\r\n";
                        out << file.readAll();
                        file.close();
                    } else {
                        QTextStream out(clientConnection);
                        out << "Error: File not found\r\n";
                    }
                } else {
                    QTextStream out(clientConnection);
                    out << "Error: Invalid command\r\n";
                }
            } else {
                QTextStream out(clientConnection);
                out << "Error: Unknown command\r\n";
            }
            clientConnection->close();
        });
    }
};

#endif // FTPSERVER_H

