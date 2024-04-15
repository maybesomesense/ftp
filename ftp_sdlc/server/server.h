#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>

class server
{

public:
    server();
private:
    QTcpSocket* socket;
public slots:
    void on_newConnection();
private:
    QTcpServer *serv;


};




#endif // SERVER_H
