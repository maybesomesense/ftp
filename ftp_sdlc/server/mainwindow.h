#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT



public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void listFiles(QTcpSocket *socket);
    void upload(QTcpSocket *socket);
    void download(QTcpSocket *socket);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

public slots:
    void on_newConnection();
    void on_readyRead();

private:
    Ui::MainWindow *ui;
    QTcpServer *serv;
};
#endif // MAINWINDOW_H


