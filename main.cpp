#include "mainwindow.h"
#include <QApplication>




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

   /* QThread *t = new QThread();
    t->start();

    UdpReceiver * net = new UdpReceiver();
    net->moveToThread(t);*/

    //w.show();


    w.showMaximized();

    return a.exec();
}
