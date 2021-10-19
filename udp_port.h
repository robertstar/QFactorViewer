#ifndef UDP_PORT_H
#define UDP_PORT_H

//#include <WinSock2.h>
#include <QObject>
#include <QtNetwork/QUdpSocket>
#include <QMainWindow>
#include <QTimer>
#include <QPainter>

#include <QFile> // Подключаем класс QFile
#include <QTextStream> // Подключаем класс QTextStream


#define SIZE_X 1024
#define SIZE_Y 600


class UDP_port : public QObject
{
    Q_OBJECT
public:
    explicit UDP_port(QObject *parent = 0);
    ~UDP_port();

    QUdpSocket thisPort;
    QImage modImage;
    QImage srcImage;
    QTimer *tmr;

    //unsigned long int packet_cnt;
    unsigned long int data_cnt;

    quint16 line;
    quint8 red, green, blue;

    quint16 packet_cnt;
    quint16 pac_cntr;
    quint16 line_y_cnt;

    qint16 pixel1;
    qint16 pixel2;

    qint16 pixel1_source;
    qint16 pixel2_source;

    quint8 packet_alg;

    quint8 Brightness;

    int alg_fone;

    long cntBADpacket;

    long cntRX;
    long cntRXspeed;
    long cntErrFrame;
    long oldFrame;
    long cntFrame;
    long crFrame;

    long errLine;

    long cnt_frame;

    unsigned long int j=0;
    unsigned long int i=0;

    quint16 buf[600];
    quint16 buf_file[600];


    uint8_t cnt_min_max;


    quint16 buf_mod[300];
    quint16 buf_min[300];
    quint16 buf_max[300];
    quint16 buf_sub[300];

    quint16 file_line_cnt;
    quint16 buf_temp_fon[600];
    quint16 buf_temp_fon2[600];

    quint8 fon_state;
    quint8 fon_old;

    QByteArray array_sub;
    QByteArray array_fone;


    int row, column;
    long pix_cnt;

    QFile fileOut;
    //QTextStream writeStream;

    unsigned char *p;
    unsigned char *pointer;


signals:
    void pktTotable(unsigned char *);
    void UDPspeed(QString);


public slots:
    void readUDP();
    void init();
    void tmrOut();
    //void FON();
    //void ChangeBrightness(quint8 value);
    void AlgFone(int value);
    void ArrayFone(QByteArray);
};

#endif // UDP_PORT_H


