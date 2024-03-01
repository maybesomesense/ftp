#include "mainwindow.h"
#include <winsock2.h>
#include <QApplication>
#include <QComboBox>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

//    QComboBox comboBox;

    w.show();
    return a.exec();
}
