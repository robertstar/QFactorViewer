#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTextEdit>
#include <QTimer>

#include <QUdpSocket>
#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QUdpSocket>

#include <qcustomplot.h>

#include <QSettings>
#include "settingsdialog.h"
#include "settings.h"

#include "udp_port.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int MinimumHeight;
    int ZoomVal;

    QWidget     *window;
    QVBoxLayout *window_l;
    QLayoutItem *item;
    QVBoxLayout *vLay;
    QHBoxLayout *hLay;
    QTabWidget  *tw;

    QVBoxLayout *scope1_l;
    QWidget     *scope1_w;
    QScrollArea *scroll_tab1;
    QScrollArea *scroll_tab2;
    QScrollArea *scroll_tab3;
    QScrollArea *scroll_tab4;

    QLabel *label_ch1_rms;
    QLabel *label_ch1_amp;
    QLabel *label_ch1_pk;

    QLabel *label_ch2_rms;
    QLabel *label_ch2_amp;
    QLabel *label_ch2_pk;

    QLabel *label_ch3_rms;
    QLabel *label_ch3_amp;
    QLabel *label_ch3_pk;

    QLabel *label_ch4_rms;
    QLabel *label_ch4_amp;
    QLabel *label_ch4_pk;

    QLabel *label_ch5_rms;
    QLabel *label_ch5_amp;
    QLabel *label_ch5_pk;

    QLabel *label_ch6_rms;
    QLabel *label_ch6_amp;
    QLabel *label_ch6_pk;

    QLabel *label_ch7_rms;
    QLabel *label_ch7_amp;
    QLabel *label_ch7_pk;

    QLabel *label_ch8_rms;
    QLabel *label_ch8_amp;
    QLabel *label_ch8_pk;

    unsigned long rms_count;

    /*double temp1;
    double temp2;
    double temp3;
    double temp4;
    double temp5;
    double temp6;
    double temp7;
    double temp8;*/

    double temp_U1rms;
    double temp_U2rms;
    double temp_U3rms;
    double temp_U4rms;
    double temp_U5rms;
    double temp_U6rms;
    double temp_U7rms;
    double temp_U8rms;

    double U1_rms;
    double U2_rms;
    double U3_rms;
    double U4_rms;
    double U5_rms;
    double U6_rms;
    double U7_rms;
    double U8_rms;

    double U1_amp;
    double U2_amp;
    double U3_amp;
    double U4_amp;
    double U5_amp;
    double U6_amp;
    double U7_amp;
    double U8_amp;

    double U1_pk;
    double U2_pk;
    double U3_pk;
    double U4_pk;
    double U5_pk;
    double U6_pk;
    double U7_pk;
    double U8_pk;


private slots:

    void readPendingDatagrams();
    void Replot();
    void mousePress(QMouseEvent* event);
    void mouseRelease(QMouseEvent* event);
    void mouseWheel();

    void PlayStop();
    void Play();
    void Stop();
    void Info();
    void ZoomIn();
    void ZoomOut();
    void addPKTtoTable(unsigned char *);
    void SendAPO();
    void DDS(const QString st);


public slots:
    void getConfigs(int value, QTableWidget *tab_settings);


private:
    Ui::MainWindow *ui;
    QUdpSocket *udpSocket;
    QTcpSocket *tcpSocket;
    QThread thrUdp;

    QTcpServer* m_ptcpServer;
    QTextEdit*  m_ptxt;
    quint16     m_nNextBlockSize;

    QTcpServer *server;
    unsigned int packet_count, cnt_oscillo, adc_cnt;
    QByteArray datagram;
    QByteArray data_tr;
    QByteArray adc_data;
    QTimer timer_data, timer_sec;
    char data_ok;
    double m1, m2, dtm;

    //char mem_data[250000]; //25 Mb
    //unsigned char *p;
    //unsigned long int j;
    //unsigned char time_cnt;
    //unsigned long int mem_cnt;

    uint16_t zero_cross_inx;
    bool zero_lock;
    char state;
    uint8_t play_stop;

    //unsigned char adc[50000];
    //unsigned char adc_ok[50000];

    unsigned char adc_t1[500000];
    //unsigned char adc_t2[150000];
    //unsigned char adc_t3[150000];
    //unsigned char adc_t4[150000];

    unsigned char adc_m1[500000];
    //unsigned char adc_m2[150000];
    //unsigned char adc_m3[150000];
    //unsigned char adc_m4[150000];

    unsigned long int j1;
    unsigned long int j2;
    unsigned long int j3;
    unsigned long int j4;

    int timer;
    int lock_data;
    int average_state;

    QCPBars *H_ch1;

    //QTextEdit   *txt;
    QAction     *actPlayStop;
    QAction     *actSettings;
    QAction     *actInfo;
    QAction     *actSPS;
    QAction     *actZoomIn;
    QAction     *actZoomOut;
    QCustomPlot *qp_zoom;


    QCustomPlot *scope_graphs[256];
    QCustomPlot *fft_graphs[256];

    QCustomPlot *scope_plot[256];
    QCustomPlot *fft_plot[256];

    QTextEdit    *console1;
    QTextEdit    *console3;

    QTableWidget *console2;
    //QPlainTextEdit *console1;
    QTextCursor    prev_cursor;

    int line_cnt1;
    int pct_cnt1;
    int pct_cnt1_old;

    int line_cnt2;
    int pct_cnt2;
    int pct_cnt2_old;


    int diff1;
    int diff2;


    //QCPGraph *fft3_graph1;
    //QCPGraph *fft3_graph2;

    QTableWidget *table;
    QHostAddress hosts[256];

    QLineEdit apo_byte_1;
    QLineEdit apo_byte_2;
    QLineEdit apo_byte_3_4_5_6;
    QLineEdit apo_byte_7_8_9_10;
    QLineEdit apo_byte_11_12_13_14;
    QLineEdit apo_byte_15_16_17_18;

    //SettingsDialog *m_settings = nullptr;

    Settings *m_settings = nullptr;


    //void sendToClient(QTcpSocket* pSocket, const QString& str);

    quint16 len;


};

#endif // MAINWINDOW_H
