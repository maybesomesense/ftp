#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include "filecombobox.h"
#include <QtNetwork>


namespace Ui {
class client;
}

class client : public QWidget
{
    Q_OBJECT

private slots:
    void onFileSelected(const QString &filePath);

private:
    FileComboBox *fileComboBox;
    QString filePath;
    QTcpSocket socket;

signals:
    void clientForm();
public:
    explicit client(QWidget *parent = nullptr);
    ~client();

public slots:
    void handleFileSelected(const QString &filePath);

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::client *ui;
};

#endif // CLIENT_H
