#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QtMath>
#include <QTabBar>

#include "settingsdialog.h"
#include "tabstyle.h"

#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* calloc, exit, free */

#include <complex>


#include <fstream>
#include <complex>
#include <vector>
#include <cstdlib>
#include <fftw3.h>
#include <math.h>       /* log10 */


using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    //m_settings(new SettingsDialog)
    m_settings(new Settings)
{
    ui->setupUi(this);

    connect(m_settings, SIGNAL(setConfigs(int, QTableWidget *)), this, SLOT(getConfigs(int, QTableWidget *)));
    MinimumHeight=100;
    ZoomVal=100;

    lock_data=0;
    rms_count=0;


    QToolBar *ToolBar = addToolBar(tr("Панель инструментов"));
    actPlayStop = new QAction(tr("&Пуск/Стоп"), this);
    //actPlayStop->setIcon(QIcon(":/images/icons8-color-96_play.png"));
    actPlayStop->setIcon(QIcon(":/images/Play_48x48.png"));
    connect(actPlayStop, &QAction::triggered, this, &MainWindow::PlayStop);
    play_stop=0;

    actSettings = new QAction(tr("&Настройки"), this);
    actSettings->setIcon(QIcon(":/images/Settings_48x48.png"));
    connect(actSettings, &QAction::triggered, m_settings, &Settings::show);

    actInfo = new QAction(tr("&О программе"), this);
    actInfo->setIcon(QIcon(":/images/Information_48x48.png"));
    connect(actInfo, &QAction::triggered, this, &MainWindow::Info);

    actZoomIn = new QAction(tr("&Увеличить по оси"), this);
    actZoomIn->setIcon(QIcon(":/images/Zoom In_48x48.png"));
    connect(actZoomIn, &QAction::triggered, this, &MainWindow::ZoomIn);

    actZoomOut = new QAction(tr("&Уменьшить по оси"), this);
    actZoomOut->setIcon(QIcon(":/images/Zoom Out_48x48.png"));
    connect(actZoomOut, &QAction::triggered, this, &MainWindow::ZoomOut);

    //actZoomIn->setEnabled(false);
    //actZoomOut->setEnabled(false);
    actPlayStop->setEnabled(false);

    ToolBar->setContextMenuPolicy(Qt::PreventContextMenu);

    this->setWindowIcon(QIcon(":/images/icon3.png"));
    //this->setWindowTitle("BUKS2: Система сбора данных с ГПБА");
    this->setWindowTitle("FACTOR: Система сбора данных");


    //QLineEdit * edit(new QLineEdit(this));
    //QString ipRange = "(?:[0-2]?[0-9]?[0-9]?[0-9]     | [2]?[0]?[0-4]?[0-8])";  //|205[0-5]  25[0-5]|2[0-4][0-‌​9]|[01]?[0-9][0-9]?
    //QString ipRange = "([2]?[0]?[0-4]?[0-8]  )";                  //?:[0-1]?[0-9]?[0-9]?[0-9]     ||
    //QRegExp ipRegex ("^" + ipRange + "$");
    //QRegExp ipRegex("^[1-2048]\\d{0,3}  | 204[0-8]$");

//    QRegExp ipRegex("^[1-2]{1}[0-9]{1}[0-9]{1}[0-9]{1}$");
//    QRegExpValidator *Validator = new QRegExpValidator(ipRegex, edit);
//    edit->setValidator(Validator);
//    edit->setMaximumWidth(50);
//    edit->setText("2048");

    ToolBar->setIconSize(QSize(35, 35));
    ToolBar->addAction(actPlayStop);
    ToolBar->addAction(actSettings);
    ToolBar->addAction(actZoomIn);
    ToolBar->addAction(actZoomOut);
    ToolBar->addAction(actInfo);
    //ToolBar->addWidget(edit);

    // Main layouts
    vLay = new QVBoxLayout(ui->centralWidget);
    hLay = new QHBoxLayout();

    vLay->setSpacing(0);
    vLay->setMargin(0);
    vLay->setContentsMargins(2,1,2,0);//int left, int top, int right, int bottom

    hLay->setSpacing(0);
    hLay->setMargin(0);
    hLay->setContentsMargins(0,0,0,0);

    window = new QWidget;
    QPalette p = palette();
    p.setColor(QPalette::Background, Qt::darkGray);
    window->setAutoFillBackground(true);
    window->setPalette(p);

    window_l = new QVBoxLayout();
    //window_l->addWidget(tw);
    window->setLayout(window_l);

    vLay->addLayout(hLay);
    //hLay->addWidget(tw);
    hLay->addWidget(window);
    //hLay->addWidget(scrollarea);

    table = new QTableWidget();


    //len=1024;
    //mem_cnt=0;
    //j=0;
    //zero_cross_inx=0;
    //zero_lock=0;
    //state=0;

    memset(adc_t1,0x00,sizeof(adc_t1));
    //memset(adc_t2,0x00,sizeof(adc_t2));
    //memset(adc_t3,0x00,sizeof(adc_t3));
    //memset(adc_t4,0x00,sizeof(adc_t4));

    memset(adc_m1,0x00,sizeof(adc_m1));
    //memset(adc_m2,0x00,sizeof(adc_m2));
    //memset(adc_m3,0x00,sizeof(adc_m3));
    //memset(adc_m4,0x00,sizeof(adc_m4));

    j1=0;
    j2=0;
    j3=0;
    j4=0;

    line_cnt1=0;
    pct_cnt1=0;

    line_cnt2=0;
    pct_cnt2=0;

    temp_U1rms=0;
    temp_U2rms=0;
    temp_U3rms=0;
    temp_U4rms=0;
    temp_U5rms=0;
    temp_U6rms=0;
    temp_U7rms=0;
    temp_U8rms=0;

    /*U1_rms=0;
    U1_amp=0;
    U1_pk=0;*/



    //adc_data.resize(1412);
    //datagram.resize(1412);

    adc_data.resize(1390);
    datagram.resize(1390);


    udpSocket = new QUdpSocket(this);
    udpSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption,2000000);

    udpSocket->bind(QHostAddress::AnyIPv4,25700,QUdpSocket::ShareAddress);
    connect(udpSocket, &QUdpSocket::readyRead,this, &MainWindow::readPendingDatagrams);
    //connect(udpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(ErrorUDP(QAbstractSocket::SocketError)));
    connect(&this->timer_sec, SIGNAL(timeout()), this, SLOT(Replot()));




    //online video
//    QThread *thUDP = new QThread;
//    UDP_port *PortNew = new UDP_port();                                                 //Создаем обьект
//    PortNew->moveToThread(thUDP);                                                       //помещаем класс в поток
//    PortNew->thisPort.moveToThread(thUDP);                                              //Помещаем сам порт в поток

//    connect(thUDP, SIGNAL(started()), PortNew, SLOT(init()));                           //Переназначения метода run
//    connect(thUDP, SIGNAL(finished()), PortNew, SLOT(deleteLater()));                   //Удалить к чертям поток
//    connect(PortNew, SIGNAL(pktTotable(unsigned char *)), this, SLOT(addPKTtoTable(unsigned char *)));
//    thUDP->start();
//    thUDP->setPriority(QThread::HighestPriority);


}


void MainWindow::addPKTtoTable(unsigned char *p){
  int diff_pkt;
  uint16_t pkt;
  int k;
  qDebug() << "addPKTtoTable";


  k=0;
  for(int i=0; i<10000; i++)
  {
      memcpy(&pkt,&p[k],2);k+=2;

//      if( play_stop && console2 != nullptr){

//          //qDebug() << "addPKTtoTable";


//          diff_pkt = abs(pkt - pct_cnt_old);
//          if( i>0 && diff_pkt > 1){
//              for(int q=0; q<(diff_pkt-1); q++){

//                  line_cnt++;
//                  console2->insertRow(console2->rowCount());
//                  console2->setItem  (console2->rowCount()-1, 0, new QTableWidgetItem(QString::number(line_cnt)));
//                  console2->item     (console2->rowCount()-1, 0)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);

//                  console2->setItem  (console2->rowCount()-1, 1, new QTableWidgetItem("XXXX"));
//                  console2->item     (console2->rowCount()-1, 1)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);

//                  console2->setItem  (console2->rowCount()-1, 2, new QTableWidgetItem("Пропуск пакета!!!"));
//                  console2->item     (console2->rowCount()-1, 2)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
//              }
//          }
//          else{
//              line_cnt++;
//              console2->insertRow(console2->rowCount());
//              console2->setItem  (console2->rowCount()-1, 0, new QTableWidgetItem(QString::number(line_cnt)));
//              console2->item     (console2->rowCount()-1, 0)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);

//              //console2->setItem  (console2->rowCount()-1, 1, new QTableWidgetItem(QString::number(pkt)));
//              console2->setItem  (console2->rowCount()-1, 1, new QTableWidgetItem(QString(" 0x%2").arg(pkt,   2, 16, QChar('0'))));

//              console2->item     (console2->rowCount()-1, 1)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
//          }

//          if(pkt == pct_cnt_old){
//              console2->setItem  (console2->rowCount()-1, 2, new QTableWidgetItem("Дубликат!!!"));
//              console2->item     (console2->rowCount()-1, 2)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
//          }

//          pct_cnt_old = pkt;
//      }
  }
}

void resetLayout(QLayout* apLayout)
{
    qDebug() << "resetLayout:";
    QLayoutItem *vpItem;
    while ((vpItem = apLayout->takeAt(0)) != nullptr)  {
        if (vpItem->layout()) {
            resetLayout(vpItem->layout());
            vpItem->layout()->deleteLater();
        }
        if (vpItem->widget()) {
            vpItem->widget()->deleteLater();
        }
        delete vpItem;
    }
}

void MainWindow::getConfigs(int val, QTableWidget *tab_settings)
{
    MinimumHeight=val;

    //save to local
    table = tab_settings;

    qDebug() << "MainWindow::getConfigs():";
    qDebug() << "val: " << QString::number(val);
    qDebug() << "window_l->count(): " << QString::number(window_l->count());
    qDebug() << "tab_settings->rowCount: " << tab_settings->rowCount();

    //Clear Layout
    if(window_l->count() > 0){
        resetLayout(window_l);
    }
    qDebug() << "window_l->count(): " << QString::number(window_l->count());

    if(window_l->count() == 0){

        //actZoomIn->setEnabled(false);
        //actZoomOut->setEnabled(false);
        actPlayStop->setEnabled(false);

        if(tab_settings->rowCount()>0)
        {
            //actZoomIn->setEnabled(true);
            //actZoomOut->setEnabled(true);
            actPlayStop->setEnabled(true);

            tw = new QTabWidget(this);
            tw->setIconSize(QSize(35, 35));
            tw->setStyleSheet("QTabBar::scroller { border: none; width: 70px; background-color: rgba(0,0,0,55);}");

            //memset(&hosts[0], 0x00, sizeof (hosts));
            //for(int i=0, j=0; i<tab_settings->rowCount(); i++, j+=32)
            for(int i=0, j=0; i<tab_settings->rowCount(); i++, j+=85)
            {
              auto scope = new QWidget;

              //Create 32 chanels
              //for(int k=0;k<32;k++)
              ZoomVal=100;
              //Create 85 chanels
              for(int k=0;k<85;k++)
              //for(int k=0;k<3;k++)
              {
                  //SCOPE 85ch
                  scope_plot[k] = new QCustomPlot(this);
                  scope_plot[k]->setObjectName("ch"+QString::number(k+1+j));
                  scope_plot[k]->addGraph();
                  scope_plot[k]->xAxis->setRange(0,2047);
                  scope_plot[k]->yAxis->setRange(-6,6);
                  scope_plot[k]->setMinimumHeight(ZoomVal);
                  scope_plot[k]->graph(0)->setName("Канал "+QString::number(k+1+j));
                  scope_plot[k]->legend->setVisible(true);
                  scope_plot[k]->legend->setFont(QFont(QFont().family(), 8));
                  scope_plot[k]->legend->setBrush(QColor(255, 255, 255, 150));
                  scope_plot[k]->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
                  scope_plot[k]->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
                  scope_plot[k]->setBackground(QBrush(Qt::black));
                  scope_plot[k]->graph(0)->setPen(QPen(Qt::green));
                  scope_plot[k]->xAxis->setBasePen(QPen(Qt::white, 1));
                  scope_plot[k]->yAxis->setBasePen(QPen(Qt::white, 1));
                  scope_plot[k]->xAxis->setTickPen(QPen(Qt::white, 1));
                  scope_plot[k]->yAxis->setTickPen(QPen(Qt::white, 1));
                  scope_plot[k]->xAxis->setSubTickPen(QPen(Qt::white, 1));
                  scope_plot[k]->yAxis->setSubTickPen(QPen(Qt::white, 1));
                  scope_plot[k]->xAxis->setTickLabelColor(Qt::white);
                  scope_plot[k]->yAxis->setTickLabelColor(Qt::white);
                  scope_plot[k]->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
                  scope_plot[k]->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
                  scope_plot[k]->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
                  scope_plot[k]->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
                  scope_plot[k]->xAxis->grid()->setSubGridVisible(true);
                  scope_plot[k]->yAxis->grid()->setSubGridVisible(false);
                  scope_plot[k]->xAxis->grid()->setZeroLinePen(Qt::NoPen);
                  scope_plot[k]->yAxis->grid()->setZeroLinePen(Qt::NoPen);
                  scope_plot[k]->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
                  scope_plot[k]->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

                  //FFT 85ch
                  fft_plot[k] = new QCustomPlot(this);
                  fft_plot[k]->setObjectName("fft"+QString::number(k+j+1));
                  fft_plot[k]->addGraph();
                  //fft_plot[k]->xAxis->setRange(0,105000);
                  fft_plot[k]->xAxis->setRange(-10050,10050);
                  fft_plot[k]->yAxis->setRange(-180,10);
                  fft_plot[k]->setMinimumHeight(ZoomVal);
                  fft_plot[k]->graph(0)->setName("Канал "+QString::number(k+j+1));
                  fft_plot[k]->legend->setVisible(true);
                  fft_plot[k]->legend->setFont(QFont(QFont().family(), 8));
                  fft_plot[k]->legend->setBrush(QColor(255, 255, 255, 150));
                  fft_plot[k]->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
                  fft_plot[k]->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
                  fft_plot[k]->setBackground(QBrush(Qt::black));
                  fft_plot[k]->graph(0)->setPen(QPen(Qt::green));
                  fft_plot[k]->xAxis->setBasePen(QPen(Qt::white, 1));
                  fft_plot[k]->yAxis->setBasePen(QPen(Qt::white, 1));
                  fft_plot[k]->xAxis->setTickPen(QPen(Qt::white, 1));
                  fft_plot[k]->yAxis->setTickPen(QPen(Qt::white, 1));
                  fft_plot[k]->xAxis->setSubTickPen(QPen(Qt::white, 1));
                  fft_plot[k]->yAxis->setSubTickPen(QPen(Qt::white, 1));
                  fft_plot[k]->xAxis->setTickLabelColor(Qt::white);
                  fft_plot[k]->yAxis->setTickLabelColor(Qt::white);
                  fft_plot[k]->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
                  fft_plot[k]->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
                  fft_plot[k]->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
                  fft_plot[k]->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
                  fft_plot[k]->xAxis->grid()->setSubGridVisible(true);
                  fft_plot[k]->yAxis->grid()->setSubGridVisible(true);
                  fft_plot[k]->xAxis->grid()->setZeroLinePen(Qt::NoPen);
                  fft_plot[k]->yAxis->grid()->setZeroLinePen(Qt::NoPen);
                  fft_plot[k]->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
                  fft_plot[k]->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
              }

              QVBoxLayout *scope_l = new QVBoxLayout();
              for(int i=0;i<85;i++){
              //for(int i=0;i<3;i++){
                scope_l->addWidget(scope_plot[i]);
              }

              QVBoxLayout *fft_l = new QVBoxLayout();
              for(int i=0;i<85;i++){
              //for(int i=0;i<3;i++){
                fft_l->addWidget(fft_plot[i]);
              }


              /****************************************************************************/

              if(i==0){

                  //RMS Layout
//                  QHBoxLayout *label_l_rms = new QHBoxLayout();

//                  label_ch1_rms = new QLabel();
//                  label_ch1_rms->setText("Ch1(rms) = 0.0");
//                  label_l_rms->addWidget(label_ch1_rms);

//                  label_ch2_rms = new QLabel();
//                  label_ch2_rms->setText("Ch2(rms) = 0.0");
//                  label_l_rms->addWidget(label_ch2_rms);

//                  label_ch3_rms = new QLabel();
//                  label_ch3_rms->setText("Ch3(rms) = 0.0");
//                  label_l_rms->addWidget(label_ch3_rms);

//                  label_ch4_rms = new QLabel();
//                  label_ch4_rms->setText("Ch4(rms) = 0.0");
//                  label_l_rms->addWidget(label_ch4_rms);

//                  label_ch5_rms = new QLabel();
//                  label_ch5_rms->setText("Ch5(rms) = 0.0");
//                  label_l_rms->addWidget(label_ch5_rms);

//                  label_ch6_rms = new QLabel();
//                  label_ch6_rms->setText("Ch6(rms) = 0.0");
//                  label_l_rms->addWidget(label_ch6_rms);

//                  label_ch7_rms = new QLabel();
//                  label_ch7_rms->setText("Ch7(rms) = 0.0");
//                  label_l_rms->addWidget(label_ch7_rms);

//                  label_ch8_rms = new QLabel();
//                  label_ch8_rms->setText("Ch8(rms) = 0.0");
//                  label_l_rms->addWidget(label_ch8_rms);

//                  auto label_w_rms = new QWidget;
//                  label_w_rms->setLayout(label_l_rms);
//                  fft_l->addWidget(label_w_rms);


                  //*********************************************************************/
                  //AMP Layout
//                  QHBoxLayout *label_l_amp = new QHBoxLayout();

//                  label_ch1_amp = new QLabel();
//                  label_ch1_amp->setText("Ch1(amp) = 0.0");
//                  label_l_amp->addWidget(label_ch1_amp);

//                  label_ch2_amp = new QLabel();
//                  label_ch2_amp->setText("Ch2(amp) = 0.0");
//                  label_l_amp->addWidget(label_ch2_amp);

//                  label_ch3_amp = new QLabel();
//                  label_ch3_amp->setText("Ch3(amp) = 0.0");
//                  label_l_amp->addWidget(label_ch3_amp);

//                  label_ch4_amp = new QLabel();
//                  label_ch4_amp->setText("Ch4(amp) = 0.0");
//                  label_l_amp->addWidget(label_ch4_amp);

//                  label_ch5_amp = new QLabel();
//                  label_ch5_amp->setText("Ch5(amp) = 0.0");
//                  label_l_amp->addWidget(label_ch5_amp);

//                  label_ch6_amp = new QLabel();
//                  label_ch6_amp->setText("Ch6(amp) = 0.0");
//                  label_l_amp->addWidget(label_ch6_amp);

//                  label_ch7_amp = new QLabel();
//                  label_ch7_amp->setText("Ch7(amp) = 0.0");
//                  label_l_amp->addWidget(label_ch7_amp);

//                  label_ch8_amp = new QLabel();
//                  label_ch8_amp->setText("Ch8(amp) = 0.0");
//                  label_l_amp->addWidget(label_ch8_amp);

//                  auto label_w_amp = new QWidget;
//                  label_w_amp->setLayout(label_l_amp);
//                  fft_l->addWidget(label_w_amp);


                  //*********************************************************************/
                  //PK Layout
//                  QHBoxLayout *label_l_pk = new QHBoxLayout();

//                  label_ch1_pk = new QLabel();
//                  label_ch1_pk->setText("Ch1(pk) = 0.0");
//                  label_l_pk->addWidget(label_ch1_pk);

//                  label_ch2_pk = new QLabel();
//                  label_ch2_pk->setText("Ch2(pk) = 0.0");
//                  label_l_pk->addWidget(label_ch2_pk);

//                  label_ch3_pk = new QLabel();
//                  label_ch3_pk->setText("Ch3(pk) = 0.0");
//                  label_l_pk->addWidget(label_ch3_pk);

//                  label_ch4_pk = new QLabel();
//                  label_ch4_pk->setText("Ch4(pk) = 0.0");
//                  label_l_pk->addWidget(label_ch4_pk);

//                  label_ch5_pk = new QLabel();
//                  label_ch5_pk->setText("Ch5(pk) = 0.0");
//                  label_l_pk->addWidget(label_ch5_pk);

//                  label_ch6_pk = new QLabel();
//                  label_ch6_pk->setText("Ch6(pk) = 0.0");
//                  label_l_pk->addWidget(label_ch6_pk);

//                  label_ch7_pk = new QLabel();
//                  label_ch7_pk->setText("Ch7(pk) = 0.0");
//                  label_l_pk->addWidget(label_ch7_pk);

//                  label_ch8_pk = new QLabel();
//                  label_ch8_pk->setText("Ch8(pk) = 0.0");
//                  label_l_pk->addWidget(label_ch8_pk);

//                  auto label_w_pk = new QWidget;
//                  label_w_pk->setLayout(label_l_pk);
//                  fft_l->addWidget(label_w_pk);

              }

              /****************************************************************************/

              auto scope_w = new QWidget;
              scope_w->setLayout(scope_l);

              auto fft_w = new QWidget;
              fft_w->setLayout(fft_l);




//              QVBoxLayout *control_vl = new QVBoxLayout();
//              QHBoxLayout *control_hl = new QHBoxLayout();

//              //lineEdit->setMaximumWidth(450);
//              //lineEdit->setText("12-13-A1B2C3D4-01020304-1ACFFC1D-55476312");

//              //QLineEdit *lineEdit1 = new QLineEdit();


//              //apo_byte_1.setInputMask("HH");
//              //apo_byte_1.setInputMask("999");
//              apo_byte_1.setText("31");
//              apo_byte_1.setMaximumWidth(100);


//              apo_byte_2.setInputMask("HH");
//              apo_byte_2.setText("00");
//              apo_byte_2.setMaximumWidth(100);

//              apo_byte_3_4_5_6.setInputMask("HHHHHHHH");
//              apo_byte_3_4_5_6.setText("1ACFFC1D");
//              apo_byte_3_4_5_6.setMaximumWidth(100);

//              apo_byte_7_8_9_10.setInputMask("HHHHHHHH");
//              apo_byte_7_8_9_10.setText("000003E8");//1000
//              apo_byte_7_8_9_10.setMaximumWidth(100);

//              apo_byte_11_12_13_14.setInputMask("HHHHHHHH");
//              apo_byte_11_12_13_14.setText("00000000");
//              apo_byte_11_12_13_14.setMaximumWidth(100);

//              apo_byte_15_16_17_18.setInputMask("HHHHHHHH");
//              apo_byte_15_16_17_18.setText("00000000");
//              apo_byte_15_16_17_18.setMaximumWidth(100);

//              QLabel *lb1 = new QLabel();
//              lb1->setText("Пакет управления:");
//              QSpacerItem  *sp1 = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

//              QPushButton *pb1 = new QPushButton();
//              pb1->setText("Отправить");
//              // подключаем сигнал к соответствующему слоту
//              connect(pb1, SIGNAL (released()), this, SLOT (SendAPO()));

//              control_hl->addWidget(lb1);
//              control_hl->addWidget(&apo_byte_1);
//              control_hl->addWidget(&apo_byte_2);
//              control_hl->addWidget(&apo_byte_3_4_5_6);
//              control_hl->addWidget(&apo_byte_7_8_9_10);
//              control_hl->addWidget(&apo_byte_11_12_13_14);
//              control_hl->addWidget(&apo_byte_15_16_17_18);
//              control_hl->addWidget(pb1);
//              control_hl->addSpacerItem(sp1);

//              control_vl->addLayout(control_hl);
//              control_vl->addSpacerItem(sp1);

//              auto control_w = new QWidget;
//              control_w->setLayout(control_vl);



              auto scroll_tab1 = new QScrollArea();
              scroll_tab1->setBackgroundRole(QPalette::Shadow);
              scroll_tab1->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
              scroll_tab1->setWidgetResizable(true);
              scroll_tab1->setWidget(scope_w);

              auto scroll_tab2 = new QScrollArea();
              scroll_tab2->setBackgroundRole(QPalette::Shadow);
              scroll_tab2->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
              scroll_tab2->setWidgetResizable(true);
              scroll_tab2->setWidget(fft_w);

//              auto scroll_tab3 = new QScrollArea();
//              scroll_tab3->setBackgroundRole(QPalette::Shadow);
//              scroll_tab3->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
//              scroll_tab3->setWidgetResizable(true);
//              scroll_tab3->setWidget(control_w);

              //****************************************************************//
              //Sub tab
              auto tabw = new QTabWidget();
              tabw->addTab(scroll_tab1, QIcon(":/images/oscillo.png"), "Канал");
              tabw->addTab(scroll_tab2, QIcon(":/images/bars.png"), "Спектр");
              //tabw->addTab(scroll_tab3, QIcon(""), "Управление");


             /* auto scroll_tab3 = new QScrollArea();
              scroll_tab3->setBackgroundRole(QPalette::Shadow);
              scroll_tab3->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
              scroll_tab3->setWidgetResizable(true);
              scroll_tab3->setWidget(label1);
              tabw->addTab(scroll_tab3,"");*/

              tabw->setTabPosition(QTabWidget::West);
              tabw->tabBar()->setStyle(new CustomTabStyle);
              tabw->setIconSize(QSize(30, 30));

              auto tw_l = new QVBoxLayout();
              tw_l->addWidget(tabw);

              scope->setLayout(tw_l);
              tw->addTab(scope, QIcon(":/images/ethernet_card.png"), tab_settings->item(i,1)->text());

              //Save HostAddress
              hosts[i] = tab_settings->item(i,1)->text();
            }





            QVBoxLayout *control_vl  = new QVBoxLayout();
            //QHBoxLayout *control_hl1 = new QHBoxLayout();
            //QHBoxLayout *control_hl2 = new QHBoxLayout();

            QGridLayout *glayout1    = new QGridLayout;

            glayout1->setContentsMargins(0, 5, 10, 5);
            glayout1->setAlignment(Qt::AlignLeft);


            apo_byte_1.setText("31");
            apo_byte_1.setMaximumWidth(100);

            apo_byte_2.setInputMask("HH");
            apo_byte_2.setText("00");
            apo_byte_2.setMaximumWidth(100);

            apo_byte_3_4_5_6.setInputMask("HHHHHHHH");
            apo_byte_3_4_5_6.setText("1ACFFC1D");
            apo_byte_3_4_5_6.setMaximumWidth(100);

            apo_byte_7_8_9_10.setInputMask("HHHHHHHH");
            apo_byte_7_8_9_10.setText("000003E8");//1000
            apo_byte_7_8_9_10.setMaximumWidth(100);

            apo_byte_11_12_13_14.setInputMask("HHHHHHHH");
            apo_byte_11_12_13_14.setText("00000000");
            apo_byte_11_12_13_14.setMaximumWidth(100);

            apo_byte_15_16_17_18.setInputMask("HHHHHHHH");
            apo_byte_15_16_17_18.setText("00000000");
            apo_byte_15_16_17_18.setMaximumWidth(100);

            //QLabel *lb1 = new QLabel();
            //lb1->setText("Пакет управления:\t");
            //lb1->setText("\t");
            //QSpacerItem  *sp1 = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

            QPushButton *pb1 = new QPushButton();
            pb1->setText("Отправить");
            pb1->setStyleSheet("QPushButton:pressed { background-color: red }");
            // подключаем сигнал к соответствующему слоту
            connect(pb1, SIGNAL (released()), this, SLOT (SendAPO()));

            //control_hl1->addWidget(lb1);
//            control_hl1->addWidget(&apo_byte_1);
//            control_hl1->addWidget(&apo_byte_2);
//            control_hl1->addWidget(&apo_byte_3_4_5_6);
//            control_hl1->addWidget(&apo_byte_7_8_9_10);
//            control_hl1->addWidget(&apo_byte_11_12_13_14);
//            control_hl1->addWidget(&apo_byte_15_16_17_18);
//            control_hl1->addWidget(pb1);
//            control_hl1->addSpacerItem(sp1);

//            auto apo_w1 = new QWidget;
//            apo_w1->setLayout(control_hl1);



            //QLabel *lb2 = new QLabel();
            //lb2->setText("\t\t\t\t\t\t\t");

            QLineEdit *le1 = new QLineEdit();
            le1->setMaximumWidth(100);
            le1->setText("");
            //le1->setValidator(new QDoubleValidator( 1.00, 16.00, 2, le1));
            le1->setValidator(new QIntValidator(0, 999999,le1));

//            QLineEdit *le2 = new QLineEdit();
//            le2->setMaximumWidth(100);
//            le2->setText("2");

//            QLineEdit *le3 = new QLineEdit();
//            le3->setMaximumWidth(100);
//            le3->setText("3");

//            QLineEdit *le4 = new QLineEdit();
//            le4->setMaximumWidth(100);
//            le4->setText("4");

//            QLineEdit *le5 = new QLineEdit();
//            le5->setMaximumWidth(100);
//            le5->setText("5");

//            QLineEdit *le6 = new QLineEdit();
//            le6->setMaximumWidth(100);
//            le6->setText("6");

//            QLineEdit *le7 = new QLineEdit();
//            le7->setMaximumWidth(100);
//            le7->setText("7");


            //control_hl2->addWidget(lb2);
            //control_hl2->addWidget(le1);
            //control_hl2->addWidget(le2);
            //control_hl2->addWidget(le3);
            //control_hl2->addSpacerItem(sp1);


//            glayout1->addWidget(le1, 0, 0, Qt::AlignLeft);
//            glayout1->addWidget(le2, 0, 1, Qt::AlignLeft);
//            glayout1->addWidget(le3, 0, 2, Qt::AlignLeft);
//            glayout1->addWidget(le4, 0, 3, Qt::AlignLeft);
//            glayout1->addWidget(le5, 0, 4, Qt::AlignLeft);
//            glayout1->addWidget(le6, 0, 5, Qt::AlignLeft);

//            glayout1->addWidget(le7, 1, 4, Qt::AlignLeft);


            glayout1->addWidget(&apo_byte_1,            0, 0, Qt::AlignLeft);
            glayout1->addWidget(&apo_byte_2,            0, 1, Qt::AlignLeft);
            glayout1->addWidget(&apo_byte_3_4_5_6,      0, 2, Qt::AlignLeft);
            glayout1->addWidget(&apo_byte_7_8_9_10,     0, 3, Qt::AlignLeft);
            glayout1->addWidget(&apo_byte_11_12_13_14,  0, 4, Qt::AlignLeft);
            glayout1->addWidget(&apo_byte_15_16_17_18,  0, 5, Qt::AlignLeft);
            glayout1->addWidget(pb1,                    0, 6, Qt::AlignLeft);
            glayout1->addWidget(le1,                    1, 4, Qt::AlignLeft);

            auto apo_w2 = new QWidget;
            //apo_w2->setLayout(control_hl2);
            apo_w2->setLayout(glayout1);


//            control_hl2->addWidget(apo_w2);
//            control_hl2->addSpacerItem(sp1);
//            auto apo_w3 = new QWidget;
//            apo_w3->setLayout(control_hl2);



            control_vl->addWidget(apo_w2);
            //control_vl->addWidget(apo_w1);
            control_vl->addWidget(tw);

            auto control_w = new QWidget;
            control_w->setLayout(control_vl);
            window_l->addWidget(control_w);


            connect(le1, SIGNAL(textChanged(const QString &)), this, SLOT(DDS(const QString &)));


            //window_l->addWidget(tw);
            //window_l->addWidget(tw);
            //auto console_w = new QWidget;
            //auto txt = new QPlainTextEdit();



            /*QPalette p = palette();
            p.setColor(QPalette::Base, Qt::black);
            p.setColor(QPalette::Text, Qt::green);

            auto txt1 = new QTextEdit();
            txt1->setObjectName("console1");
            txt1->setPalette(p);

            auto txt2 = new QTextEdit();
            txt2->setObjectName("console3");
            txt2->setPalette(p);
*/

//            QStringList JEheader, TPheader;
//            console2 = new QTableWidget();
//            console2->setObjectName("console2");
//            // Create table of parametrs
//            console2->setSelectionBehavior(QAbstractItemView::SelectRows);
//            //console2->setItemDelegate(new Delegate);
//            //config->setRowCount(2);
//            console2->setColumnCount(3);
//            TPheader<<"№"<<"Счетчик пакета"<<"Статус";
//            console2->setHorizontalHeaderLabels(TPheader);
//            console2->verticalHeader()->setVisible(false);
//            //console2->verticalHeader()->setFixedWidth(100);

//            console2->setColumnWidth( 1, 200 );
//            console2->horizontalHeader()->setSectionResizeMode( 2, QHeaderView::Stretch );


//            console2->horizontalHeader()->setVisible(true);
//            //config->horizontalHeader()->setMode( 1, QHeaderView::Stretch );
//            //console2->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
//            //console2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//            connect(console2->model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), console2, SLOT(scrollToBottom()));

/*
            console2->insertRow(console2->rowCount());
            console2->setItem  (console2->rowCount()-1, 0, new QTableWidgetItem("Канал [ " + QString::number(1) + " - " + QString::number(8) + " ]" ));
            console2->item     (console2->rowCount()-1, 0)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
            console2->item     (console2->rowCount()-1, 0)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

            console2->setItem  (console2->rowCount()-1, 1, new QTableWidgetItem("192.168.4." + QString::number(2)));
            console2->item     (console2->rowCount()-1, 1)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);*/


            //tw->addTab(txt1,  "Отладка №1");
            //tw->addTab(txt2,  "Отладка №2");
            //window_l->addWidget(tw);




            //    txt = new QTextEdit();
            //    //txt->setText("Hello, world!");
            //    //txt->append("Appending some text…");

            //    QPalette p = palette();
            //    p.setColor(QPalette::Base, Qt::black);
            //    p.setColor(QPalette::Text, Qt::green);
            //    txt->setPalette(p);

        }
    }
}


void MainWindow::DDS(const QString st){
    qDebug() << "DDS " + st;

    //unsigned long int val = st.toUInt(nullptr,10);

    double temp = st.toDouble();
    //double dds_word = (temp/200000.0)*4294967296.0;
    //double dds_word = (temp/200000.0) * 4294967296.0;
    //double dds_word = (temp/200000.0) * 16777216.0;


    double dds_word = (temp/125000000.0) * 4294967296.0;
    //round = int(dds_word+0.5);

    int round = int(dds_word+0.5);
    qDebug() << "dds_word: " + QString::number(round);
    apo_byte_11_12_13_14.setText(QString("%1").arg((unsigned long int)round, 8, 16, QLatin1Char( '0' )));

    //qDebug() << "dds_word: " + QString::number((unsigned long int)dds_word);

    //apo_byte_11_12_13_14.setText(QByteArray::fromHex(QString::number((unsigned long int)dds_word)));


    //apo_byte_11_12_13_14.setText(QString("%1").arg((unsigned long int)dds_word, 8, 16, QLatin1Char( '0' )));

}



void MainWindow::SendAPO(){


    unsigned char buffer[50];

    unsigned char temp1[4];
    unsigned char temp2[4];
    unsigned char temp3[4];
    unsigned char temp4[4];

    QString str1;
    QString str2;
    QString str3;
    QString str4;
    QString str5;
    QString str6;

    str1 = apo_byte_1.text();
    str2 = apo_byte_2.text();
    str3 = apo_byte_3_4_5_6.text();
    str4 = apo_byte_7_8_9_10.text();
    str5 = apo_byte_11_12_13_14.text();
    str6 = apo_byte_15_16_17_18.text();

    //QString s1      = apo_byte_1.text();
    //QByteArray ar   = QByteArray::fromHex(s1.toUtf8());
    //qDebug() << "ar: "           << ar;

    QByteArray byte1            = QByteArray::fromHex(str1.toUtf8());
    QByteArray byte2            = QByteArray::fromHex(str2.toUtf8());
    QByteArray byte3_4_5_6      = QByteArray::fromHex(str3.toUtf8());
    QByteArray byte7_8_9_10     = QByteArray::fromHex(str4.toUtf8());
    QByteArray byte11_12_13_14  = QByteArray::fromHex(str5.toUtf8());
    QByteArray byte15_16_17_18  = QByteArray::fromHex(str6.toUtf8());

    memcpy(&buffer[0], byte1,           1);
    memcpy(&buffer[1], byte2,           1);

    memcpy(&temp1[0],  byte3_4_5_6,     4);
    memcpy(&temp2[0],  byte7_8_9_10,    4);
    memcpy(&temp3[0],  byte11_12_13_14, 4);
    memcpy(&temp4[0],  byte15_16_17_18, 4);


//    memcpy(&buffer[0], byte1,           1);
//    memcpy(&buffer[1], byte2,           1);
//    memcpy(&buffer[2], byte3_4_5_6,     4);
//    memcpy(&buffer[6], byte7_8_9_10,    4);
//    memcpy(&buffer[10],byte11_12_13_14, 4);
//    memcpy(&buffer[14],byte15_16_17_18, 4);

    buffer[2]  = temp1[3];
    buffer[3]  = temp1[2];
    buffer[4]  = temp1[1];
    buffer[5]  = temp1[0];

    buffer[6]  = temp2[3];
    buffer[7]  = temp2[2];
    buffer[8]  = temp2[1];
    buffer[9]  = temp2[0];

    buffer[10] = temp3[3];
    buffer[11] = temp3[2];
    buffer[12] = temp3[1];
    buffer[13] = temp3[0];

    buffer[14] = temp4[3];
    buffer[15] = temp4[2];
    buffer[16] = temp4[1];
    buffer[17] = temp4[0];



    //if(udpSocket->writeDatagram((const char*)buffer,18,QHostAddress("192.168.0.222"), 25900)<0) //hosts[0]
    if(udpSocket->writeDatagram((const char*)buffer,18,hosts[0], 25900)<0) //hosts[0]
    {
        qDebug()<<"Error: Discovery: writeDatagram failed "<<udpSocket->errorString();
        return;
    }
    udpSocket->flush();

}


typedef struct {
    QCPItemLine *hLine;
    QCPItemLine *vLine;
} QCPCursor;

bool cursorEnabled=true;

void ManageCursor(QCustomPlot *customPlot, QCPCursor *cursor, double x, double y, QPen pen)
{
    if(cursorEnabled)
    {
        /*if(cursor->hLine) customPlot->removeItem(cursor->hLine);
        cursor->hLine = new QCPItemLine(customPlot);
        customPlot->addItem(cursor->hLine);
        cursor->hLine->setPen(pen);
        cursor->hLine->start->setCoords(QCPRange::minRange, y);
        cursor->hLine->end->setCoords(QCPRange::maxRange, y);*/

        if(cursor->vLine) customPlot->removeItem(cursor->vLine);
        cursor->vLine = new QCPItemLine(customPlot);
        customPlot->hasItem(cursor->vLine);
        cursor->vLine->setPen(pen);
        cursor->vLine->start->setCoords( x, QCPRange::minRange);
        cursor->vLine->end->setCoords( x, QCPRange::maxRange);
    }
}

void MainWindow::mouseRelease(QMouseEvent* event)
{
//    QCustomPlot *customPlot=ui->Oscillo1;
//    static QCPCursor cursor1, cursor2;
//    double x=customPlot->xAxis->pixelToCoord(event->pos().x());
//    double y=customPlot->yAxis->pixelToCoord(event->pos().y());

//    if(event->button() == Qt::LeftButton)
//    {
//        ManageCursor(customPlot, &cursor1, x, y, QPen(Qt::red));
//        //ui->label_12->setText(QString::number(x));
//        qDebug() << "m1: " << QString::number(x);
//        m1=x;
//    }
//    else
//    {
//        ManageCursor(customPlot, &cursor2, x, y, QPen(Qt::blue));
//        //ui->label_13->setText(QString::number(x));
//        qDebug() << "m2: " << QString::number(x);
//        m2=x;
//    }


//    if(m1 > m2)
//        dtm=m1-m2;
//    else if(m2 > m1)
//        dtm=m2-m1;
//    qDebug() << "dt: " << QString::number(dtm);
//    //ui->label_15->setText(QString::number(dtm));


//    customPlot->replot();
//    cursorEnabled=true;
}

void MainWindow::mousePress(QMouseEvent* event)
{
//  // if an axis is selected, only allow the direction of that axis to be dragged
//  // if no axis is selected, both directions may be dragged

//  if (ui->Oscillo1->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
//    ui->Oscillo1->axisRect()->setRangeDrag(ui->Oscillo1->xAxis->orientation());

//  else if (ui->Oscillo1->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
//    ui->Oscillo1->axisRect()->setRangeDrag(ui->Oscillo1->yAxis->orientation());

//  else
//    ui->Oscillo1->axisRect()->setRangeDrag(Qt::Horizontal);
//    //ui->Oscillo1->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);




    /*QCustomPlot *customPlot=ui->Oscillo1;
    static QCPItemLine *hCursor, *vCursor;
    double x=customPlot->xAxis->pixelToCoord(event->pos().x());
    double y=customPlot->yAxis->pixelToCoord(event->pos().y());

    if(hCursor) customPlot->removeItem(hCursor);
    hCursor = new QCPItemLine(customPlot);
    customPlot->addItem(hCursor);
    hCursor->start->setCoords(QCPRange::minRange, y);
    hCursor->end->setCoords(QCPRange::maxRange, y);

    if(vCursor) customPlot->removeItem(vCursor);
    vCursor = new QCPItemLine(customPlot);
    customPlot->addItem(vCursor);
    vCursor->start->setCoords( x, QCPRange::minRange);
    vCursor->end->setCoords( x, QCPRange::maxRange);

    customPlot->replot();*/


}

void MainWindow::mouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed

//  if (ui->Oscillo1->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
//    ui->Oscillo1->axisRect()->setRangeZoom(ui->Oscillo1->xAxis->orientation());

//  else if (ui->Oscillo1->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
//    ui->Oscillo1->axisRect()->setRangeZoom(ui->Oscillo1->yAxis->orientation());

//  else
//    ui->Oscillo1->axisRect()->setRangeZoom(Qt::Horizontal);
}

MainWindow::~MainWindow()
{
    //thrUdp.quit();
    //thrUdp.wait();
    delete ui;
}

int sign(int n)
{
  if(n < 0) return -1;
  if(n > 0) return 1;
  return n;
}

int signf(float n)
{
  if(n < 0.0) return -1;
  if(n >= 0.0) return 1;
  return n;
}

int signd(double n)
{
  if(n < 0.0) return -1;
  if(n >= 0.0) return 1;
  return n;
}

// Fixed-point Format: 11.5 (16-bit)
typedef int16_t fixed_point_t;
#define FIXED_POINT_FRACTIONAL_BITS 5

/*inline fixed_point_t float_to_fixed(double input)
{
    return (fixed_point_t)(input * (1 << FIXED_POINT_FRACTIONAL_BITS));
}*/

inline fixed_point_t float_to_fixed(double input)
{
    return (fixed_point_t)(round(input * (1 << FIXED_POINT_FRACTIONAL_BITS)));
}

/* get sign of number */
bool getSign(int16_t data)
{
    if(data>0)      /* positif data */
        return (1);
    else            /* negatif data */
        return (0);
}

void MainWindow::PlayStop()
{

    average_state=0;

    //diff1=0;
    //diff2=0;

    play_stop=~play_stop;
    if(play_stop==0)
    {
        actSettings->setEnabled(true);
        actPlayStop->setIcon(QIcon(":/images/Play_48x48.png"));

        //if(console1 != nullptr) console1->append("diff1= " + QString::number(diff1) + "  Количество пропущенных пакетов");
        //if(console3 != nullptr) console3->append("diff2= " + QString::number(diff2) + "  Количество пропущенных пакетов");

        Stop();
    }
    else
    {
        actSettings->setEnabled(false);
        actPlayStop->setIcon(QIcon(":/images/Stop_48x48.png"));
        Play();
    }
}

void MainWindow::Play()
{
    diff1=0;
    diff2=0;


    auto qplot = new QCustomPlot();
    for(int i=1; i<87; i++){
        //Scope: clear & replot
        qplot = window->findChild<QCustomPlot *>("ch"+QString::number(i));
        if(qplot != nullptr) {
            qplot->graph(0)->data()->clear();
            qplot->replot();
        }

        //FFT: clear & replot
        qplot = window->findChild<QCustomPlot *>("fft"+QString::number(i));
        if(qplot != nullptr) {
            qplot->graph(0)->data()->clear();
            qplot->replot();
        }
    }
    delete qplot;

    //**************************************************************************************//
    //SCOPE & FFT
    for(int i=0;i<85;i++)
    {
       scope_graphs[i] = window->findChild<QCustomPlot *>("ch" +QString::number(i+1));
       fft_graphs[i]   = window->findChild<QCustomPlot *>("fft"+QString::number(i+1));
    }

    /*QList<QTextEdit *> allQTextEdit = window->findChildren<QTextEdit *>();
    //qDebug() << allWidgets;
    foreach( QTextEdit* QTextEditPtr, allQTextEdit )
    {
        qDebug() << "Child QTextEdit" << QTextEditPtr;
    }*/

    QList<QCustomPlot *> allQCustomPlot = window->findChildren<QCustomPlot *>();
    //qDebug() << allWidgets;
    foreach( QCustomPlot* QCustomPlotPtr, allQCustomPlot )
    {
        qDebug() << "Child QCustomPlot" << QCustomPlotPtr;
    }



    this->timer_sec.start(100); // 1sec
    //qDebug() <<  "Play\n";
}

void MainWindow::Stop()
{


    //diff1=0;
    //diff2=0;


//    auto qplot = new QCustomPlot();
//    for(int i=1; i<87; i++){
//        //Scope: clear & replot
//        qplot = window->findChild<QCustomPlot *>("ch"+QString::number(i));
//        if(qplot != nullptr) {
//            qplot->graph(0)->data()->clear();
//            qplot->replot();
//        }

//        //FFT: clear & replot
//        qplot = window->findChild<QCustomPlot *>("fft"+QString::number(i));
//        if(qplot != nullptr) {
//            qplot->graph(0)->data()->clear();
//            qplot->replot();
//        }
//    }
//    delete qplot;
    this->timer_sec.stop();
    //qDebug() <<  "Stop\n";
}

void MainWindow::Info()
{
//   qDebug() <<  "MainWindow::Info()";
//   QList<QCustomPlot *> allQCustomPlot = window->findChildren<QCustomPlot *>();
//   //qDebug() << allWidgets;
//   foreach( QCustomPlot* QCustomPlotPtr, allQCustomPlot )
//   {
//       qDebug() << "Child QCustomPlot" << QCustomPlotPtr;
//   }

   //QCustomPlot *qplot = window->findChild<QCustomPlot *>("ch1");

//   QCustomPlot *qplot = window->findChild<QCustomPlot *>();
//   if(qplot != nullptr){
//        qDebug() << "Found QCustomPlot!";

//        // generate some data:
//        QVector<double> x(101), y(101); // initialize with entries 0..100
//        for (int i=0; i<101; ++i)
//        {
//          x[i] = i/50.0 - 1; // x goes from -1 to 1
//          y[i] = x[i]*x[i]; // let's plot a quadratic function
//        }
//        // create graph and assign data to it:
//        //qplot->addGraph();
//        qplot->graph(0)->setData(x, y);
//        // give the axes some labels:
//        qplot->xAxis->setLabel("x");
//        qplot->yAxis->setLabel("y");
//        // set axes ranges, so we see all data:
//        qplot->xAxis->setRange(-1, 1);
//        qplot->yAxis->setRange(0, 1);
//        qplot->replot();
//    }
}

void MainWindow::ZoomIn()
{
    if(ZoomVal<300){
        ZoomVal+=10;
        for(int i=1; i<87; i++){
            qp_zoom = window->findChild<QCustomPlot *>("ch"+QString::number(i));
            if(qp_zoom != nullptr) qp_zoom->setMinimumHeight(ZoomVal);\

            qp_zoom = window->findChild<QCustomPlot *>("fft"+QString::number(i));
            if(qp_zoom != nullptr) qp_zoom->setMinimumHeight(ZoomVal);
        }
    }
}

void MainWindow::ZoomOut()
{
    if(ZoomVal>100){
        ZoomVal-=10;
        for(int i=1; i<87; i++){
            qp_zoom = window->findChild<QCustomPlot *>("ch"+QString::number(i));
            if(qp_zoom != nullptr) qp_zoom->setMinimumHeight(ZoomVal);

            qp_zoom = window->findChild<QCustomPlot *>("fft"+QString::number(i));
            if(qp_zoom != nullptr) qp_zoom->setMinimumHeight(ZoomVal);
        }
    }
}


/**
 * The reference bitreverse function.
 */
static int bitreverseReference(int j, int nu) {
    int j2;
    int j1 = j;
    int k = 0;
    for (int i = 1; i <= nu; i++) {
        j2 = j1 / 2;
        k = 2 * k + j1 - 2 * j2;
        j1 = j2;
    }
    return k;
}

#define PI 3.14159265359

static void fft(double inputReal[2048], double inputImag[2048], double out[4096], bool DIRECT) {
        // - n is the dimension of the problem
        // - nu is its logarithm in base e
        int n = 2048;

        // If n is a power of 2, then ld is an integer (_without_ decimals)
        double ld = log(n) / log(2.0);

        // Here I check if n is a power of 2. If exist decimals in ld, I quit
        // from the function returning null.
//        if (((int) ld) - ld != 0) {
//            System.out.println("The number of elements is not a power of 2.");
//            return null;
//        }



        // Declaration and initialization of the variables
        // ld should be an integer, actually, so I don't lose any information in
        // the cast
        int nu = (int) ld;
        int n2 = n / 2;
        int nu1 = nu - 1;
        double xReal[n];
        double xImag[n];
        double tReal, tImag, p, arg, c, s;

        // Here I check if I'm going to do the direct transform or the inverse
        // transform.
        double constant;
        if (DIRECT)
            constant = -2 * PI;
        else
            constant = 2 * PI;

        // I don't want to overwrite the input arrays, so here I copy them. This
        // choice adds \Theta(2n) to the complexity.
        for (int i = 0; i < n; i++) {
            xReal[i] = inputReal[i];
            xImag[i] = inputImag[i];
        }

        // First phase - calculation
        int k = 0;
        for (int l = 1; l <= nu; l++) {
            while (k < n) {
                for (int i = 1; i <= n2; i++) {
                    p = bitreverseReference(k >> nu1, nu);
                    // direct FFT or inverse FFT
                    arg = constant * p / n;
                    c = cos(arg);
                    s = sin(arg);
                    tReal = xReal[k + n2] * c + xImag[k + n2] * s;
                    tImag = xImag[k + n2] * c - xReal[k + n2] * s;
                    xReal[k + n2] = xReal[k] - tReal;
                    xImag[k + n2] = xImag[k] - tImag;
                    xReal[k] += tReal;
                    xImag[k] += tImag;
                    k++;
                }
                k += n2;
            }
            k = 0;
            nu1--;
            n2 /= 2;
        }

        // Second phase - recombination
        k = 0;
        int r;
        while (k < n) {
            r = bitreverseReference(k, nu);
            if (r > k) {
                tReal = xReal[k];
                tImag = xImag[k];
                xReal[k] = xReal[r];
                xImag[k] = xImag[r];
                xReal[r] = tReal;
                xImag[r] = tImag;
            }
            k++;
        }

        // Here I have to mix xReal and xImag to have an array (yes, it should
        // be possible to do this stuff in the earlier parts of the code, but
        // it here to readibility).
        //double newArray[4096];// = new double[xReal.length * 2];
        double radice = 1 / sqrt(n);
        for (int i = 0; i < 4096; i += 2) {
            int i2 = i / 2;
            // I used Stephen Wolfram Mathematica as a reference so I'm going
            // to normalize the output while I'm copying the elements.

            //newArray[i] = xReal[i2] * radice;
            //newArray[i + 1] = xImag[i2] * radice;

            out[i]   = xReal[i2] * radice;
            out[i+1] = xImag[i2] * radice;
        }
        //return newArray;
    }



void MainWindow::Replot()
{
    int i,j;
    QString str;
    double adc_t=0.0;
    double x;

    int16_t chanels[256];
    QVector<QCPGraphData> dataFFT3(2048);

    double scope_y[256];
    double fft_y[256];

    double pi = 3.14159265359;
    //double mean_fft3[5][2048];


    QVector<double> fft_amp_Ch1(2048);
    vector<complex<float> > fft_ch1(2048, 0.);
    vector<complex<float> > fft_ch2(2048, 0.);
    vector<complex<float> > fft_ch3(2048, 0.);
    vector<complex<float> > fft_ch4(2048, 0.);
    vector<complex<float> > fft_ch5(2048, 0.);
    vector<complex<float> > fft_ch6(2048, 0.);
    vector<complex<float> > fft_ch7(2048, 0.);
    vector<complex<float> > fft_ch8(2048, 0.);
    vector<complex<float> > fft_ch9(2048, 0.);
    vector<complex<float> > fft_ch10(2048, 0.);
    vector<complex<float> > fft_ch11(2048, 0.);
    vector<complex<float> > fft_ch12(2048, 0.);
    vector<complex<float> > fft_ch13(2048, 0.);
    vector<complex<float> > fft_ch14(2048, 0.);
    vector<complex<float> > fft_ch15(2048, 0.);
    vector<complex<float> > fft_ch16(2048, 0.);



    double re[2048], im[2048], out[4096];


    //float fft_ch14_t[2048];

    for(int i=0;i<128;i++)
    {
       //scope_graphs[i] = window->findChild<QCustomPlot  *>("ch"+QString::number(i));
       //fft_graphs[i]   = window->findChild<QCustomPlot *>("fft"+QString::number(i));
        if(scope_graphs[i] != nullptr) scope_graphs[i]->graph(0)->data()->clear();
        //if(fft_graphs[i]   != nullptr) fft_graphs[i]->graph(0)->data()->clear();
    }

    //if(console1 != nullptr) console1->clear();

    i=0;
    j=0;



    lock_data=1;

    //RMS
//    for(int i=0;i<=2048;i++)
//    {
//        memcpy(&ch1_t, &adc_m1[j],2);
//        memcpy(&ch2_t, &adc_m1[j+2],2);
//        memcpy(&ch3_t, &adc_m1[j+4],2);
//        memcpy(&ch4_t, &adc_m1[j+6],2);
//        memcpy(&ch5_t, &adc_m1[j+8],2);
//        memcpy(&ch6_t, &adc_m1[j+10],2);
//        memcpy(&ch7_t, &adc_m1[j+12],2);
//        memcpy(&ch8_t, &adc_m1[j+14],2);

//        temp_U1rms += qPow(ch1_t*0.000152, 2);
//        temp_U2rms += qPow(ch2_t*0.000152, 2);
//        temp_U3rms += qPow(ch3_t*0.000152, 2);
//        temp_U4rms += qPow(ch4_t*0.000152, 2);
//        temp_U5rms += qPow(ch5_t*0.000152, 2);
//        temp_U6rms += qPow(ch6_t*0.000152, 2);
//        temp_U7rms += qPow(ch7_t*0.000152, 2);
//        temp_U8rms += qPow(ch8_t*0.000152, 2);

//        j+=16;
//        rms_count++;

//        if(rms_count>=8192)
//        {
//            U1_rms = qSqrt( (1.0/8192.0)* temp_U1rms );
//            U1_amp = U1_rms * qSqrt(2.0);
//            U1_pk  = U1_amp * 2.0;

//            U2_rms = qSqrt( (1.0/8192.0)* temp_U2rms );
//            U2_amp = U2_rms * qSqrt(2.0);
//            U2_pk  = U2_amp * 2.0;

//            U3_rms = qSqrt( (1.0/8192.0)* temp_U3rms );
//            U3_amp = U3_rms * qSqrt(2.0);
//            U3_pk  = U3_amp * 2.0;

//            U4_rms = qSqrt( (1.0/8192.0)* temp_U4rms );
//            U4_amp = U4_rms * qSqrt(2.0);
//            U4_pk  = U4_amp * 2.0;

//            U5_rms = qSqrt( (1.0/8192.0)* temp_U5rms );
//            U5_amp = U5_rms * qSqrt(2.0);
//            U5_pk  = U5_amp * 2.0;

//            U6_rms = qSqrt( (1.0/8192.0)* temp_U6rms );
//            U6_amp = U6_rms * qSqrt(2.0);
//            U6_pk  = U6_amp * 2.0;

//            U7_rms = qSqrt( (1.0/8192.0)* temp_U7rms );
//            U7_amp = U7_rms * qSqrt(2.0);
//            U7_pk  = U7_amp * 2.0;

//            U8_rms = qSqrt( (1.0/8192.0)* temp_U8rms );
//            U8_amp = U8_rms * qSqrt(2.0);
//            U8_pk  = U8_amp * 2.0;

//            label_ch1_rms->setText("Ch1(rms) = "+QString::number(U1_rms));
//            label_ch2_rms->setText("Ch2(rms) = "+QString::number(U2_rms));
//            label_ch3_rms->setText("Ch3(rms) = "+QString::number(U3_rms));
//            label_ch4_rms->setText("Ch4(rms) = "+QString::number(U4_rms));
//            label_ch5_rms->setText("Ch5(rms) = "+QString::number(U5_rms));
//            label_ch6_rms->setText("Ch6(rms) = "+QString::number(U6_rms));
//            label_ch7_rms->setText("Ch7(rms) = "+QString::number(U7_rms));
//            label_ch8_rms->setText("Ch8(rms) = "+QString::number(U8_rms));

//            /*label_ch1_amp->setText("Ch1(amp) = "+QString::number(U1_amp));
//            label_ch2_amp->setText("Ch2(amp) = "+QString::number(U2_amp));
//            label_ch3_amp->setText("Ch3(amp) = "+QString::number(U3_amp));
//            label_ch4_amp->setText("Ch4(amp) = "+QString::number(U4_amp));
//            label_ch5_amp->setText("Ch5(amp) = "+QString::number(U5_amp));
//            label_ch6_amp->setText("Ch6(amp) = "+QString::number(U6_amp));
//            label_ch7_amp->setText("Ch7(amp) = "+QString::number(U7_amp));
//            label_ch8_amp->setText("Ch8(amp) = "+QString::number(U8_amp));*/

//            label_ch1_pk->setText("Ch1(pk) = "+QString::number(U1_pk));
//            label_ch2_pk->setText("Ch2(pk) = "+QString::number(U2_pk));
//            label_ch3_pk->setText("Ch3(pk) = "+QString::number(U3_pk));
//            label_ch4_pk->setText("Ch4(pk) = "+QString::number(U4_pk));
//            label_ch5_pk->setText("Ch5(pk) = "+QString::number(U5_pk));
//            label_ch6_pk->setText("Ch6(pk) = "+QString::number(U6_pk));
//            label_ch7_pk->setText("Ch7(pk) = "+QString::number(U7_pk));
//            label_ch8_pk->setText("Ch8(pk) = "+QString::number(U8_pk));

//            rms_count=0;
//            temp_U1rms=0;
//            temp_U2rms=0;
//            temp_U3rms=0;
//            temp_U4rms=0;
//            temp_U5rms=0;
//            temp_U6rms=0;
//            temp_U7rms=0;
//            temp_U8rms=0;
//        }

//    }



    //int N=2048;
    //fftwf_complex in[N], out[N];
    //fftwf_plan p1, q;
//    for (int i = 0; i < N; i++) {
//        in[i][0] = cos(3 * 2*M_PI*i/N);
//        in[i][1] = 0;
//    }


    i=0;
    j=0;
    while(i < ( 2048))
    {
        /*********************************************************************/
        //Module 1
        /*memcpy(&chanels[0],  &adc_m1[j],    2);
        memcpy(&chanels[1],  &adc_m1[j+2],  2);
        memcpy(&chanels[2],  &adc_m1[j+4],  2);
        memcpy(&chanels[3],  &adc_m1[j+6],  2);
        memcpy(&chanels[4],  &adc_m1[j+8],  2);
        memcpy(&chanels[5],  &adc_m1[j+10], 2);
        memcpy(&chanels[6],  &adc_m1[j+12], 2);
        memcpy(&chanels[7],  &adc_m1[j+14], 2);
        memcpy(&chanels[8],  &adc_m1[j+16], 2);
        memcpy(&chanels[9],  &adc_m1[j+18], 2);
        memcpy(&chanels[10], &adc_m1[j+20], 2);
        memcpy(&chanels[11], &adc_m1[j+22], 2);
        memcpy(&chanels[12], &adc_m1[j+24], 2);
        memcpy(&chanels[13], &adc_m1[j+26], 2);
        memcpy(&chanels[14], &adc_m1[j+28], 2);
        memcpy(&chanels[15], &adc_m1[j+30], 2);
        memcpy(&chanels[16], &adc_m1[j+32], 2);
        memcpy(&chanels[17], &adc_m1[j+34], 2);
        memcpy(&chanels[18], &adc_m1[j+36], 2);
        memcpy(&chanels[19], &adc_m1[j+38], 2);
        memcpy(&chanels[20], &adc_m1[j+40], 2);
        memcpy(&chanels[21], &adc_m1[j+42], 2);
        memcpy(&chanels[22], &adc_m1[j+44], 2);
        memcpy(&chanels[23], &adc_m1[j+46], 2);
        memcpy(&chanels[24], &adc_m1[j+48], 2);
        memcpy(&chanels[25], &adc_m1[j+50], 2);
        memcpy(&chanels[26], &adc_m1[j+52], 2);
        memcpy(&chanels[27], &adc_m1[j+54], 2);
        memcpy(&chanels[28], &adc_m1[j+56], 2);
        memcpy(&chanels[29], &adc_m1[j+58], 2);
        memcpy(&chanels[30], &adc_m1[j+60], 2);
        memcpy(&chanels[31], &adc_m1[j+62], 2);
        memcpy(&chanels[32], &adc_m1[j+64], 2);
        memcpy(&chanels[33], &adc_m1[j+66], 2);
        memcpy(&chanels[34], &adc_m1[j+68], 2);
        memcpy(&chanels[35], &adc_m1[j+70], 2);
        memcpy(&chanels[36], &adc_m1[j+72], 2);
        memcpy(&chanels[37], &adc_m1[j+74], 2);
        memcpy(&chanels[38], &adc_m1[j+76], 2);
        memcpy(&chanels[39], &adc_m1[j+78], 2);
        memcpy(&chanels[40], &adc_m1[j+80], 2);
        memcpy(&chanels[41], &adc_m1[j+82], 2);
        memcpy(&chanels[42], &adc_m1[j+84], 2);
        memcpy(&chanels[43], &adc_m1[j+86], 2);
        memcpy(&chanels[44], &adc_m1[j+88], 2);
        memcpy(&chanels[45], &adc_m1[j+90], 2);
        memcpy(&chanels[46], &adc_m1[j+92], 2);
        memcpy(&chanels[47], &adc_m1[j+94], 2);
        memcpy(&chanels[48], &adc_m1[j+96], 2);
        memcpy(&chanels[49], &adc_m1[j+98], 2);
        memcpy(&chanels[50], &adc_m1[j+100],2);
        memcpy(&chanels[51], &adc_m1[j+102],2);
        memcpy(&chanels[52], &adc_m1[j+104],2);
        memcpy(&chanels[53], &adc_m1[j+106],2);
        memcpy(&chanels[54], &adc_m1[j+108],2);
        memcpy(&chanels[55], &adc_m1[j+110],2);
        memcpy(&chanels[56], &adc_m1[j+112],2);
        memcpy(&chanels[57], &adc_m1[j+114],2);
        memcpy(&chanels[58], &adc_m1[j+116],2);
        memcpy(&chanels[59], &adc_m1[j+118],2);
        memcpy(&chanels[60], &adc_m1[j+120],2);
        memcpy(&chanels[61], &adc_m1[j+122],2);
        memcpy(&chanels[62], &adc_m1[j+124],2);
        memcpy(&chanels[63], &adc_m1[j+126],2);
        memcpy(&chanels[64], &adc_m1[j+128],2);
        memcpy(&chanels[65], &adc_m1[j+130],2);
        memcpy(&chanels[66], &adc_m1[j+132],2);
        memcpy(&chanels[67], &adc_m1[j+134],2);
        memcpy(&chanels[68], &adc_m1[j+136],2);
        memcpy(&chanels[69], &adc_m1[j+138],2);
        memcpy(&chanels[70], &adc_m1[j+140],2);
        memcpy(&chanels[71], &adc_m1[j+142],2);
        memcpy(&chanels[72], &adc_m1[j+144],2);
        memcpy(&chanels[73], &adc_m1[j+146],2);
        memcpy(&chanels[74], &adc_m1[j+148],2);
        memcpy(&chanels[75], &adc_m1[j+150],2);
        memcpy(&chanels[76], &adc_m1[j+152],2);
        memcpy(&chanels[77], &adc_m1[j+154],2);
        memcpy(&chanels[78], &adc_m1[j+156],2);
        memcpy(&chanels[79], &adc_m1[j+158],2);
        memcpy(&chanels[80], &adc_m1[j+160],2);
        memcpy(&chanels[81], &adc_m1[j+162],2);
        memcpy(&chanels[82], &adc_m1[j+164],2);
        memcpy(&chanels[83], &adc_m1[j+166],2);
        memcpy(&chanels[84], &adc_m1[j+168],2);
        //memcpy(&chanels[85], &adc_m1[j+170],2);*/


        for(int t=0, m=0; m<170; t+=2, m++){
            memcpy(&chanels[m], &adc_m1[j+t],2);
        }

        adc_t+=1;
        x = adc_t;

        //128 channels
        for(int k=0; k<85; k++){
            scope_y[k] =chanels[k]*0.000152;
        }

        //hann window
        double multiplier = 0.5 * (1.0 - std::cos(2.0 * pi * i / (2048 - 1)));

        //hamming window
        //double multiplier = (0.54 - 0.46 * std::cos(2.0 * pi * i / (2048 - 1)));

        //blackman window
        //double multiplier = 0.42 - 0.5 * std::cos(2.0 * pi * i / (2048 - 1)) + 0.08 * std::cos(4.0 * pi * i / (2048 - 1));

        //fft_ch1[i].imag(chanels[0]*0.000152 *multiplier);
        //fft_ch1[i].real(chanels[1]*0.000152 *multiplier);


        //in[i][0] = (chanels[0]*0.000152 *multiplier);
        //in[i][1] = (chanels[1]*0.000152 *multiplier);


        //in[i][0] = (chanels[0]*0.000152);
        //in[i][1] = (chanels[1]*0.000152);

        //in[i][0] = (chanels[0]*0.000152);
        //in[i][1] = (0);


        re[i]=(chanels[0]*0.000152)*multiplier;
        im[i]=(chanels[1]*0.000152)*multiplier;

//        re[i]=(chanels[0]*0.000152);
//        im[i]=(chanels[1]*0.000152);


//        fft_ch1[i].imag(0);
//        fft_ch1[i].real(chanels[0]*0.000152);

//        fft_ch2[i].imag(0);
//        fft_ch2[i].real(chanels[1]*0.000152);

        /*fft_ch1[i]  = (float)chanels[0]*0.000152 *multiplier;
        fft_ch2[i]  = (float)chanels[1]*0.000152 *multiplier;
        fft_ch3[i]  = (float)chanels[2]*0.000152 *multiplier;
        fft_ch4[i]  = (float)chanels[3]*0.000152 *multiplier;
        fft_ch5[i]  = (float)chanels[4]*0.000152 *multiplier;
        fft_ch6[i]  = (float)chanels[5]*0.000152 *multiplier;
        fft_ch7[i]  = (float)chanels[6]*0.000152 *multiplier;
        fft_ch8[i]  = (float)chanels[7]*0.000152 *multiplier;

        fft_ch9[i]  = (float)chanels[8]*0.000152  *multiplier;
        fft_ch10[i] = (float)chanels[9]*0.000152 *multiplier;
        fft_ch11[i] = (float)chanels[10]*0.000152 *multiplier;
        fft_ch12[i] = (float)chanels[11]*0.000152 *multiplier;
        fft_ch13[i] = (float)chanels[12]*0.000152 *multiplier;
        fft_ch14[i] = (float)chanels[13]*0.000152 *multiplier;
        fft_ch15[i] = (float)chanels[14]*0.000152 *multiplier;
        fft_ch16[i] = (float)chanels[15]*0.000152 *multiplier;


        fft_ch14_t[i] = (float)chanels[13]*0.000152 *multiplier;*/

//        if(scope_graphs[0] != nullptr) scope_graphs[0]->graph(0)->addData( x, scope_y[1]);
//        if(scope_graphs[1] != nullptr) scope_graphs[1]->graph(0)->addData( x, scope_y[2]);

        for(int k=0;k<85;k++){
            if(scope_graphs[k] != nullptr) scope_graphs[k]->graph(0)->addData( x, scope_y[k]);
        }

        i++;
        //j+=64;
        j+=170;
    }
    i=0;
    j=0;
    lock_data=0;



//    float mean_ch1 = 0;
//    //float fft_ch1_dc[2048];
//    for (uint16_t i = 0; i < 2048; i++)
//    {
//        //mean_ch1 += fft_ch15[i].real();
//        mean_ch1 += fft_ch14_t[i];
//    }
//    mean_ch1 /= 2048;
//    // Subtract the mean from vData
//    for (uint16_t i = 0; i < 2048; i++)
//    {
//        //fft_ch1[i].real() -= mean_ch1;
//        //fft_ch1_dc[i-1] = fft_ch15[i].real() - mean_ch1;
//        fft_ch14[i] = (float)fft_ch14_t[i] - mean_ch1;
//    }


    /*********************************************************************************************************/
    // создаем план для библиотеки fftw
    double y;
    double yl;




    //p1 = fftwf_plan_dft_1d(N, in, (fftwf_complex*) &fft_ch1[0], FFTW_FORWARD, FFTW_ESTIMATE);
    //fftwf_execute(p1);
    //fftwf_destroy_plan(p1);


    fft(re, im, out, true);


    /*fftwf_plan  plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &fft_ch1[0], (fftwf_complex*) &fft_ch1[0], FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);

    plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &fft_ch2[0], (fftwf_complex*) &fft_ch2[0], FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);*/

    /*plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &fft_ch3[0], (fftwf_complex*) &fft_ch3[0], FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);

    plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &fft_ch4[0], (fftwf_complex*) &fft_ch4[0], FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);

    plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &fft_ch5[0], (fftwf_complex*) &fft_ch5[0], FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);

    plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &fft_ch6[0], (fftwf_complex*) &fft_ch6[0], FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);

    plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &fft_ch7[0], (fftwf_complex*) &fft_ch7[0], FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);

    plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &fft_ch8[0], (fftwf_complex*) &fft_ch8[0], FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);

    plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &fft_ch9[0], (fftwf_complex*) &fft_ch9[0], FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);

    plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &fft_ch10[0], (fftwf_complex*) &fft_ch10[0], FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);

    plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &fft_ch11[0], (fftwf_complex*) &fft_ch11[0], FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);

    plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &fft_ch12[0], (fftwf_complex*) &fft_ch12[0], FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);

    plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &fft_ch13[0], (fftwf_complex*) &fft_ch13[0], FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);

    plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &fft_ch14[0], (fftwf_complex*) &fft_ch14[0], FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);

    plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &fft_ch15[0], (fftwf_complex*) &fft_ch15[0], FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);

    plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &fft_ch16[0], (fftwf_complex*) &fft_ch16[0], FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);*/






    //***************************************************************************************************/



    i=0;
    x = 0;
    adc_t=-10000.0;

    //Harmonics
    //Clear 16 fft
    for(int i=0;i<86;i++)
    {
       if(fft_graphs[i]   != nullptr) fft_graphs[i]->graph(0)->data()->clear();
    }

    j=0;
    i=0;
    int k=0;
    int m=0;
    while(i < 2048)
    {

        x = adc_t;
        //adc_t+=12.20703125;
        //adc_t+=97.65625;
        adc_t+=9.765625;

        //double Q = sqrt(pow(fft_ch1[i].real(),2.0) + pow(fft_ch1[i].imag(),2.0));
        //double I = sqrt(pow(fft_ch2[i].real(),2.0) + pow(fft_ch2[i].imag(),2.0));

        //double Q = sqrt(pow((double)out[i],2.0) + pow((double)out[2047-i],2.0));
        //double I = sqrt(pow((double)out[i],2.0) + pow((double)out[i],2.0));



        double Q;
        double I;


        if(i<1024){
            Q = sqrt(pow(out[2048+k],2.0) + pow(out[2048+k+1],2.0));
            fft_y[1]  = (double) (20* log10(abs(Q)/64));
            k+=2;
        }
        else{
            //I = sqrt(pow(out[2047-k],2.0) + pow(out[2047-(k+1)],2.0));
            I = sqrt(pow(out[m],2.0) + pow(out[m+1],2.0));
            fft_y[1]  = (double) (20* log10(abs(I)/64));
            m+=2;
        }

        /*if(i<1024)
            fft_y[1]  = (double) (20* log10(abs(Q)/1024));
        else
            fft_y[1]  = (double) (20* log10(abs(Q)/1024));*/



        //fft_y[1]  = Q;

       //if(i<1024)
            //fft_y[1]  = (double) (20* log10(abs(fft_ch1[i].real())/2048));
        //else
            //fft_y[1]  = (double) (20* log10(abs(fft_ch1[i].imag())/2048));

        //fft_y[1]  = (double) (20* log10(abs(fft_ch1[i])/2048));




//        if(i<1024)
//            fft_y[1]  = (double) (20* log10(abs(out[1023-i][0])/2048));
//        else{
//            fft_y[1]  = (double) (20* log10(abs(out[2047-j][1])/2048));
//            j++;
//        }




//        if(i<1024)
//            fft_y[1]  = (double) (20* log10(abs(out[1023-i][0])/2048));
//        else{
//            fft_y[1]  = (double) (20* log10(abs(out[2047-j][1])/2048));
//            j++;
//        }









            //out[i][0] << " + j" << out[i][1]

        //fft_y[2]  = (double) (20* log10(abs(fft_ch2[i])/2048));
        /*fft_y[3]  = (double) (20* log10(abs(fft_ch3[i])/2048));
        fft_y[4]  = (double) (20* log10(abs(fft_ch4[i])/2048));
        fft_y[5]  = (double) (20* log10(abs(fft_ch5[i])/2048));
        fft_y[6]  = (double) (20* log10(abs(fft_ch6[i])/2048));
        fft_y[7]  = (double) (20* log10(abs(fft_ch7[i])/2048));
        fft_y[8]  = (double) (20* log10(abs(fft_ch8[i])/2048));
        fft_y[9]  = (double) (20* log10(abs(fft_ch9[i])/2048));
        fft_y[10]  = (double) (20* log10(abs(fft_ch10[i])/2048));
        fft_y[11] = (double) (20* log10(abs(fft_ch11[i])/2048));
        fft_y[12] = (double) (20* log10(abs(fft_ch12[i])/2048));
        fft_y[13] = (double) (20* log10(abs(fft_ch13[i])/2048));
        fft_y[14] = (double) (20* log10(abs(fft_ch14[i])/2048));
        fft_y[15] = (double) (20* log10(abs(fft_ch15[i])/2048));
        fft_y[16] = (double) (20* log10(abs(fft_ch16[i])/2048));*/

        //for(int k=0;k<85;k++){
        //for(int k=0;k<3;k++){
            if(fft_graphs[0] != nullptr) fft_graphs[0]->graph(0)->addData( x, fft_y[1]);
        //}

        i++;
    }
    i=0;
    j=0;
    adc_t=0;


    for(int i=0;i<86;i++)
    {
       if(scope_graphs[i] != nullptr) scope_graphs[i]->replot();
       if(fft_graphs[i]   != nullptr) fft_graphs[i]->replot();
    }

}

//Read datagram from UDP
void MainWindow::readPendingDatagrams()
{
    QHostAddress sender;
    quint16 senderPort;
    //int diff_pkt1;
    //int diff_pkt2;
    //qDebug() <<  " MainWindow::readPendingDatagrams";

    while (udpSocket->pendingDatagramSize()!=-1)
    {
        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        //signal from timer
        if(!lock_data)   //enable
        {
            //Search host in table
            if(table!=nullptr){
                for(int i=0; i<table->rowCount(); i++)
                {
                    if(sender == hosts[i])
                    {
                        //qDebug() << "sender: " << sender.toString(); //QString::number(val);
                        //qDebug() << "host:   " << hosts[i].toString();

                        //Module 1
                        if(i==0){
                            memcpy(&adc_t1[j1],&datagram.data()[30], 1360);
                            j1+=1360;
                            if(j1==348160){//256 pakets
                                j1=0;
                                //memset(adc_m1,0x00,348160);
                                memcpy(adc_m1,adc_t1,348160);
                            }
                        }

                        //Module 2
//                        if(i==1){
//                            memcpy(&adc_t2[j2],&datagram.data()[4], (int)datagram.size());
//                            j2+=1408;
//                            if(j2==132352){//94 pakets
//                                j2=0;
//                                memset(adc_m2,0x00,132352);
//                                memcpy(adc_m2,adc_t2,132352);
//                            }
//                        }

//                        //Module 3
//                        if(i==2){
//                            memcpy(&adc_t3[j3],&datagram.data()[4], (int)datagram.size());
//                            j3+=1408;
//                            if(j3==132352){//94 pakets
//                                j3=0;
//                                memset(adc_m3,0x00,132352);
//                                memcpy(adc_m3,adc_t3,132352);
//                            }
//                        }

//                        //Module 4
//                        if(i==3){
//                            memcpy(&adc_t4[j4],&datagram.data()[4], (int)datagram.size());
//                            j4+=1408;
//                            if(j4==132352){//94 pakets
//                                j4=0;
//                                memset(adc_m4,0x00,132352);
//                                memcpy(adc_m4,adc_t4,132352);
//                            }
//                        }

                    }
                }
             }
        }
        else{
            j1=0;
            j2=0;
            j3=0;
            j4=0;
        }

    }
}
































/*scope_y[1] =ch1_t*0.000152;
scope_y[2] =ch2_t*0.000152;
scope_y[3] =ch3_t*0.000152;
scope_y[4] =ch4_t*0.000152;
scope_y[5] =ch5_t*0.000152;
scope_y[6] =ch6_t*0.000152;
scope_y[7] =ch7_t*0.000152;
scope_y[8] =ch8_t*0.000152;
scope_y[9] =ch9_t*0.000152;
scope_y[10]=ch10_t*0.000152;
scope_y[11]=ch11_t*0.000152;
scope_y[12]=ch12_t*0.000152;
scope_y[13]=ch13_t*0.000152;
scope_y[14]=ch14_t*0.000152;
scope_y[15]=ch15_t*0.000152;
scope_y[16]=ch16_t*0.000152;
scope_y[17]=ch17_t*0.000152;
scope_y[18]=ch18_t*0.000152;
scope_y[19]=ch19_t*0.000152;
scope_y[20]=ch20_t*0.000152;
scope_y[21]=ch21_t*0.000152;
scope_y[22]=ch22_t*0.000152;
scope_y[23]=ch23_t*0.000152;
scope_y[24]=ch24_t*0.000152;
scope_y[25]=ch25_t*0.000152;
scope_y[26]=ch26_t*0.000152;
scope_y[27]=ch27_t*0.000152;
scope_y[28]=ch28_t*0.000152;
scope_y[29]=ch29_t*0.000152;
scope_y[30]=ch30_t*0.000152;
scope_y[31]=ch31_t*0.000152;
scope_y[32]=ch32_t*0.000152;

scope_y[33]=ch33_t*0.000152;
scope_y[34]=ch34_t*0.000152;
scope_y[35]=ch35_t*0.000152;
scope_y[36]=ch36_t*0.000152;
scope_y[37]=ch37_t*0.000152;
scope_y[38]=ch38_t*0.000152;
scope_y[39]=ch39_t*0.000152;
scope_y[40]=ch40_t*0.000152;
scope_y[41]=ch41_t*0.000152;
scope_y[42]=ch42_t*0.000152;
scope_y[43]=ch43_t*0.000152;
scope_y[44]=ch44_t*0.000152;
scope_y[45]=ch45_t*0.000152;
scope_y[46]=ch46_t*0.000152;
scope_y[47]=ch47_t*0.000152;
scope_y[48]=ch48_t*0.000152;
scope_y[49]=ch49_t*0.000152;
scope_y[50]=ch50_t*0.000152;
scope_y[51]=ch51_t*0.000152;
scope_y[52]=ch52_t*0.000152;
scope_y[53]=ch53_t*0.000152;
scope_y[54]=ch54_t*0.000152;
scope_y[55]=ch55_t*0.000152;
scope_y[56]=ch56_t*0.000152;
scope_y[57]=ch57_t*0.000152;
scope_y[58]=ch58_t*0.000152;
scope_y[59]=ch59_t*0.000152;
scope_y[60]=ch60_t*0.000152;
scope_y[61]=ch61_t*0.000152;
scope_y[62]=ch62_t*0.000152;
scope_y[63]=ch63_t*0.000152;
scope_y[64]=ch64_t*0.000152;*/


/*scope1 = window->findChild<QCustomPlot  *>("ch1");
scope2 = window->findChild<QCustomPlot  *>("ch2");
scope3 = window->findChild<QCustomPlot  *>("ch3");
scope4 = window->findChild<QCustomPlot  *>("ch4");
scope5 = window->findChild<QCustomPlot  *>("ch5");
scope6 = window->findChild<QCustomPlot  *>("ch6");
scope7 = window->findChild<QCustomPlot  *>("ch7");
scope8 = window->findChild<QCustomPlot  *>("ch8");
scope9 = window->findChild<QCustomPlot  *>("ch9");
scope10 = window->findChild<QCustomPlot *>("ch10");
scope11 = window->findChild<QCustomPlot *>("ch11");
scope12 = window->findChild<QCustomPlot *>("ch12");
scope13 = window->findChild<QCustomPlot *>("ch13");
scope14 = window->findChild<QCustomPlot *>("ch14");
scope15 = window->findChild<QCustomPlot *>("ch15");
scope16 = window->findChild<QCustomPlot *>("ch16");
scope17 = window->findChild<QCustomPlot *>("ch17");
scope18 = window->findChild<QCustomPlot *>("ch18");
scope19 = window->findChild<QCustomPlot *>("ch19");
scope20 = window->findChild<QCustomPlot *>("ch20");
scope21 = window->findChild<QCustomPlot *>("ch21");
scope22 = window->findChild<QCustomPlot *>("ch22");
scope23 = window->findChild<QCustomPlot *>("ch23");
scope24 = window->findChild<QCustomPlot *>("ch24");
scope25 = window->findChild<QCustomPlot *>("ch25");
scope26 = window->findChild<QCustomPlot *>("ch26");
scope27 = window->findChild<QCustomPlot *>("ch27");
scope28 = window->findChild<QCustomPlot *>("ch28");
scope29 = window->findChild<QCustomPlot *>("ch29");
scope30 = window->findChild<QCustomPlot *>("ch30");
scope31 = window->findChild<QCustomPlot *>("ch31");
scope32 = window->findChild<QCustomPlot *>("ch32");

scope33 = window->findChild<QCustomPlot *>("ch33");
scope34 = window->findChild<QCustomPlot *>("ch34");
scope35 = window->findChild<QCustomPlot *>("ch35");
scope36 = window->findChild<QCustomPlot *>("ch36");
scope37 = window->findChild<QCustomPlot *>("ch37");
scope38 = window->findChild<QCustomPlot *>("ch38");
scope39 = window->findChild<QCustomPlot *>("ch39");
scope40 = window->findChild<QCustomPlot *>("ch40");
scope41 = window->findChild<QCustomPlot *>("ch41");
scope42 = window->findChild<QCustomPlot *>("ch42");
scope43 = window->findChild<QCustomPlot *>("ch43");
scope44 = window->findChild<QCustomPlot *>("ch44");
scope45 = window->findChild<QCustomPlot *>("ch45");
scope46 = window->findChild<QCustomPlot *>("ch46");
scope47 = window->findChild<QCustomPlot *>("ch47");
scope48 = window->findChild<QCustomPlot *>("ch48");
scope49 = window->findChild<QCustomPlot *>("ch49");
scope50 = window->findChild<QCustomPlot *>("ch50");
scope51 = window->findChild<QCustomPlot *>("ch51");
scope52 = window->findChild<QCustomPlot *>("ch52");
scope53 = window->findChild<QCustomPlot *>("ch53");
scope54 = window->findChild<QCustomPlot *>("ch54");
scope55 = window->findChild<QCustomPlot *>("ch55");
scope56 = window->findChild<QCustomPlot *>("ch56");
scope57 = window->findChild<QCustomPlot *>("ch57");
scope58 = window->findChild<QCustomPlot *>("ch58");
scope59 = window->findChild<QCustomPlot *>("ch59");
scope60 = window->findChild<QCustomPlot *>("ch60");
scope61 = window->findChild<QCustomPlot *>("ch61");
scope62 = window->findChild<QCustomPlot *>("ch62");
scope63 = window->findChild<QCustomPlot *>("ch63");
scope64 = window->findChild<QCustomPlot *>("ch64");*/


//**************************************************************************************//
//FFT
/*fft1 = window->findChild<QCustomPlot *>("fft1");
fft2 = window->findChild<QCustomPlot *>("fft2");
fft3 = window->findChild<QCustomPlot *>("fft3");
fft4 = window->findChild<QCustomPlot *>("fft4");
fft5 = window->findChild<QCustomPlot *>("fft5");
fft6 = window->findChild<QCustomPlot *>("fft6");
fft7 = window->findChild<QCustomPlot *>("fft7");
fft8 = window->findChild<QCustomPlot *>("fft8");
fft9 = window->findChild<QCustomPlot *>("fft9");
fft10 = window->findChild<QCustomPlot *>("fft10");
fft11 = window->findChild<QCustomPlot *>("fft11");
fft12 = window->findChild<QCustomPlot *>("fft12");
fft13 = window->findChild<QCustomPlot *>("fft13");
fft14 = window->findChild<QCustomPlot *>("fft14");
fft15 = window->findChild<QCustomPlot *>("fft15");
fft16 = window->findChild<QCustomPlot *>("fft16");*/

//console1 = window->findChild<QTextEdit *>("console1");
//console3 = window->findChild<QTextEdit *>("console3");

//console1 = window->findChild<QPlainTextEdit *>("console");
//console2 = window->findChild<QTableWidget *>("console2");
//prev_cursor = console1->textCursor();
//QPlainTextEdit


//Clear 16 chanel's
/*if(scope1  != nullptr) scope1->graph(0)->data()->clear();
if(scope2  != nullptr) scope2->graph(0)->data()->clear();
if(scope3  != nullptr) scope3->graph(0)->data()->clear();
if(scope4  != nullptr) scope4->graph(0)->data()->clear();
if(scope5  != nullptr) scope5->graph(0)->data()->clear();
if(scope6  != nullptr) scope6->graph(0)->data()->clear();
if(scope7  != nullptr) scope7->graph(0)->data()->clear();
if(scope8  != nullptr) scope8->graph(0)->data()->clear();
if(scope9  != nullptr) scope9->graph(0)->data()->clear();
if(scope10 != nullptr) scope10->graph(0)->data()->clear();
if(scope11 != nullptr) scope11->graph(0)->data()->clear();
if(scope12 != nullptr) scope12->graph(0)->data()->clear();
if(scope13 != nullptr) scope13->graph(0)->data()->clear();
if(scope14 != nullptr) scope14->graph(0)->data()->clear();
if(scope15 != nullptr) scope15->graph(0)->data()->clear();
if(scope16 != nullptr) scope16->graph(0)->data()->clear();*/




/*if(scope1  != nullptr) scope1->graph(0)->addData( x, scope_y[0]);
if(scope2  != nullptr) scope2->graph(0)->addData( x, scope_y[1]);
if(scope3  != nullptr) scope3->graph(0)->addData( x, scope_y[2]);
if(scope4  != nullptr) scope4->graph(0)->addData( x, scope_y[3]);
if(scope5  != nullptr) scope5->graph(0)->addData( x, scope_y[4]);
if(scope6  != nullptr) scope6->graph(0)->addData( x, scope_y[5]);
if(scope7  != nullptr) scope7->graph(0)->addData( x, scope_y[6]);
if(scope8  != nullptr) scope8->graph(0)->addData( x, scope_y[7]);
if(scope9  != nullptr) scope9->graph(0)->addData( x, scope_y[8]);
if(scope10 != nullptr) scope10->graph(0)->addData(x, scope_y[9]);
if(scope11 != nullptr) scope11->graph(0)->addData(x, scope_y[10]);
if(scope12 != nullptr) scope12->graph(0)->addData(x, scope_y[11]);
if(scope13 != nullptr) scope13->graph(0)->addData(x, scope_y[12]);
if(scope14 != nullptr) scope14->graph(0)->addData(x, scope_y[13]);
if(scope15 != nullptr) scope15->graph(0)->addData(x, scope_y[14]);
if(scope16 != nullptr) scope16->graph(0)->addData(x, scope_y[15]);*/



//***************************************************************************************************/
//    double fk_Ua, gen_Ua[2048];
//    double pi = 3.14159265359;
//    QVector<double> h(2048);
//    QVector<double> temp_Ua(2048);
//    fk_Ua = 2*pi*(100.0/25000.0);

//    vector<complex<float> > fft_in(2048, 0.);
//    vector<complex<float> > fft_out(2048, 0.);

//    for(int q=0; q<2048; q++)
//    {
//        //h[q] = q;
//        //gen_Ua[q] = 5.0*sin(q*fk_Ua);

//        fft_in[q].real(5.0*sin(q*fk_Ua));

//    }

    // создаем план для библиотеки fftw
//    plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &fft_ch3[0], (fftwf_complex*) &fft_out[0], FFTW_FORWARD, FFTW_ESTIMATE);
//    fftwf_execute(plan);
//    fftwf_destroy_plan(plan);

/*for(int q=0; q<2048; q++)
{
    temp_Ua[q] = abs(gen_Ua[q]);
}*/

/*if(fft1  != nullptr) fft1->graph(0)->data()->clear();
if(fft2  != nullptr) fft2->graph(0)->data()->clear();
if(fft3  != nullptr) fft3->graph(0)->data()->clear();
if(fft4  != nullptr) fft4->graph(0)->data()->clear();
if(fft5  != nullptr) fft5->graph(0)->data()->clear();
if(fft6  != nullptr) fft6->graph(0)->data()->clear();
if(fft7  != nullptr) fft7->graph(0)->data()->clear();
if(fft8  != nullptr) fft8->graph(0)->data()->clear();
if(fft9  != nullptr) fft9->graph(0)->data()->clear();
if(fft10 != nullptr) fft10->graph(0)->data()->clear();
if(fft11 != nullptr) fft11->graph(0)->data()->clear();
if(fft12 != nullptr) fft12->graph(0)->data()->clear();
if(fft13 != nullptr) fft13->graph(0)->data()->clear();
if(fft14 != nullptr) fft14->graph(0)->data()->clear();
if(fft15 != nullptr) fft15->graph(0)->data()->clear();
if(fft16 != nullptr) fft16->graph(0)->data()->clear();*/

//scope_graphs[i] = window->findChild<QCustomPlot  *>("ch"+QString::number(i));
//fft_graphs[i]   = window->findChild<QCustomPlot *>("fft"+QString::number(i));
//if(scope_graphs[i] != nullptr) scope_graphs[i]->graph(0)->data()->clear();

/*float mean_ch1 = 0;
float fft_ch1_dc[1024];
for (uint16_t i = 1; i < (1024+1); i++)
{
    mean_ch1 += fft_ch15[i].real();
}
mean_ch1 /= 1024;
// Subtract the mean from vData
for (uint16_t i = 1; i < (1024 + 1); i++)
{
    //fft_ch1[i].real() -= mean_ch1;
    fft_ch1_dc[i-1] = fft_ch15[i].real() - mean_ch1;
}*/



//if(i==0)//DC
//{
    /*fft_y[0]  = (double) (20* log10(abs(0)/2048));
    fft_y[1]  = (double) (20* log10(abs(0)/2048));
    fft_y[2]  = (double) (20* log10(abs(0)/2048));
    fft_y[3]  = (double) (20* log10(abs(0)/2048));
    fft_y[4]  = (double) (20* log10(abs(0)/2048));
    fft_y[5]  = (double) (20* log10(abs(0)/2048));
    fft_y[6]  = (double) (20* log10(abs(0)/2048));
    fft_y[7]  = (double) (20* log10(abs(0)/2048));
    fft_y[8]  = (double) (20* log10(abs(0)/2048));
    fft_y[9]  = (double) (20* log10(abs(0)/2048));
    fft_y[10] = (double) (20* log10(abs(0)/2048));
    fft_y[11] = (double) (20* log10(abs(0)/2048));
    fft_y[12] = (double) (20* log10(abs(0)/2048));
    fft_y[13] = (double) (20* log10(abs(0)/2048));
    fft_y[14] = (double) (20* log10(abs(0)/2048));
    fft_y[15] = (double) (20* log10(abs(0)/2048));*/

//}
/*else{
    fft_y[0] = (double) (20* log10(abs(fft_ch1[i])/16384));
    fft_y[1] = (double) (20* log10(abs(fft_ch2[i])/16384));
    fft_y[2] = (double) (20* log10(abs(fft_ch3[i])/16384));
    fft_y[3] = (double) (20* log10(abs(fft_ch4[i])/16384));
    fft_y[4] = (double) (20* log10(abs(fft_ch5[i])/16384));
    fft_y[5] = (double) (20* log10(abs(fft_ch6[i])/16384));
    fft_y[6] = (double) (20* log10(abs(fft_ch7[i])/16384));
    fft_y[7] = (double) (20* log10(abs(fft_ch8[i])/16384));
    fft_y[8] = (double) (20* log10(abs(fft_ch9[i])/16384));
    fft_y[9] = (double) (20* log10(abs(fft_ch10[i])/16384));
    fft_y[10] = (double) (20* log10(abs(fft_ch11[i])/16384));
    fft_y[11] = (double) (20* log10(abs(fft_ch12[i])/16384));
    fft_y[12] = (double) (20* log10(abs(fft_ch13[i])/16384));
    fft_y[13] = (double) (20* log10(abs(fft_ch14[i])/16384));
    fft_y[14] = (double) (20* log10(abs(fft_ch15[i])/16384));
    fft_y[15] = (double) (20* log10(abs(fft_ch16[i])/16384));
}*/



//        f1 = fft_ch1[i].real()/2048;
//        f2 = fft_ch2[i].real()/2048;
//        f3 = fft_ch3[i].real()/2048;

//fft_y[0] = (double) (20* log10(2*abs(f1)));
//fft_y[1] = (double) (20* log10(2*abs(f2)));
//fft_y[2] = (double) (20* log10(2*abs(f3)));




/*if(fft1  != nullptr) fft1->graph(0)->addData( x, fft_y[0]);
if(fft2  != nullptr) fft2->graph(0)->addData( x, fft_y[1]);
//if(fft3  != nullptr) fft3->graph(0)->addData( x, fft_y[2]);
if(fft4  != nullptr) fft4->graph(0)->addData( x, fft_y[3]);
if(fft5  != nullptr) fft5->graph(0)->addData( x, fft_y[4]);
if(fft6  != nullptr) fft6->graph(0)->addData( x, fft_y[5]);
if(fft7  != nullptr) fft7->graph(0)->addData( x, fft_y[6]);
if(fft8  != nullptr) fft8->graph(0)->addData( x, fft_y[7]);
if(fft9  != nullptr) fft9->graph(0)->addData( x, fft_y[8]);
if(fft10 != nullptr) fft10->graph(0)->addData(x, fft_y[9]);
if(fft11 != nullptr) fft11->graph(0)->addData(x, fft_y[10]);
if(fft12 != nullptr) fft12->graph(0)->addData(x, fft_y[11]);
if(fft13 != nullptr) fft13->graph(0)->addData(x, fft_y[12]);
if(fft14 != nullptr) fft14->graph(0)->addData(x, fft_y[13]);
if(fft15 != nullptr) fft15->graph(0)->addData(x, fft_y[14]);
if(fft16 != nullptr) fft16->graph(0)->addData(x, fft_y[15]);*/


//if(fft3_graph1 != nullptr) fft3_graph1->addData(x,fft_y[2]);
//if(fft3_graph2 != nullptr) fft3_graph2->addData(x,-160.0);



//if(fft3  != nullptr) fft3->graph(0)->addData( x, fft_y[2]);
//if(fft3  != nullptr) fft3->graph(1)->addData( x, -200.0);


//fft_y[2] = (double) (20* log10(abs(fft_ch3[i])/16384));
//dataFFT3[i].key   = x;
//dataFFT3[i].value = fft_y[2];




//mean_fft3[average_state][i] = fft_y[2];



//    average_state+=1;
//    if(average_state==5){
//        for(int i=0; i<1024; i++) {
//            x = adc_t;
//            adc_t+=12.20703125;
//            dataFFT3[i].key   = x;
//            dataFFT3[i].value = (mean_fft3[0][i] + mean_fft3[1][i] + mean_fft3[2][i] + mean_fft3[3][i] + mean_fft3[4][i])/5;
//        }
//        if(fft3  != nullptr) fft3->graph(0)->data()->clear();
//        if(fft3  != nullptr) {fft3->graph(0)->data()->set(dataFFT3); /*fft3->yAxis->rescale();*/ fft3->replot();}// fft3->yAxis->rescale();

//       average_state=0;
//    }


/*if(scope1  != nullptr) scope1->replot();
if(scope2  != nullptr) scope2->replot();
if(scope3  != nullptr) scope3->replot();
if(scope4  != nullptr) scope4->replot();
if(scope5  != nullptr) scope5->replot();
if(scope6  != nullptr) scope6->replot();
if(scope7  != nullptr) scope7->replot();
if(scope8  != nullptr) scope8->replot();
if(scope9  != nullptr) scope9->replot();
if(scope10 != nullptr) scope10->replot();
if(scope11 != nullptr) scope11->replot();
if(scope12 != nullptr) scope12->replot();
if(scope13 != nullptr) scope13->replot();
if(scope14 != nullptr) scope14->replot();
if(scope15 != nullptr) scope15->replot();
if(scope16 != nullptr) scope16->replot();

if(fft1  != nullptr) {/*fft1->yAxis->rescale();// fft1->replot();}
if(fft2  != nullptr) {fft2->replot();}
if(fft3  != nullptr) {/*fft3->graph(0)->data()->set(dataFFT3);//fft3->yAxis->rescale();// fft3->replot();}// fft3->yAxis->rescale();



if(fft4  != nullptr) fft4->replot();
if(fft5  != nullptr) fft5->replot();
if(fft6  != nullptr) fft6->replot();
if(fft7  != nullptr) fft7->replot();
if(fft8  != nullptr) fft8->replot();
if(fft9  != nullptr) fft9->replot();
if(fft10 != nullptr) fft10->replot();
if(fft11 != nullptr) fft11->replot();
if(fft12 != nullptr) fft12->replot();
if(fft13 != nullptr) fft13->replot();
if(fft14 != nullptr) fft14->replot();
if(fft15 != nullptr) fft15->replot();
if(fft16 != nullptr) fft16->replot();*/


//        if(sender == QHostAddress("192.168.4.2") )
//        {
//           memcpy(&pct_cnt1,&datagram.data()[0], 4);

//           if(play_stop && console1 != nullptr) console1->append("pkt_cnt= " + QString::number(pct_cnt1));

////           diff_pkt1 = abs(pct_cnt1 - pct_cnt1_old);
////           if(play_stop) diff1 += diff_pkt1-1;


////           if( diff_pkt1 > 1){
////               if(play_stop && console1 != nullptr) console1->append("pkt_cnt= " + QString::number(pct_cnt1) + "  Пропуск пакета !!!!");
////           }else{

////               if(pct_cnt1 == pct_cnt1_old){
////                   if(play_stop && console1 != nullptr) console1->append("pkt_cnt= " + QString::number(pct_cnt1) + "  Дубликат пакета !!!!");
////               }
////               else
////                    if(play_stop && console1 != nullptr) console1->append("pkt_cnt= " + QString::number(pct_cnt1) + "  Количество пропущенных пакетов: " + QString::number(diff1));
////           }

////           pct_cnt1_old = pct_cnt1;

//        }



//        if(sender == QHostAddress("192.168.4.3") )
//        {
//           memcpy(&pct_cnt2,&datagram.data()[0], 4);

//           diff_pkt2 = abs(pct_cnt2 - pct_cnt2_old);
//           if(play_stop) diff2 += diff_pkt2-1;

//           if( diff_pkt2 > 1){
//               if(play_stop && console3 != nullptr) console3->append("pkt_cnt= " + QString::number(pct_cnt2) + "  Пропуск пакета !!!!");
//           }else{

//               if(pct_cnt2 == pct_cnt2_old){
//                   if(play_stop && console3 != nullptr) console3->append("pkt_cnt= " + QString::number(pct_cnt2) + "  Дубликат пакета !!!!");
//               }
//               else
//                    if(play_stop && console3 != nullptr) console3->append("pkt_cnt= " + QString::number(pct_cnt2) + "  Количество пропущенных пакетов: " + QString::number(diff2));
//           }

//           pct_cnt2_old = pct_cnt2;

//        }




        //QString::number(line_cnt1,10) +
        //QString::number(line_cnt1) +

//           if( play_stop && console2 != nullptr){
//               line_cnt++;
//               console2->insertRow(console2->rowCount());


//               console2->setItem  (console2->rowCount()-1, 0, new QTableWidgetItem(QString::number(line_cnt)));
//               console2->item     (console2->rowCount()-1, 0)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);

//               if(pct_cnt == pct_cnt_old){
//                   console2->setItem  (console2->rowCount()-1, 2, new QTableWidgetItem("Дубликат!!!"));
//                   console2->item     (console2->rowCount()-1, 2)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
//               }

//               diff_pkt = abs(pct_cnt - pct_cnt_old);
//               if(  diff_pkt > 1){
//                   for(int i=0;i<diff_pkt;i++){
//                       console2->setItem  (console2->rowCount()-1, 1, new QTableWidgetItem("XXXX"));
//                       console2->item     (console2->rowCount()-1, 1)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
//                       console2->setItem  (console2->rowCount()-1, 2, new QTableWidgetItem("Пропуск пакета!!!"));
//                       console2->item     (console2->rowCount()-1, 2)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
//                   }
//               }
//               else{
//                   console2->setItem  (console2->rowCount()-1, 1, new QTableWidgetItem(QString::number(pct_cnt)));
//                   console2->item     (console2->rowCount()-1, 1)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
//               }
//               pct_cnt_old = pct_cnt;
//           }

/*
           console2->insertRow(console2->rowCount());
           console2->setItem  (console2->rowCount()-1, 0, new QTableWidgetItem("Канал [ " + QString::number(1) + " - " + QString::number(8) + " ]" ));
           console2->item     (console2->rowCount()-1, 0)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
           console2->item     (console2->rowCount()-1, 0)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

           console2->setItem  (console2->rowCount()-1, 1, new QTableWidgetItem("192.168.4." + QString::number(2)));
           console2->item     (console2->rowCount()-1, 1)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);*/




           /*if(console1 != nullptr){
               prev_cursor = console1->textCursor();
               console1->moveCursor (QTextCursor::End);

               if(pct_cnt == pct_cnt_old){
                   console1->insertPlainText (QString::number(line_cnt) + " packet_cnt= " + QString::number(pct_cnt) + "  DUPLICATE !!!!");
               }else{
                   console1->insertPlainText (QString::number(line_cnt) + " packet_cnt= " + QString::number(pct_cnt));
               }
               console1->setTextCursor (prev_cursor);
           }*/


           //line_cnt++;
           //pct_cnt_old = pct_cnt;
        //}


        //if(console1 != nullptr) console1->clear();

        //ui->textEdit->append(   QString(" 0x%1").arg(p[i],   2, 16, QChar('0'))




















//memcpy(&adc[j],&datagram.data()[0], datagram.size());


//txt->append("Sender: " + sender.toString());
//        j+=1408;
//        if(j==35200){
//            j=0;
//            memset(adc_ok,0x00,35200);
//            memcpy(adc_ok,adc,35200);
//        }


//               /*ui->Oscillo1->graph(0)->setData(x1, y1);
//               ui->Oscillo1->graph(1)->setData(x2, y2);
//               ui->Oscillo1->graph(2)->setData(x3, y3);
//               ui->Oscillo1->graph(3)->setData(x4, y4);

//               ui->Oscillo1->graph(4)->setData(x5, y5);
//               ui->Oscillo1->graph(5)->setData(x6, y6);
//               ui->Oscillo1->graph(6)->setData(x7, y7);
//               ui->Oscillo1->graph(7)->setData(x8, y8);*/

/*QVector<double> x1(2048), y1(2048);
QVector<double> x2(2048), y2(2048);
QVector<double> x3(2048), y3(2048);
QVector<double> x4(2048), y4(2048);
QVector<double> x5(2048), y5(2048);
QVector<double> x6(2048), y6(2048);
QVector<double> x7(2048), y7(2048);
QVector<double> x8(2048), y8(2048);*/

//QVector<std::array<double, 8>> scope_x(2048), scope_y(2048);
 //QVector< QVector<double, 8> > scope_x(2048);
//scope_x[2048][2048] = 1.0;


/*ch1_t = adc_m1[j];
ch1_t <<= 8;
ch1_t |= adc_m1[j+1];

ch2_t = adc_m1[j+2];
ch2_t <<= 8;
ch2_t |= adc_m1[j+3];

ch3_t = adc_m1[j+4];
ch3_t <<= 8;
ch3_t |= adc_m1[j+5];

ch4_t = adc_m1[j+6];
ch4_t <<= 8;
ch4_t |= adc_m1[j+7];

ch5_t = adc_m1[j+8];
ch5_t <<= 8;
ch5_t |= adc_m1[j+9];

ch6_t = adc_m1[j+10];
ch6_t <<= 8;
ch6_t |= adc_m1[j+11];

ch7_t = adc_m1[j+12];
ch7_t <<= 8;
ch7_t |= adc_m1[j+13];

ch8_t = adc_m1[j+14];
ch8_t <<= 8;
ch8_t |= adc_m1[j+15];*/



//    for(int i=1; i<9; i++){
//        //Scope: clear & replot
//        scope = window->findChild<QCustomPlot *>("ch"+QString::number(i));
//        if(scope != nullptr) {
//            scope->graph(0)->data()->clear();
//            //scope->replot();
//        }
//    }


/*for(int k=1; k<9; k++)
{
    //Scope: clear & replot
    scope = window->findChild<QCustomPlot *>("ch"+QString::number(k));
    if(scope != nullptr)
    {
        scope->graph(0)->addData(scope_x[k-1], scope_y[k-1]);
        //scope->replot();
    }
}*/

/*for(int i=1; i<9; i++){
    //Scope: clear & replot
    scope = window->findChild<QCustomPlot *>("ch"+QString::number(i));
    if(scope != nullptr) {
        //scope->graph(0)->data()->clear();
        scope->replot();
    }
}*/



//Trigger
//    for(uint16_t i=10; i<35200; i+=16)
//    {
//        //memcpy(&val16_1,&adc[i],2);
//        //memcpy(&val16_2,&adc[i+16],2);


//        val16_1 = adc_ok[i];
//        val16_1 <<= 8;
//        val16_1 |= adc_ok[i+1];

//        val16_2 = adc_ok[i+16];
//        val16_2 <<= 8;
//        val16_2 |= adc_ok[i+17];


//        sign1 = getSign(val16_1);
//        sign2 = getSign(val16_2);


//        //qDebug() << "sign1 = " << QString::number(val16_1);
//        //qDebug() << "sign2 = " << QString::number(val16_2);


//        if(sign1!=sign2)
//        {
////           qDebug() << "Zero cross find! "<< QString::number(i);
////           qDebug() << "val16_1 "<< QString::number(val16_1);
////           qDebug() << "val16_2 "<< QString::number(val16_2);



////           if( (val16_1>0) & (val16_2<0) ){
////               zero_cross_inx = i;
////           }
//           if( (val16_1<0) & (val16_2>0) )
//           {
//               zero_cross_inx = i+16;

//               ch1->graph(0)->data()->clear();
//               ch2->graph(0)->data()->clear();
//               ch3->graph(0)->data()->clear();
//               ch4->graph(0)->data()->clear();
//               ch5->graph(0)->data()->clear();
//               ch6->graph(0)->data()->clear();
//               ch7->graph(0)->data()->clear();
//               ch8->graph(0)->data()->clear();
//               ch9->graph(0)->data()->clear();
//               ch10->graph(0)->data()->clear();
//               ch11->graph(0)->data()->clear();
//               ch12->graph(0)->data()->clear();
//               ch13->graph(0)->data()->clear();
//               ch14->graph(0)->data()->clear();
//               ch15->graph(0)->data()->clear();
//               ch16->graph(0)->data()->clear();

//               i=0;
//               j=zero_cross_inx-10;
//               //while(i < ( 88*5 - 16))
//               while(i < ( 2048))
//               {
//                   //lsb = adc_ok[j];
//                   //msb = adc_ok[j+1];

//                   //lsb = adc_ok[j+1];
//                   //msb = adc_ok[j];

//                   //val16 = ((msb<<8)) | (lsb&0x00FF);
//                   //val16 = adc_ok[j]<<8 | adc_ok[j+1];


//                   val16_ch1 = adc_ok[j];
//                   val16_ch1 <<= 8;
//                   val16_ch1 |= adc_ok[j+1];

//                   val16_ch2 = adc_ok[j+2];
//                   val16_ch2 <<= 8;
//                   val16_ch2 |= adc_ok[j+3];

//                   val16_ch3 = adc_ok[j+4];
//                   val16_ch3 <<= 8;
//                   val16_ch3 |= adc_ok[j+5];

//                   val16_ch4 = adc_ok[j+6];
//                   val16_ch4 <<= 8;
//                   val16_ch4 |= adc_ok[j+7];

//                   val16_ch5 = adc_ok[j+8];
//                   val16_ch5 <<= 8;
//                   val16_ch5 |= adc_ok[j+9];

//                   val16_ch6 = adc_ok[j+10];
//                   val16_ch6 <<= 8;
//                   val16_ch6 |= adc_ok[j+11];

//                   val16_ch7 = adc_ok[j+12];
//                   val16_ch7 <<= 8;
//                   val16_ch7 |= adc_ok[j+13];

//                   val16_ch8 = adc_ok[j+14];
//                   val16_ch8 <<= 8;
//                   val16_ch8 |= adc_ok[j+15];

//                   y_ch1 = val16_ch1*0.000152;
//                   y_ch2 = val16_ch2*0.000152;
//                   y_ch3 = val16_ch3*0.000152;
//                   y_ch4 = val16_ch4*0.000152;
//                   y_ch5 = val16_ch5*0.000152;
//                   y_ch6 = val16_ch6*0.000152;
//                   y_ch7 = val16_ch7*0.000152;
//                   y_ch8 = val16_ch8*0.000152;


//                   adc_t+=1;
//                   x = adc_t;

//                   /*x1[i] = adc_t;
//                   y1[i] = y_ch1;

//                   x2[i] = adc_t;
//                   y2[i] = y_ch2;

//                   x3[i] = adc_t;
//                   y3[i] = y_ch3;

//                   x4[i] = adc_t;
//                   y4[i] = y_ch4;

//                   x5[i] = adc_t;
//                   y5[i] = y_ch5;

//                   x6[i] = adc_t;
//                   y6[i] = y_ch6;

//                   x7[i] = adc_t;
//                   y7[i] = y_ch7;

//                   x8[i] = adc_t;
//                   y8[i] = y_ch8;*/


//                   data_Ch1[i] = (float)y_ch1;
//                   data_Ch2[i] = (float)y_ch2;
//                   data_Ch3[i] = (float)y_ch3;
//                   data_Ch4[i] = (float)y_ch4;
//                   data_Ch5[i] = (float)y_ch5;
//                   data_Ch6[i] = (float)y_ch6;
//                   data_Ch7[i] = (float)y_ch7;
//                   data_Ch8[i] = (float)y_ch8;

////                   ui->Oscillo1->graph(0)->addData(x,y_ch1);
////                   ui->Oscillo1->graph(1)->addData(x,y_ch2);
////                   ui->Oscillo1->graph(2)->addData(x,y_ch3);
////                   ui->Oscillo1->graph(3)->addData(x,y_ch4);
////                   ui->Oscillo1->graph(4)->addData(x,y_ch5);
////                   ui->Oscillo1->graph(5)->addData(x,y_ch6);
////                   ui->Oscillo1->graph(6)->addData(x,y_ch7);
////                   ui->Oscillo1->graph(7)->addData(x,y_ch8);

//                   ch1->graph(0)->addData(x,y_ch1);
//                   ch2->graph(0)->addData(x,y_ch2);
//                   ch3->graph(0)->addData(x,y_ch3);
//                   ch4->graph(0)->addData(x,y_ch4);
//                   ch5->graph(0)->addData(x,y_ch5);
//                   ch6->graph(0)->addData(x,y_ch6);
//                   ch7->graph(0)->addData(x,y_ch7);
//                   ch8->graph(0)->addData(x,y_ch8);
//                   /*ch9->graph(0)->data()->clear();
//                   ch10->graph(0)->data()->clear();
//                   ch11->graph(0)->data()->clear();
//                   ch12->graph(0)->data()->clear();
//                   ch13->graph(0)->data()->clear();
//                   ch14->graph(0)->data()->clear();
//                   ch15->graph(0)->data()->clear();
//                   ch16->graph(0)->data()->clear();*/

//                   i++;
//                   j+=16;
//               }
//               i=0;
//               j=0;

//               /*ui->Oscillo1->graph(0)->setData(x1, y1);
//               ui->Oscillo1->graph(1)->setData(x2, y2);
//               ui->Oscillo1->graph(2)->setData(x3, y3);
//               ui->Oscillo1->graph(3)->setData(x4, y4);

//               ui->Oscillo1->graph(4)->setData(x5, y5);
//               ui->Oscillo1->graph(5)->setData(x6, y6);
//               ui->Oscillo1->graph(6)->setData(x7, y7);
//               ui->Oscillo1->graph(7)->setData(x8, y8);*/



//               ch1->replot();
//               ch2->replot();
//               ch3->replot();
//               ch4->replot();
//               ch5->replot();
//               ch6->replot();
//               ch7->replot();
//               ch8->replot();

//               /*********************************************************************************************************/
//               // создаем план для библиотеки fftw
//               double y;
//               double yl;


//               fftwf_plan  plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &data_Ch1[0], (fftwf_complex*) &data_Ch1[0], FFTW_FORWARD, FFTW_ESTIMATE);
//               fftwf_execute(plan);
//               fftwf_destroy_plan(plan);

//               plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &data_Ch2[0], (fftwf_complex*) &data_Ch2[0], FFTW_FORWARD, FFTW_ESTIMATE);
//               fftwf_execute(plan);
//               fftwf_destroy_plan(plan);

//               plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &data_Ch3[0], (fftwf_complex*) &data_Ch3[0], FFTW_FORWARD, FFTW_ESTIMATE);
//               fftwf_execute(plan);
//               fftwf_destroy_plan(plan);

//               plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &data_Ch4[0], (fftwf_complex*) &data_Ch4[0], FFTW_FORWARD, FFTW_ESTIMATE);
//               fftwf_execute(plan);
//               fftwf_destroy_plan(plan);

//               plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &data_Ch5[0], (fftwf_complex*) &data_Ch5[0], FFTW_FORWARD, FFTW_ESTIMATE);
//               fftwf_execute(plan);
//               fftwf_destroy_plan(plan);

//               plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &data_Ch6[0], (fftwf_complex*) &data_Ch6[0], FFTW_FORWARD, FFTW_ESTIMATE);
//               fftwf_execute(plan);
//               fftwf_destroy_plan(plan);

//               plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &data_Ch7[0], (fftwf_complex*) &data_Ch7[0], FFTW_FORWARD, FFTW_ESTIMATE);
//               fftwf_execute(plan);
//               fftwf_destroy_plan(plan);

//               plan = fftwf_plan_dft_1d(2048, (fftwf_complex*) &data_Ch8[0], (fftwf_complex*) &data_Ch8[0], FFTW_FORWARD, FFTW_ESTIMATE);
//               fftwf_execute(plan);
//               fftwf_destroy_plan(plan);




//               i=0;
//               x = 0;
//               adc_t=0.0;

//               //Harmonics
//               h1->graph(0)->data()->clear();
//               h2->graph(0)->data()->clear();
//               h3->graph(0)->data()->clear();
//               h4->graph(0)->data()->clear();
//               h5->graph(0)->data()->clear();
//               h6->graph(0)->data()->clear();
//               h7->graph(0)->data()->clear();
//               h8->graph(0)->data()->clear();



//               while(i < 2048)
//               {
//                   adc_t+=12.207;
//                   x = adc_t;
//                   //yl = 20*qLn(y/1024);
//                   y_ch1 = (double) (20* log10(abs(data_Ch1[i])/16384));
//                   y_ch2 = (double) (20* log10(abs(data_Ch2[i])/16384));
//                   y_ch3 = (double) (20* log10(abs(data_Ch3[i])/16384));
//                   y_ch4 = (double) (20* log10(abs(data_Ch4[i])/16384));
//                   y_ch5 = (double) (20* log10(abs(data_Ch5[i])/16384));
//                   y_ch6 = (double) (20* log10(abs(data_Ch6[i])/16384));
//                   y_ch7 = (double) (20* log10(abs(data_Ch7[i])/16384));
//                   y_ch8 = (double) (20* log10(abs(data_Ch8[i])/16384));


//                   h1->graph(0)->addData(x,y_ch1);
//                   h2->graph(0)->addData(x,y_ch2);
//                   h3->graph(0)->addData(x,y_ch3);
//                   h4->graph(0)->addData(x,y_ch4);
//                   h5->graph(0)->addData(x,y_ch5);
//                   h6->graph(0)->addData(x,y_ch6);
//                   h7->graph(0)->addData(x,y_ch7);
//                   h8->graph(0)->addData(x,y_ch8);
//                   i++;
//               }
//               i=0;
//               j=0;

//               //h1->yAxis->rescale();
//               //h2->yAxis->rescale();
//               //h3->yAxis->rescale();
//               //h4->yAxis->rescale();
//               //h5->yAxis->rescale();
//               //h6->yAxis->rescale();
//               //h7->yAxis->rescale();
//               //h8->yAxis->rescale();

//               h1->replot();
//               h2->replot();
//               h3->replot();
//               h4->replot();
//               h5->replot();
//               h6->replot();
//               h7->replot();
//               h8->replot();

//               break;
//           }
//        }
//    }





//    scope1_l = new QVBoxLayout();
//    scope1_l->addWidget(ch1);
//    scope1_l->addWidget(ch2);
//    scope1_l->addWidget(ch3);
//    scope1_l->addWidget(ch4);
//    scope1_l->addWidget(ch5);
//    scope1_l->addWidget(ch6);
//    scope1_l->addWidget(ch7);
//    scope1_l->addWidget(ch8);

//    QVBoxLayout *scope2_l = new QVBoxLayout();
//    scope2_l->addWidget(ch9);
//    scope2_l->addWidget(ch10);
//    scope2_l->addWidget(ch11);
//    scope2_l->addWidget(ch12);
//    scope2_l->addWidget(ch13);
//    scope2_l->addWidget(ch14);
//    scope2_l->addWidget(ch15);
//    scope2_l->addWidget(ch16);

//    QVBoxLayout *harm1_l = new QVBoxLayout();
//    harm1_l->addWidget(h1);
//    harm1_l->addWidget(h2);
//    harm1_l->addWidget(h3);
//    harm1_l->addWidget(h4);
//    harm1_l->addWidget(h5);
//    harm1_l->addWidget(h6);
//    harm1_l->addWidget(h7);
//    harm1_l->addWidget(h8);

//    QVBoxLayout *debug_l = new QVBoxLayout();

//    txt = new QTextEdit();
//    //txt->setText("Hello, world!");
//    //txt->append("Appending some text…");

//    QPalette p = palette();
//    p.setColor(QPalette::Base, Qt::black);
//    p.setColor(QPalette::Text, Qt::green);
//    txt->setPalette(p);




//    debug_l->addWidget(txt);

//    scope1_w = new QWidget;
//    scope1_w->setLayout(scope1_l);

//    auto scope2_w = new QWidget;
//    scope2_w->setLayout(scope2_l);

//    auto harm1_w = new QWidget;
//    harm1_w->setLayout(harm1_l);

//    auto debug_w = new QWidget;
//    debug_w->setLayout(debug_l);



//    scroll_tab1 = new QScrollArea();
//    scroll_tab1->setBackgroundRole(QPalette::Shadow);
//    scroll_tab1->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
//    scroll_tab1->setWidgetResizable( true );
//    scroll_tab1->setWidget(scope1_w);

//    scroll_tab2 = new QScrollArea();
//    scroll_tab2->setBackgroundRole(QPalette::Shadow);
//    scroll_tab2->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
//    scroll_tab2->setWidgetResizable( true );
//    scroll_tab2->setWidget(scope2_w);

//    scroll_tab3 = new QScrollArea();
//    scroll_tab3->setBackgroundRole(QPalette::Shadow);
//    scroll_tab3->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
//    scroll_tab3->setWidgetResizable( true );
//    scroll_tab3->setWidget(harm1_w);

//    tw = new QTabWidget(this);
//    tw->addTab(scroll_tab1, "Канал [1 - 8]");
//    tw->addTab(scroll_tab2, "Канал [9 - 16]");
//    tw->addTab(scroll_tab3, "Спектр [1 - 8]");

//    //tw->addTab(new QWidget, "Спектр [9 - 16]");
//    //tw->addTab(debug_w, "Отладка");
//    //scrollarea->setWidget(tw);

//**************************************************************************************************/
//Scope


/*QGridLayout * layout3 = new QGridLayout(ch1);
layout3->setAlignment(Qt::AlignRight | Qt::AlignTop);

QCheckBox * trg1 = new QCheckBox(this);
//trg1->setText("Триггер");
trg1->setIcon();

layout3->addWidget(trg1 , 0, 0);*/


//    ch1  = new QCustomPlot(this);
//    ch2  = new QCustomPlot(this);
//    ch3  = new QCustomPlot(this);
//    ch4  = new QCustomPlot(this);
//    ch5  = new QCustomPlot(this);
//    ch6  = new QCustomPlot(this);
//    ch7  = new QCustomPlot(this);
//    ch8  = new QCustomPlot(this);

//    ch9  = new QCustomPlot(this);
//    ch10 = new QCustomPlot(this);
//    ch11 = new QCustomPlot(this);
//    ch12 = new QCustomPlot(this);
//    ch13 = new QCustomPlot(this);
//    ch14 = new QCustomPlot(this);
//    ch15 = new QCustomPlot(this);
//    ch16 = new QCustomPlot(this);


//    ch1->addGraph();
//    ch1->xAxis->setRange(0,2047);
//    ch1->yAxis->setRange(-6,6);
//    ch1->setMinimumHeight(MinimumHeight);

//    ch2->addGraph();
//    ch2->xAxis->setRange(0,2047);
//    ch2->yAxis->setRange(-6,6);
//    ch2->setMinimumHeight(MinimumHeight);

//    ch3->addGraph();
//    ch3->xAxis->setRange(0,2047);
//    ch3->yAxis->setRange(-6,6);
//    ch3->setMinimumHeight(MinimumHeight);

//    ch4->addGraph();
//    ch4->xAxis->setRange(0,2047);
//    ch4->yAxis->setRange(-6,6);
//    ch4->setMinimumHeight(MinimumHeight);

//    ch5->addGraph();
//    ch5->xAxis->setRange(0,2047);
//    ch5->yAxis->setRange(-6,6);
//    ch5->setMinimumHeight(MinimumHeight);

//    ch6->addGraph();
//    ch6->xAxis->setRange(0,2047);
//    ch6->yAxis->setRange(-6,6);
//    ch6->setMinimumHeight(MinimumHeight);

//    ch7->addGraph();
//    ch7->xAxis->setRange(0,2047);
//    ch7->yAxis->setRange(-6,6);
//    ch7->setMinimumHeight(MinimumHeight);

//    ch8->addGraph();
//    ch8->xAxis->setRange(0,2047);
//    ch8->yAxis->setRange(-6,6);
//    ch8->setMinimumHeight(MinimumHeight);

//    ch9->addGraph();
//    ch9->xAxis->setRange(0,2047);
//    ch9->yAxis->setRange(-6,6);

//    ch10->addGraph();
//    ch10->xAxis->setRange(0,2047);
//    ch10->yAxis->setRange(-6,6);

//    ch11->addGraph();
//    ch11->xAxis->setRange(0,2047);
//    ch11->yAxis->setRange(-6,6);

//    ch12->addGraph();
//    ch12->xAxis->setRange(0,2047);
//    ch12->yAxis->setRange(-6,6);

//    ch13->addGraph();
//    ch13->xAxis->setRange(0,2047);
//    ch13->yAxis->setRange(-6,6);

//    ch14->addGraph();
//    ch14->xAxis->setRange(0,2047);
//    ch14->yAxis->setRange(-6,6);

//    ch15->addGraph();
//    ch15->xAxis->setRange(0,2047);
//    ch15->yAxis->setRange(-6,6);

//    ch16->addGraph();
//    ch16->xAxis->setRange(0,2047);
//    ch16->yAxis->setRange(-6,6);

//************************************************************************************************************/
//Chanel 1
//    ch1->graph(0)->setName("Канал 1");
//    ch1->legend->setVisible(true);
//    ch1->legend->setFont(QFont(QFont().family(), 8));
//    ch1->legend->setBrush(QColor(255, 255, 255, 150));
//    ch1->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch1->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch1->setBackground(QBrush(Qt::black));
//    ch1->graph(0)->setPen(QPen(Qt::green));
//    // set some pens, brushes and backgrounds:
//    ch1->xAxis->setBasePen(QPen(Qt::white, 1));
//    ch1->yAxis->setBasePen(QPen(Qt::white, 1));
//    ch1->xAxis->setTickPen(QPen(Qt::white, 1));
//    ch1->yAxis->setTickPen(QPen(Qt::white, 1));
//    ch1->xAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch1->yAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch1->xAxis->setTickLabelColor(Qt::white);
//    ch1->yAxis->setTickLabelColor(Qt::white);
//    ch1->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch1->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch1->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch1->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch1->xAxis->grid()->setSubGridVisible(true);
//    ch1->yAxis->grid()->setSubGridVisible(false);
//    ch1->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch1->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch1->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    ch1->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//************************************************************************************************************/
//Chanel 2
//    ch2->graph(0)->setName("Канал 2");
//    ch2->legend->setVisible(true);
//    ch2->legend->setFont(QFont(QFont().family(), 8));
//    ch2->legend->setBrush(QColor(255, 255, 255, 150));
//    ch2->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch2->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch2->setBackground(QBrush(Qt::black));
//    ch2->graph(0)->setPen(QPen(Qt::green));
//    // set some pens, brushes and backgrounds:
//    ch2->xAxis->setBasePen(QPen(Qt::white, 1));
//    ch2->yAxis->setBasePen(QPen(Qt::white, 1));
//    ch2->xAxis->setTickPen(QPen(Qt::white, 1));
//    ch2->yAxis->setTickPen(QPen(Qt::white, 1));
//    ch2->xAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch2->yAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch2->xAxis->setTickLabelColor(Qt::white);
//    ch2->yAxis->setTickLabelColor(Qt::white);
//    ch2->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch2->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch2->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch2->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch2->xAxis->grid()->setSubGridVisible(true);
//    ch2->yAxis->grid()->setSubGridVisible(false);
//    ch2->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch2->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch2->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    ch2->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//************************************************************************************************************/
//Chanel 3
//    ch3->graph(0)->setName("Канал 3");
//    ch3->legend->setVisible(true);
//    ch3->legend->setFont(QFont(QFont().family(), 8));
//    ch3->legend->setBrush(QColor(255, 255, 255, 150));
//    // set a more compact font size for bottom and left axis tick labels:
//    ch3->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch3->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    //ch3->xAxis->setLabel("Напряжение");
//    //ch3->yAxis->setLabel("Напряжение");
//    //ch3->xAxis->setLabelColor(Qt::white);
//    //ch3->yAxis->setLabelColor(Qt::white);
//    //ch3->xAxis->setLabelFont(QFont(QFont().family(), 8));
//    //ch3->yAxis->setLabelFont(QFont(QFont().family(), 8));

//    ch3->setBackground(QBrush(Qt::black));
//    ch3->graph(0)->setPen(QPen(Qt::green));
//    // set some pens, brushes and backgrounds:
//    ch3->xAxis->setBasePen(QPen(Qt::white, 1));
//    ch3->yAxis->setBasePen(QPen(Qt::white, 1));
//    ch3->xAxis->setTickPen(QPen(Qt::white, 1));
//    ch3->yAxis->setTickPen(QPen(Qt::white, 1));
//    ch3->xAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch3->yAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch3->xAxis->setTickLabelColor(Qt::white);
//    ch3->yAxis->setTickLabelColor(Qt::white);
//    ch3->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch3->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch3->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch3->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch3->xAxis->grid()->setSubGridVisible(true);
//    ch3->yAxis->grid()->setSubGridVisible(false);
//    ch3->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch3->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch3->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    ch3->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//************************************************************************************************************/
//Chanel 4
//    ch4->graph(0)->setName("Канал 4");
//    ch4->legend->setVisible(true);
//    ch4->legend->setFont(QFont(QFont().family(), 8));
//    ch4->legend->setBrush(QColor(255, 255, 255, 150));
//    ch4->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch4->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch4->setBackground(QBrush(Qt::black));
//    ch4->graph(0)->setPen(QPen(Qt::green));
//    // set some pens, brushes and backgrounds:
//    ch4->xAxis->setBasePen(QPen(Qt::white, 1));
//    ch4->yAxis->setBasePen(QPen(Qt::white, 1));
//    ch4->xAxis->setTickPen(QPen(Qt::white, 1));
//    ch4->yAxis->setTickPen(QPen(Qt::white, 1));
//    ch4->xAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch4->yAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch4->xAxis->setTickLabelColor(Qt::white);
//    ch4->yAxis->setTickLabelColor(Qt::white);
//    ch4->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch4->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch4->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch4->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch4->xAxis->grid()->setSubGridVisible(true);
//    ch4->yAxis->grid()->setSubGridVisible(false);
//    ch4->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch4->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch4->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    ch4->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//************************************************************************************************************/
//Chanel 5
//    ch5->graph(0)->setName("Канал 5");
//    ch5->legend->setVisible(true);
//    ch5->legend->setFont(QFont(QFont().family(), 8));
//    ch5->legend->setBrush(QColor(255, 255, 255, 150));
//    ch5->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch5->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch5->setBackground(QBrush(Qt::black));
//    ch5->graph(0)->setPen(QPen(Qt::green));
//    // set some pens, brushes and backgrounds:
//    ch5->xAxis->setBasePen(QPen(Qt::white, 1));
//    ch5->yAxis->setBasePen(QPen(Qt::white, 1));
//    ch5->xAxis->setTickPen(QPen(Qt::white, 1));
//    ch5->yAxis->setTickPen(QPen(Qt::white, 1));
//    ch5->xAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch5->yAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch5->xAxis->setTickLabelColor(Qt::white);
//    ch5->yAxis->setTickLabelColor(Qt::white);
//    ch5->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch5->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch5->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch5->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch5->xAxis->grid()->setSubGridVisible(true);
//    ch5->yAxis->grid()->setSubGridVisible(false);
//    ch5->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch5->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch5->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    ch5->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//************************************************************************************************************/
//Chanel 6
//    ch6->graph(0)->setName("Канал 6");
//    ch6->legend->setVisible(true);
//    ch6->legend->setFont(QFont(QFont().family(), 8));
//    ch6->legend->setBrush(QColor(255, 255, 255, 150));
//    ch6->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch6->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch6->setBackground(QBrush(Qt::black));
//    ch6->graph(0)->setPen(QPen(Qt::green));
//    // set some pens, brushes and backgrounds:
//    ch6->xAxis->setBasePen(QPen(Qt::white, 1));
//    ch6->yAxis->setBasePen(QPen(Qt::white, 1));
//    ch6->xAxis->setTickPen(QPen(Qt::white, 1));
//    ch6->yAxis->setTickPen(QPen(Qt::white, 1));
//    ch6->xAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch6->yAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch6->xAxis->setTickLabelColor(Qt::white);
//    ch6->yAxis->setTickLabelColor(Qt::white);
//    ch6->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch6->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch6->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch6->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch6->xAxis->grid()->setSubGridVisible(true);
//    ch6->yAxis->grid()->setSubGridVisible(false);
//    ch6->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch6->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch6->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    ch6->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//************************************************************************************************************/
//Chanel 7
//    ch7->graph(0)->setName("Канал 7");
//    ch7->legend->setVisible(true);
//    ch7->legend->setFont(QFont(QFont().family(), 8));
//    ch7->legend->setBrush(QColor(255, 255, 255, 150));
//    ch7->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch7->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch7->setBackground(QBrush(Qt::black));
//    ch7->graph(0)->setPen(QPen(Qt::green));
//    // set some pens, brushes and backgrounds:
//    ch7->xAxis->setBasePen(QPen(Qt::white, 1));
//    ch7->yAxis->setBasePen(QPen(Qt::white, 1));
//    ch7->xAxis->setTickPen(QPen(Qt::white, 1));
//    ch7->yAxis->setTickPen(QPen(Qt::white, 1));
//    ch7->xAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch7->yAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch7->xAxis->setTickLabelColor(Qt::white);
//    ch7->yAxis->setTickLabelColor(Qt::white);
//    ch7->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch7->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch7->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch7->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch7->xAxis->grid()->setSubGridVisible(true);
//    ch7->yAxis->grid()->setSubGridVisible(false);
//    ch7->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch7->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch7->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    ch7->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//************************************************************************************************************/
//Chanel 8
//    ch8->graph(0)->setName("Канал 8");
//    ch8->legend->setVisible(true);
//    ch8->legend->setFont(QFont(QFont().family(), 8));
//    ch8->legend->setBrush(QColor(255, 255, 255, 150));
//    ch8->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch8->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch8->setBackground(QBrush(Qt::black));
//    ch8->graph(0)->setPen(QPen(Qt::green));
//    // set some pens, brushes and backgrounds:
//    ch8->xAxis->setBasePen(QPen(Qt::white, 1));
//    ch8->yAxis->setBasePen(QPen(Qt::white, 1));
//    ch8->xAxis->setTickPen(QPen(Qt::white, 1));
//    ch8->yAxis->setTickPen(QPen(Qt::white, 1));
//    ch8->xAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch8->yAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch8->xAxis->setTickLabelColor(Qt::white);
//    ch8->yAxis->setTickLabelColor(Qt::white);
//    ch8->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch8->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch8->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch8->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch8->xAxis->grid()->setSubGridVisible(true);
//    ch8->yAxis->grid()->setSubGridVisible(false);
//    ch8->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch8->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch8->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    ch8->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//************************************************************************************************************/
//Chanel 9
//    ch9->graph(0)->setName("Канал 9");
//    ch9->legend->setVisible(true);
//    ch9->legend->setFont(QFont(QFont().family(), 8));
//    ch9->legend->setBrush(QColor(255, 255, 255, 150));
//    ch9->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch9->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch9->setBackground(QBrush(Qt::black));
//    ch9->graph(0)->setPen(QPen(Qt::green));
//    // set some pens, brushes and backgrounds:
//    ch9->xAxis->setBasePen(QPen(Qt::white, 1));
//    ch9->yAxis->setBasePen(QPen(Qt::white, 1));
//    ch9->xAxis->setTickPen(QPen(Qt::white, 1));
//    ch9->yAxis->setTickPen(QPen(Qt::white, 1));
//    ch9->xAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch9->yAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch9->xAxis->setTickLabelColor(Qt::white);
//    ch9->yAxis->setTickLabelColor(Qt::white);
//    ch9->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch9->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch9->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch9->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch9->xAxis->grid()->setSubGridVisible(true);
//    ch9->yAxis->grid()->setSubGridVisible(false);
//    ch9->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch9->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch9->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    ch9->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//    //************************************************************************************************************/
//    //Chanel 10
//    ch10->graph(0)->setName("Канал 10");
//    ch10->legend->setVisible(true);
//    ch10->legend->setFont(QFont(QFont().family(), 8));
//    ch10->legend->setBrush(QColor(255, 255, 255, 150));
//    ch10->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch10->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch10->setBackground(QBrush(Qt::black));
//    ch10->graph(0)->setPen(QPen(Qt::green));
//    // set some pens, brushes and backgrounds:
//    ch10->xAxis->setBasePen(QPen(Qt::white, 1));
//    ch10->yAxis->setBasePen(QPen(Qt::white, 1));
//    ch10->xAxis->setTickPen(QPen(Qt::white, 1));
//    ch10->yAxis->setTickPen(QPen(Qt::white, 1));
//    ch10->xAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch10->yAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch10->xAxis->setTickLabelColor(Qt::white);
//    ch10->yAxis->setTickLabelColor(Qt::white);
//    ch10->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch10->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch10->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch10->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch10->xAxis->grid()->setSubGridVisible(true);
//    ch10->yAxis->grid()->setSubGridVisible(false);
//    ch10->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch10->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch10->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    ch10->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//    //************************************************************************************************************/
//    //Chanel 11
//    ch11->graph(0)->setName("Канал 11");
//    ch11->legend->setVisible(true);
//    ch11->legend->setFont(QFont(QFont().family(), 8));
//    ch11->legend->setBrush(QColor(255, 255, 255, 150));
//    ch11->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch11->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch11->setBackground(QBrush(Qt::black));
//    ch11->graph(0)->setPen(QPen(Qt::green));
//    // set some pens, brushes and backgrounds:
//    ch11->xAxis->setBasePen(QPen(Qt::white, 1));
//    ch11->yAxis->setBasePen(QPen(Qt::white, 1));
//    ch11->xAxis->setTickPen(QPen(Qt::white, 1));
//    ch11->yAxis->setTickPen(QPen(Qt::white, 1));
//    ch11->xAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch11->yAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch11->xAxis->setTickLabelColor(Qt::white);
//    ch11->yAxis->setTickLabelColor(Qt::white);
//    ch11->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch11->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch11->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch11->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch11->xAxis->grid()->setSubGridVisible(true);
//    ch11->yAxis->grid()->setSubGridVisible(false);
//    ch11->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch11->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch11->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    ch11->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//    //************************************************************************************************************/
//    //Chanel 12
//    ch12->graph(0)->setName("Канал 12");
//    ch12->legend->setVisible(true);
//    ch12->legend->setFont(QFont(QFont().family(), 8));
//    ch12->legend->setBrush(QColor(255, 255, 255, 150));
//    ch12->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch12->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch12->setBackground(QBrush(Qt::black));
//    ch12->graph(0)->setPen(QPen(Qt::green));
//    // set some pens, brushes and backgrounds:
//    ch12->xAxis->setBasePen(QPen(Qt::white, 1));
//    ch12->yAxis->setBasePen(QPen(Qt::white, 1));
//    ch12->xAxis->setTickPen(QPen(Qt::white, 1));
//    ch12->yAxis->setTickPen(QPen(Qt::white, 1));
//    ch12->xAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch12->yAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch12->xAxis->setTickLabelColor(Qt::white);
//    ch12->yAxis->setTickLabelColor(Qt::white);
//    ch12->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch12->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch12->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch12->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch12->xAxis->grid()->setSubGridVisible(true);
//    ch12->yAxis->grid()->setSubGridVisible(false);
//    ch12->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch12->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch12->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    ch12->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//    //************************************************************************************************************/
//    //Chanel 13
//    ch13->graph(0)->setName("Канал 13");
//    ch13->legend->setVisible(true);
//    ch13->legend->setFont(QFont(QFont().family(), 8));
//    ch13->legend->setBrush(QColor(255, 255, 255, 150));
//    ch13->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch13->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch13->setBackground(QBrush(Qt::black));
//    ch13->graph(0)->setPen(QPen(Qt::green));
//    // set some pens, brushes and backgrounds:
//    ch13->xAxis->setBasePen(QPen(Qt::white, 1));
//    ch13->yAxis->setBasePen(QPen(Qt::white, 1));
//    ch13->xAxis->setTickPen(QPen(Qt::white, 1));
//    ch13->yAxis->setTickPen(QPen(Qt::white, 1));
//    ch13->xAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch13->yAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch13->xAxis->setTickLabelColor(Qt::white);
//    ch13->yAxis->setTickLabelColor(Qt::white);
//    ch13->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch13->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch13->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch13->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch13->xAxis->grid()->setSubGridVisible(true);
//    ch13->yAxis->grid()->setSubGridVisible(false);
//    ch13->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch13->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch13->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    ch13->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//    //************************************************************************************************************/
//    //Chanel 14
//    ch14->graph(0)->setName("Канал 14");
//    ch14->legend->setVisible(true);
//    ch14->legend->setFont(QFont(QFont().family(), 8));
//    ch14->legend->setBrush(QColor(255, 255, 255, 150));
//    ch14->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch14->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch14->setBackground(QBrush(Qt::black));
//    ch14->graph(0)->setPen(QPen(Qt::green));
//    // set some pens, brushes and backgrounds:
//    ch14->xAxis->setBasePen(QPen(Qt::white, 1));
//    ch14->yAxis->setBasePen(QPen(Qt::white, 1));
//    ch14->xAxis->setTickPen(QPen(Qt::white, 1));
//    ch14->yAxis->setTickPen(QPen(Qt::white, 1));
//    ch14->xAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch14->yAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch14->xAxis->setTickLabelColor(Qt::white);
//    ch14->yAxis->setTickLabelColor(Qt::white);
//    ch14->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch14->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch14->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch14->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch14->xAxis->grid()->setSubGridVisible(true);
//    ch14->yAxis->grid()->setSubGridVisible(false);
//    ch14->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch14->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch14->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    ch14->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//    //************************************************************************************************************/
//    //Chanel 15
//    ch15->graph(0)->setName("Канал 15");
//    ch15->legend->setVisible(true);
//    ch15->legend->setFont(QFont(QFont().family(), 8));
//    ch15->legend->setBrush(QColor(255, 255, 255, 150));
//    ch15->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch15->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch15->setBackground(QBrush(Qt::black));
//    ch15->graph(0)->setPen(QPen(Qt::green));
//    // set some pens, brushes and backgrounds:
//    ch15->xAxis->setBasePen(QPen(Qt::white, 1));
//    ch15->yAxis->setBasePen(QPen(Qt::white, 1));
//    ch15->xAxis->setTickPen(QPen(Qt::white, 1));
//    ch15->yAxis->setTickPen(QPen(Qt::white, 1));
//    ch15->xAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch15->yAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch15->xAxis->setTickLabelColor(Qt::white);
//    ch15->yAxis->setTickLabelColor(Qt::white);
//    ch15->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch15->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch15->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch15->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch15->xAxis->grid()->setSubGridVisible(true);
//    ch15->yAxis->grid()->setSubGridVisible(false);
//    ch15->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch15->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch15->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    ch15->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//    //************************************************************************************************************/
//    //Chanel 16
//    ch16->graph(0)->setName("Канал 16");
//    ch16->legend->setVisible(true);
//    ch16->legend->setFont(QFont(QFont().family(), 8));
//    ch16->legend->setBrush(QColor(255, 255, 255, 150));
//    ch16->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch16->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//    ch16->setBackground(QBrush(Qt::black));
//    ch16->graph(0)->setPen(QPen(Qt::green));
//    // set some pens, brushes and backgrounds:
//    ch16->xAxis->setBasePen(QPen(Qt::white, 1));
//    ch16->yAxis->setBasePen(QPen(Qt::white, 1));
//    ch16->xAxis->setTickPen(QPen(Qt::white, 1));
//    ch16->yAxis->setTickPen(QPen(Qt::white, 1));
//    ch16->xAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch16->yAxis->setSubTickPen(QPen(Qt::white, 1));
//    ch16->xAxis->setTickLabelColor(Qt::white);
//    ch16->yAxis->setTickLabelColor(Qt::white);
//    ch16->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch16->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ch16->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch16->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ch16->xAxis->grid()->setSubGridVisible(true);
//    ch16->yAxis->grid()->setSubGridVisible(false);
//    ch16->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch16->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ch16->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    ch16->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);



//    ch1->replot();
//    ch2->replot();
//    ch3->replot();
//    ch4->replot();
//    ch5->replot();
//    ch6->replot();
//    ch7->replot();
//    ch8->replot();
//    ch9->replot();
//    ch10->replot();
//    ch11->replot();
//    ch12->replot();
//    ch13->replot();
//    ch14->replot();
//    ch15->replot();
//    ch16->replot();


/*auto wdg1 = new QWidget;
QVBoxLayout *layout = new QVBoxLayout();
wdg1->setLayout( layout );*/

/*layout->addWidget(ch1);
layout->addWidget(ch2);
layout->addWidget(ch3);
layout->addWidget(ch4);
layout->addWidget(ch5);
layout->addWidget(ch6);
layout->addWidget(ch7);
layout->addWidget(ch8);*/

/* for (int i = 0; i < 100; i++)
{
    QPushButton *button = new QPushButton( QString( "%1" ).arg( i ) );
    layout->addWidget( button );
}*/





//QHBoxLayout *mainLayout = new QHBoxLayout();
//QWidget *wgt = new QWidget();




































//const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));

//QPixmap(":/images/play.png")
//QPixmap pixmap(100,100);
//pixmap.fill(QColor("red"));


//const QIcon newIcon = QIcon(QPixmap(":/images/play.png"));

//newAct->setShortcuts(QKeySequence::New);
//newAct->setStatusTip(tr("Create a new file"));
//connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
//fileMenu->addAction(newAct);

//connect(ui->actionStart, &QAction::triggered, this, &MainWindow::PlayStop);
//connect(m_ui->actionDisconnect, &QAction::triggered, this, &FlashLoader::closeSerialPort);
//connect(m_ui->actionQuit,       &QAction::triggered, this, &FlashLoader::close);
//connect(m_ui->actionConfigure,  &QAction::triggered, m_settings, &SettingsDialog::show);



//QMenuBar* menu = new QMenuBar();
//menu->addAction( "action 1" );
//menu->addAction( "action 2" );
//ui->menuBar->addAction("action 1");
// menu->setLayoutDirection( Qt::RightToLeft); // Display menu bar to the right

// Set widget on the top left corner
//QPushButton* menuTopRightButton = new QPushButton("TL", m_menubar);


// create Tool Bar with 3 buttons //
//QToolBar *toolbar = new QToolBar(this);



//ui->menuBar->setVisible(false);

/*p = (unsigned char *) calloc(800000000, sizeof(unsigned char)); //800 Mb
if(!p)
{
    qDebug()<<"Allocation memory failure";
    exit (1);
}
else
    qDebug()<<"Allocation memory ok!";*/

//ui->textEdit->setReadOnly(true);

//    ui->Oscillo1->addGraph();
//    ui->Oscillo1->addGraph();
//    ui->Oscillo1->addGraph();
//    ui->Oscillo1->addGraph();
//    ui->Oscillo1->addGraph();
//    ui->Oscillo1->addGraph();
//    ui->Oscillo1->addGraph();
//    ui->Oscillo1->addGraph();

//    ui->Oscillo1->graph(0)->setPen(QPen(Qt::red));
//    ui->Oscillo1->graph(1)->setPen(QPen(Qt::gray));
//    ui->Oscillo1->graph(2)->setPen(QPen(Qt::green));
//    ui->Oscillo1->graph(3)->setPen(QPen(Qt::yellow));
//    ui->Oscillo1->graph(4)->setPen(QPen(Qt::blue));
//    ui->Oscillo1->graph(5)->setPen(QPen(Qt::magenta));
//    ui->Oscillo1->graph(6)->setPen(QPen(Qt::darkCyan));
//    ui->Oscillo1->graph(7)->setPen(QPen(Qt::darkYellow));

//    ui->Oscillo1->graph(0)->setName("ch1");
//    ui->Oscillo1->graph(1)->setName("ch2");
//    ui->Oscillo1->graph(2)->setName("ch3");
//    ui->Oscillo1->graph(3)->setName("ch4");
//    ui->Oscillo1->graph(4)->setName("ch5");
//    ui->Oscillo1->graph(5)->setName("ch6");
//    ui->Oscillo1->graph(6)->setName("ch7");
//    ui->Oscillo1->graph(7)->setName("ch8");

//    ui->Oscillo1->graph(0)->setAntialiasedFill(false);
//    ui->Oscillo1->graph(1)->setAntialiasedFill(false);
//    ui->Oscillo1->graph(2)->setAntialiasedFill(false);
//    ui->Oscillo1->graph(3)->setAntialiasedFill(false);
//    ui->Oscillo1->graph(4)->setAntialiasedFill(false);
//    ui->Oscillo1->graph(5)->setAntialiasedFill(false);
//    ui->Oscillo1->graph(6)->setAntialiasedFill(false);
//    ui->Oscillo1->graph(7)->setAntialiasedFill(false);



//    ui->Oscillo1->legend->setVisible(true);
//    ui->Oscillo1->legend->setFont(QFont("Helvetica", 12));

//    QCPLayoutGrid *subLayout = new QCPLayoutGrid;
//    subLayout->setMargins(QMargins(5, 0, 5, 5));
//    subLayout->addElement(0, 0, ui->Oscillo1->legend);
//    ui->Oscillo1->plotLayout()->addElement(1, 0, subLayout);
//    // change the fill order of the legend, so it's filled left to right in columns:
//    ui->Oscillo1->legend->setFillOrder(QCPLegend::foColumnsFirst);
//    // set legend's row stretch factor very small so it ends up with minimum height:
//    ui->Oscillo1->plotLayout()->setRowStretchFactor(1, 0.001);



//    // change the fill order of the legend, so it's filled left to right in columns:
//    //ui->Oscillo1->legend->setFillOrder(QCPLegend::foColumnsFirst);
//    // set legend's row stretch factor very small so it ends up with minimum height:
//    //ui->Oscillo1->plotLayout()->setRowStretchFactor(1, 0.001);

//    //ui->Oscillo1->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignBottom);
//    //ui->Oscillo1->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom);

//    //ui->Oscillo1->xAxis->setTickLabelType(QCPAxis::ltDateTime);
//    //ui->Oscillo1->xAxis->setDateTimeFormat("zzz");
//    //ui->Oscillo1->xAxis->setTickStep(0.05);

//    ui->Oscillo1->xAxis->setRange(0,440);
//    //ui->Oscillo1->yAxis->setRange(-40000,40000);
//    ui->Oscillo1->yAxis->setRange(-5,5);
//    //ui->Oscillo1->yAxis->setRange(-256,256);

//    //ui->Oscillo1->xAxis->setAutoTickStep(true); ??

//    //ui->Oscillo1->xAxis->setNumberPrecision(12);


//    ui->Oscillo1->axisRect()->setupFullAxesBox();
//    ui->Oscillo1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);


//    connect(ui->Oscillo1, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(mouseRelease(QMouseEvent*)));
//    connect(ui->Oscillo1, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress(QMouseEvent*)));
//    connect(ui->Oscillo1, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
//    //connect(ui->Oscillo1->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->Oscillo1->xAxis2, SLOT(setRange(QCPRange)));
//    //connect(ui->Oscillo1->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->Oscillo1->yAxis2, SLOT(setRange(QCPRange)));

//    // make left and bottom axes transfer their ranges to right and top axes:
//    connect(ui->Oscillo1->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->Oscillo1->xAxis2, SLOT(setRange(QCPRange)));
//    //connect(ui->Oscillo1->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->Oscillo1->yAxis2, SLOT(setRange(QCPRange)));




//    ui->FFTW->addGraph();
//    ui->FFTW->graph(0)->setPen(QPen(Qt::red));
//    ui->FFTW->graph(0)->setName("ch1");
//    ui->FFTW->graph(0)->setAntialiasedFill(false);
//    ui->FFTW->legend->setVisible(true);
//    ui->FFTW->legend->setFont(QFont("Helvetica", 12));
//    ui->FFTW->xAxis->setRange(0,12500);
//    ui->FFTW->yAxis->setRange(0,2048);
//    ui->FFTW->axisRect()->setupFullAxesBox();







//************************************************************************************************************************
    // Create harmonics bars
/*H_ch1 = new QCPBars(ui->FFTW->xAxis, ui->FFTW->yAxis);
ui->FFTW->addPlottable(H_ch1);

QFont legendFont;
QPen pen,legendPen;
pen.setWidthF(1.2);
pen.setColor(QColor(0, 0, 255));
H_ch1->setPen(pen);
H_ch1->setName("Ch1");
H_ch1->setBrush(QColor(0, 0, 255, 50));

ui->FFTW->setBackground(ui->tabWidget->palette().background().color());
//ui->FFTW->xAxis->setRange(0, 132);
//ui->FFTW->yAxis->setRange(0, 100);

ui->FFTW->xAxis->setRange(0, 128);
ui->FFTW->yAxis->setRange(0, 512);

ui->FFTW->xAxis->grid()->setVisible(true);
ui->FFTW->yAxis->grid()->setSubGridVisible(true);
ui->FFTW->legend->setVisible(true);
ui->FFTW->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);
ui->FFTW->legend->setBrush(QColor(255, 255, 255, 200));

legendPen.setColor(QColor(130, 130, 130, 200));
legendFont = font();
legendFont.setPointSize(10);
ui->FFTW->legend->setBorderPen(legendPen);
ui->FFTW->legend->setFont(legendFont);*/

//connect(&thrUdp, &QThread::finished, udpSocket, &QObject::deleteLater);
//udpSocket->moveToThread(&thrUdp);
//thrUdp.start();


//udpSocket->bind(QHostAddress("192.168.0.2"), 8080);
//udpSocket->bind(QHostAddress::Any, 48000);
//udpSocket->bind(QHostAddress("192.168.4.2"), 48000);
//udpSocket->bind(QHostAddress::Any, 48000, QUdpSocket::ShareAddress);
//udpSocket->bind( 48000, QUdpSocket::ShareAddress );
//udpSocket->bind(8080, QUdpSocket::ShareAddress);


//if (false == udpSocket->bind(QHostAddress::AnyIPv4, 48000, QUdpSocket::ShareAddress))

//    if (false == udpSocket->bind(QHostAddress::Broadcast, 48000, QUdpSocket::ShareAddress))
//    {
//        qDebug() << "Warn: cannot bind to the multicast port " << 48000;
//    }


/*
void MainWindow::mousePress(QMouseEvent* event)
{
    QCustomPlot *customPlot=ui->customPlot;
    static QCPItemLine *hCursor, *vCursor;
    double x=customPlot->xAxis->pixelToCoord(event->pos().x());
    double y=customPlot->yAxis->pixelToCoord(event->pos().y());

    if(hCursor) customPlot->removeItem(hCursor);
    hCursor = new QCPItemLine(customPlot);
    customPlot->addItem(hCursor);
    hCursor->start->setCoords(QCPRange::minRange, y);
    hCursor->end->setCoords(QCPRange::maxRange, y);

    if(vCursor) customPlot->removeItem(vCursor);
    vCursor = new QCPItemLine(customPlot);
    customPlot->addItem(vCursor);
    vCursor->start->setCoords( x, QCPRange::minRange);
    vCursor->end->setCoords( x, QCPRange::maxRange);

    customPlot->replot();
}*/


//void MainWindow::on_pushButton_clicked()
//{
//    //adc_data.resize(8192);
//    adc_cnt = 0;
//    packet_count=0;
//    cnt_oscillo = 0;

//    j=0;
//    time_cnt = 5;

//   /* len=1024;
//    mem_cnt=0;
//    j=0;*/

//    //udpSocket = new QUdpSocket(this);
//    //udpSocket->bind(QHostAddress("192.168.0.3"), 8080);
//    //udpSocket->bind( 8080, QUdpSocket::ShareAddress );
//    //connect(udpSocket, SIGNAL(readyRead()),this, SLOT(readPendingDatagrams()));


//    //QThread *t = new QThread();
//    //t->start();

//    //UdpReceiver * net = new UdpReceiver();
//    //net->moveToThread(t);
//    //connect( t, SIGNAL( updateLabel( QString ) ), SLOT( onUpdateLabel( QString ) ) );


//     //MyThread *m_thread = new MyThread();
//     //connect( m_thread, SIGNAL( updateLabel( QString ) ), SLOT( onUpdateLabel( QString ) ) );
//     //m_thread->initialize();
//     //m_thread->start();


//    //connect(udpSocket, SIGNAL(readyRead()),this, SLOT(readPendingDatagrams()));
//    /*connect(&this->timer_sec, SIGNAL(timeout()), this, SLOT(Update_speed()));
//    this->timer_sec.start(1000);*/ // 1sec

//   /* connect(&this->timer_data, SIGNAL(timeout()), this, SLOT(Data_received()));
//    connect(&this->timer_sec, SIGNAL(timeout()), this, SLOT(Update_speed()));
//    this->timer_sec.start(1000); // 1sec
//    this->timer_data.start(1);*/   // 1ms 1000 datagram*/
//}
//void MainWindow::sendToClient(QTcpSocket* pSocket, const QString& str)
//{
//    QByteArray  arrBlock;
//    QDataStream out(&arrBlock, QIODevice::WriteOnly);
//    out.setVersion(QDataStream::Qt_4_2);
//    out << quint16(0) << QTime::currentTime() << str;

//    out.device()->seek(0);
//    out << quint16(arrBlock.size() - sizeof(quint16));

//    pSocket->write(arrBlock);
//}

//*virtual*/ void MainWindow::slotNewConnection()
//{
//    QTcpSocket* pClientSocket = server->nextPendingConnection();
//    connect(pClientSocket, SIGNAL(readyRead()), this,          SLOT(slotReadClient()));
//    sendToClient(pClientSocket, "Server Response: Connected!");
//}

//void MainWindow::slotReadClient()
//{
//    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
//    //QDataStream in(pClientSocket);
//    //in.setVersion(QDataStream::Qt_4_2);

//    while(pClientSocket->bytesAvailable())
//    {
//        QTime   time;
//        QString str;
//        //in >> time >> str;

//       QByteArray data =  pClientSocket->readAll();
//       //QDataStream in(&data, QIODevice::ReadOnly);
//       //in >> str;

//       ui->textEdit->append(data);

//        //QString strMessage = time.toString() + " " + "Client has sended - " + str;

//        //ui->textEdit->append(strMessage);
//    }

//   //sendToClient(pClientSocket, "Server receive");

//   /* for (;;) {
//        if (!m_nNextBlockSize) {
//            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
//                break;
//            }
//            in >> m_nNextBlockSize;
//        }

//        if (pClientSocket->bytesAvailable() < m_nNextBlockSize) {
//            break;
//        }
//        QTime   time;
//        QString str;
//        in >> time >> str;

//        QString strMessage = time.toString() + " " + "Client has sended - " + str;

//        ui->textEdit->append(strMessage);
//        //m_ptxt->append(strMessage);
//        m_nNextBlockSize = 0;
//        sendToClient(pClientSocket, "Server Response: Received \"" + str + "\"");
//    }*/
//}


//void MainWindow::Data_received()
//{
//    if(data_ok)
//    {
//        ui->textEdit->append(data_tr.toHex());
//    }
//}

//void MainWindow::on_pushButton_2_clicked()
//{
//    unsigned char buffer[2];

//    buffer[0]=(char)0x72;
//    buffer[1]=(char)0X72;
//    //if(udpSocket->writeDatagram((const char*)buffer,sizeof(buffer),QHostAddress("192.168.2.2"), 48000)<0)
//    if(udpSocket->writeDatagram(datagram, QHostAddress::Broadcast, 48000)<0)
//    {
//        qDebug()<<"Error: Discovery: writeDatagram failed "<<udpSocket->errorString();
//        return;
//    }
//    udpSocket->flush();
//}

//void MainWindow::on_pushButton_3_clicked()
//{
//    unsigned char buffer[2];

//    buffer[0]='s';
//    buffer[1]='s';
//    if(udpSocket->writeDatagram((const char*)buffer,sizeof(buffer),QHostAddress("192.168.2.2"), 48000)<0)
//    {
//        qDebug()<<"Error: Discovery: writeDatagram failed "<<udpSocket->errorString();
//        return;
//    }
//    udpSocket->flush();
//}

//void MainWindow::on_pushButton_4_clicked()
//{
//    packet_count = 0;
//    //ui->label->setText("Packet number: " + QString::number(packet_count));

//}
//Запись
//void MainWindow::on_pushButton_5_clicked()
//{
//    j=0;
//    time_cnt = 1;
//    packet_count=0;
//    cnt_oscillo = 0;

//    len=1024;
//    mem_cnt=0;

//    /*ui->Oscillo1->graph(0)->clearData();
//    ui->Oscillo1->graph(1)->clearData();
//    ui->Oscillo1->graph(2)->clearData();
//    ui->Oscillo1->graph(3)->clearData();
//    ui->Oscillo1->graph(4)->clearData();
//    ui->Oscillo1->graph(5)->clearData();
//    ui->Oscillo1->graph(6)->clearData();
//    ui->Oscillo1->graph(7)->clearData();*/

//    ui->Oscillo1->graph(0)->data()->clear();
//    ui->Oscillo1->graph(1)->data()->clear();
//    ui->Oscillo1->graph(2)->data()->clear();
//    ui->Oscillo1->graph(3)->data()->clear();
//    ui->Oscillo1->graph(4)->data()->clear();
//    ui->Oscillo1->graph(5)->data()->clear();
//    ui->Oscillo1->graph(6)->data()->clear();
//    ui->Oscillo1->graph(7)->data()->clear();

//    //ui->Oscillo1->replot();
//    //ui->Oscillo1->clearGraphs();

//    //adc_data.resize(8192);
//    //adc_cnt = 0;
//    //packet_count=0;
//    //cnt_oscillo = 0;

//    //j=0;
//    //time_cnt = 1;

//  /*  udpSocket = new QUdpSocket(this);
//    udpSocket->bind(QHostAddress("192.168.0.3"), 8080);
//    //udpSocket->bind(QHostAddress::Any, 8080);
//    //udpSocket->bind( 8080, QUdpSocket::ShareAddress );
//    //udpSocket->bind(8080, QUdpSocket::ShareAddress);
//    connect(udpSocket, SIGNAL(readyRead()),this, SLOT(readPendingDatagrams()));
//    connect(&this->timer_sec, SIGNAL(timeout()), this, SLOT(Update_speed()));*/


//    //connect(&this->timer_sec, SIGNAL(timeout()), this, SLOT(Update_speed()));
//    //this->timer_sec.start(10); // 1sec

//    //std::complex<int16_t> adc_data;
//    //adc_data.imag();
//    //adc_data.real();


//    qDebug() <<  "on_pushButton_5_clicked\n";
//    connect(&this->timer_sec, SIGNAL(timeout()), this, SLOT(Update_speed()));
//    this->timer_sec.start(100); // 1sec



//    //Generate sine and cos

//   /* int i=0;
//    double x1=0;
//    double y1=0;

//    double x2=0;
//    double y2=0;

//    double pi = 3.14159265359;
//    double freq =10000;
//    double sps =5000000;

//    int16_t I, Q;
//    int32_t It, Qt;

//    float If, Qf;
//    float valf;
//    double valD;

//    fixed_point_t val16;

//    while(i <= 1000000)
//    {


//        //lsb = p[j+1];
//        //msb = p[j];
//        //val16 = ((msb<<8)) | (lsb&0x00FF);j+=4;


//        x1 = i;
//        x2 = i;

//        y1 = 5*cos(2*pi*(freq/sps)*i);
//        //y2 = 5*sin(2*pi*(freq/sps)*i);

//        //I = (int16_t)(5*cos(2*pi*(freq/sps)*i));
//        //Q = (int16_t)(5*sin(2*pi*(freq/sps)*i));

//        //It = int (5*cos(2*pi*(freq/sps)*i));
//        //Qt = int (5*sin(2*pi*(freq/sps)*i));


//        //If = (float) (5*cos(2*pi*(freq/sps)*i));
//        Qf = (float) (5*sin(2*pi*(freq/sps)*i));

//        //It = (int) If;
//        //If = (float) It;

//        valf = Qf;

//        //(X>=0)? (int) (X + 0.5) : (int)-(ABS(X) +0.5)


//        //It = Round(valf);
//        //valf = float(It);

//        val16 = float_to_fixed(y1);

//        ui->Oscillo1->graph(0)->addData(x1, val16);
//        ui->Oscillo1->graph(1)->addData(x2, Qf);

//        i++;
//    }
//    ui->Oscillo1->replot();
//    free (p);*/

//}
/*ui->textEdit->append(   QString(" 0x%1").arg(adc_ok[0],   2, 16, QChar('0'))   + QString(" 0x%1").arg(adc_ok[1], 2, 16, QChar('0')) );
for(int i=2; i < 1024; i+=16)
{
    ui->textEdit->append(   QString(" 0x%1").arg(adc_ok[i],   2, 16, QChar('0'))   +\
                            QString(" 0x%1").arg(adc_ok[i+1], 2, 16, QChar('0'))   +\
                            QString(" 0x%1").arg(adc_ok[i+2], 2, 16, QChar('0'))   +\
                            QString(" 0x%1").arg(adc_ok[i+3], 2, 16, QChar('0'))   +\
                            QString(" 0x%1").arg(adc_ok[i+4], 2, 16, QChar('0'))   +\
                            QString(" 0x%1").arg(adc_ok[i+5], 2, 16, QChar('0'))   +\
                            QString(" 0x%1").arg(adc_ok[i+6], 2, 16, QChar('0'))   +\
                            QString(" 0x%1").arg(adc_ok[i+7], 2, 16, QChar('0'))   +\
                            QString(" 0x%1").arg(adc_ok[i+8], 2, 16, QChar('0'))   +\
                            QString(" 0x%1").arg(adc_ok[i+9], 2, 16, QChar('0'))   +\
                            QString(" 0x%1").arg(adc_ok[i+10], 2, 16, QChar('0'))  +\
                            QString(" 0x%1").arg(adc_ok[i+11], 2, 16, QChar('0'))  +\
                            QString(" 0x%1").arg(adc_ok[i+12], 2, 16, QChar('0'))  +\
                            QString(" 0x%1").arg(adc_ok[i+13], 2, 16, QChar('0'))  +\
                            QString(" 0x%1").arg(adc_ok[i+14], 2, 16, QChar('0'))  +\
                            QString(" 0x%1").arg(adc_ok[i+15], 2, 16, QChar('0')));
}*/













//ui->label->setText(QString::number(ch1_zero));
//ui->label_2->setText(QString::number(ch2_zero));
//ui->label_3->setText(QString::number(degree));
//ui->label_4->setText(QString::number(ch2[ch2_zero+1]));
//ui->label_5->setText(QString::number(ch2[ch2_zero+2]));
//ui->Oscillo1->graph(0)->setData(time_MOM, Ua_MOM);
//ui->Oscillo1->graph(0)->rescaleValueAxis();
//void MainWindow::ErrorUDP(QAbstractSocket::SocketError e)
//{
//    qDebug() << "Error " << e;
//}













/*

    QString str;
    double adc_t=0.0;
    double x;
    double y;
    char val;

    short int val16, lsb, msb;
    //quint16 val16, lsb, msb;

    //int val32;

    qDebug() << "time_cnt: " << QString::number(time_cnt);
    qDebug() << "j: " << QString::number(j);
    time_cnt--;
    if(time_cnt == 0)
    {
        this->timer_sec.stop();
        udpSocket->close();
        //ui->textEdit->append(   QString(" 0x%1").arg(p[0],   2, 16, QChar('0'))   + QString(" 0x%1").arg(p[1], 2, 16, QChar('0')) );
        for(int i=2; i < 1024; i+=16)
        {
            ui->textEdit->append(   QString(" 0x%1").arg(p[i],   2, 16, QChar('0'))   +\
                                    QString(" 0x%1").arg(p[i+1], 2, 16, QChar('0'))   +\
                                    QString(" 0x%1").arg(p[i+2], 2, 16, QChar('0'))   +\
                                    QString(" 0x%1").arg(p[i+3], 2, 16, QChar('0'))   +\
                                    QString(" 0x%1").arg(p[i+4], 2, 16, QChar('0'))   +\
                                    QString(" 0x%1").arg(p[i+5], 2, 16, QChar('0'))   +\
                                    QString(" 0x%1").arg(p[i+6], 2, 16, QChar('0'))   +\
                                    QString(" 0x%1").arg(p[i+7], 2, 16, QChar('0'))   +\
                                    QString(" 0x%1").arg(p[i+8], 2, 16, QChar('0'))   +\
                                    QString(" 0x%1").arg(p[i+9], 2, 16, QChar('0'))   +\
                                    QString(" 0x%1").arg(p[i+10], 2, 16, QChar('0'))  +\
                                    QString(" 0x%1").arg(p[i+11], 2, 16, QChar('0'))  +\
                                    QString(" 0x%1").arg(p[i+12], 2, 16, QChar('0'))  +\
                                    QString(" 0x%1").arg(p[i+13], 2, 16, QChar('0'))  +\
                                    QString(" 0x%1").arg(p[i+14], 2, 16, QChar('0'))  +\
                                    QString(" 0x%1").arg(p[i+15], 2, 16, QChar('0')));
        }

        j=0;
        for(unsigned long int k=0;k<=10000;k+=2)    //10Mb
        {

            if(j==0)
            {
                k+=2; // пропустить 2 байта счетчика
            }

            memcpy(&val16,&p[k],2);

            if(j==1440) // последний пустой
            {
                j=0;
                k-=3; // пропустить 2 байта счетчика
            }



            adc_t+=0.000005; //5 us
            x = adc_t;
            y = val16;
            ui->Oscillo1->graph(0)->addData(x,y);
        }




        unsigned long int k;
        unsigned long int capture_bytes = j;
        j=0;

         if(j==0)
         {
             k+=2;
             j+=2;
         }*/
        //memcpy(&val16,&p[k],2); k+=4; //j+=4;

        /*if(j==1438)
        {
            memcpy(&val16,&p[k],2); k+=4; j=0;
        }




        //float ch1[200000], ch2[200000];


        adc_t=0;j=0;
        while(adc_t <= 16384)
        {
            //lsb = p[j];
            //msb = p[j+1];

            lsb = p[j+1];
            msb = p[j];
            val16 = ((msb<<8)) | (lsb&0x00FF);j+=4;

            x = adc_t+=1;
            y = val16;
            ui->Oscillo1->graph(0)->addData(x,y);
        }


        adc_t=0;j=2;
        while(adc_t <= 16384)
        {
            //lsb = p[j];
            //msb = p[j+1];

            lsb = p[j+1];
            msb = p[j];
            val16 = ((msb<<8)) | (lsb&0x00FF);j+=4;

            x = adc_t+=1;
            y = val16;
            ui->Oscillo1->graph(1)->addData(x,y);
        }

        //ui->label->setText(QString::number(ch1_zero));
        //ui->label_2->setText(QString::number(ch2_zero));
        //ui->label_3->setText(QString::number(degree));
        //ui->label_4->setText(QString::number(ch2[ch2_zero+1]));
        //ui->label_5->setText(QString::number(ch2[ch2_zero+2]));
        //ui->Oscillo1->graph(0)->setData(time_MOM, Ua_MOM);
        //ui->Oscillo1->graph(0)->rescaleValueAxis();
        ui->Oscillo1->replot();
        free (p);
    }
 */




// ui->label->setText(QString::number(packet_count++));

/* j=0;
 for(int i=0; i<1000; i++)
 {
     memcpy(&val,&datagram.data()[i],1);
     val_f = 15*qSin(val);
     time_MOM[i] = adc_t;
     Ua_MOM[i] = val_f;
     //adc_t+=0.00002;
     adc_t+=0.0002;
     //j+=2;

 }

 ui->Oscillo1->graph(0)->setData(time_MOM,Ua_MOM);
 //ui->Oscillo1->graph(0)->rescaleValueAxis();
 ui->Oscillo1->replot();*/



//while (udpSocket->hasPendingDatagrams());

//QString str;
//QDataStream in(&datagram, QIODevice::ReadOnly);
//in >> str;

//ui->textEdit->append(datagram.toHex());

//ui->label->setText(str);
//ui->textEdit->append(datagram.toHex().toUpper());
//ui->label->setText(QString::number(packet_count++));


//ui->label->setText("Packet number: " + QString::number(packet_count) + ", size: " + QString::number(datagram.size()) + " B");
     //ui->label_2->setText(QString::number( (packet_count * datagram.size() * 8)) + " Mbytes");


     //ui->label_2->setText(datagram.toHex());

     //short int val;

     //int j=2;
     /*for(int i=2;i<=1024;i+=4)
     {
         memcpy(&val,&datagram.data()[i],2);
         QString str=QString::number(val);
         ui->textEdit->append(str);
     }*/



    // memcpy(&adc_data.data()[adc_cnt],&datagram.data()[0],datagram.size());
     //adc_cnt+=datagram.size();






  /*   if(adc_cnt == 8192)
     {
         adc_cnt = 0;
         index=0;
         j=0;
         for(int i=0; i<4096; i++)
         {
             memcpy(&val_prev, &adc_data.data()[j],   2);
             memcpy(&val_new,  &adc_data.data()[j+2], 2);

             if( sign(val_prev) < sign(val_new) )
             {
                 index = j+2;
                 break;
             }
             j+=2;
         }*/

         //QString str=QString::number(index);
         //ui->textEdit->append(str);

         //j=index;
 /*        j=0;
         for(int i=0; i<1000; i++)
         {
             memcpy(&val,&adc_data.data()[j],2);
             //val_f = (val/32768.0)*5.0;
             val_f = val;
             time_MOM[i] = adc_t;
             Ua_MOM[i] = val_f;
             adc_t+=0.00002;
             j+=2;
         }

         ui->Oscillo1->graph(0)->setData(time_MOM,Ua_MOM);
         ui->Oscillo1->graph(0)->rescaleValueAxis();
         ui->Oscillo1->replot();*/
    // }





     //memcpy(&val,&datagram.data()[j],2);
     //val_f = (val/32768.0)*5.0;
     //QString str=QString::number(sign(val));
     //ui->textEdit->append(str);

     //ui->Oscillo1->graph(0)->setData(time_MOM,Ua_MOM);
     //ui->Oscillo1->replot();

     //if( (Ua_MOM[0] >= 0.0) && (Ua_MOM[0] <=0.1) )
     //{

     //cnt_oscillo++;

     //ui->Oscillo1->graph(0)->rescaleValueAxis();


     //if(cnt_oscillo>50)
     //{

         //cnt_oscillo = 0;
     //}

    // }


     //QString command(QString::fromRawData(datagram));


     //ui->textEdit->setText(datagram.toHex());
     //ui->textBrowser->append(buffer.data());

     //qDebug() << "Message from: " << sender.toString();
     //qDebug() << "Message port: " << senderPort;
     //qDebug() << "Message: " << datagram.toHex().toUpper();


     //qDebug() << "Packet number: " << QString::number(packet_count);

 //}
//ui->label->setText("Packet number: " + QString::number(packet_count) + ", size: " + QString::number(datagram.size()) + " B");




/* if( (signf(ch1[0]) > 0) || (signf(ch2[0]) > 0) ) // поиск с + на -
 {
      for(int i=0; i<200000; i++)
      {
          if( (signf(ch1[i]) > signf(ch1[i+1])) )
          {
              ch1_zero = i;
              break;
          }
      }

      for(int i=0; i<200000; i++)
      {
          if( (signf(ch2[i]) > signf(ch2[i+1])) )
          {
              ch2_zero = i;
              break;
          }
      }
 }
 else if( (signf(ch1[0]) < 0) || (signf(ch2[0]) < 0) ) // поиск с - на +
 {
     for(int i=0; i<200000; i++)
     {
         if( (signf(ch1[i]) < signf(ch1[i+1])) )
         {
             ch1_zero = i;
             break;
         }
     }

     for(int i=0; i<200000; i++)
     {
         if( (signf(ch2[i]) < signf(ch2[i+1])) )
         {
             ch2_zero = i;
             break;
         }
     }
 }*/



 /*for(int i=0; i<200000; i++)
 {
     if( (signf(ch1[i]) < signf(ch1[i+1])) )
     {
         ch1_zero = i;
         break;
     }
 }

 for(int i=0; i<200000; i++)
 {
     if( (signf(ch2[i]) < signf(ch2[i+1])) )
     {
         ch2_zero = i;
         break;
     }
 }*/


              /*k=5;adc_t=0;j=0;
              while(adc_t <= 3.0)
              {
                  adc_t+=0.000020; //5 us
                  x = adc_t;

                  lsb = p[k];
                  msb = p[k+1];
                  val16 = ((msb<<8)) | (lsb&0x00FF);k+=16;
                  y = val16*0.000152;
                  if(j<50000)
                      ch1[j++]= val16*0.000152;

                  ui->Oscillo1->graph(0)->addData(x,y);
              }

              k=7;adc_t=0;
              while(adc_t <= 3.0)
              {
                  adc_t+=0.000020; //5 us
                  x = adc_t;

                  lsb = p[k];
                  msb = p[k+1];
                  val16 = ((msb<<8)) | (lsb&0x00FF);k+=16;
                  y = val16*0.000152;
                  ui->Oscillo1->graph(1)->addData(x,y);
              }

              k=9;adc_t=0;
              while(adc_t <= 3.0)
              {
                  adc_t+=0.000020; //5 us
                  x = adc_t;

                  lsb = p[k];
                  msb = p[k+1];
                  val16 = ((msb<<8)) | (lsb&0x00FF);k+=16;
                  y = val16*0.000152;
                  ui->Oscillo1->graph(2)->addData(x,y);
              }

              k=11;adc_t=0;
              while(adc_t <= 3.0)
              {
                  adc_t+=0.000020; //5 us
                  x = adc_t;

                  lsb = p[k];
                  msb = p[k+1];
                  val16 = ((msb<<8)) | (lsb&0x00FF);k+=16;
                  y = val16*0.000152;
                  ui->Oscillo1->graph(3)->addData(x,y);
              }

              k=13;adc_t=0;
              while(adc_t <= 3.0)
              {
                  adc_t+=0.000020; //5 us
                  x = adc_t;

                  lsb = p[k];
                  msb = p[k+1];
                  val16 = ((msb<<8)) | (lsb&0x00FF);k+=16;
                  y = val16*0.000152;
                  ui->Oscillo1->graph(4)->addData(x,y);
              }

              k=15;adc_t=0;j=0;
              while(adc_t <= 3.0)
              {
                  adc_t+=0.000020; //5 us
                  x = adc_t;

                  lsb = p[k];
                  msb = p[k+1];
                  val16 = ((msb<<8)) | (lsb&0x00FF);k+=16;
                  y = val16*0.000152;

                  if(j<50000)
                      ch6[j++]= val16*0.000152;

                  ui->Oscillo1->graph(5)->addData(x,y);
              }

              k=17;adc_t=0;
              while(adc_t <= 3.0)
              {
                  adc_t+=0.000020; //5 us
                  x = adc_t;

                  lsb = p[k];
                  msb = p[k+1];
                  val16 = ((msb<<8)) | (lsb&0x00FF);k+=16;
                  y = val16*0.000152;
                  ui->Oscillo1->graph(6)->addData(x,y);
              }

              k=19;adc_t=0;
              while(adc_t <= 3.0)
              {
                  adc_t+=0.000020; //5 us
                  x = adc_t;

                  lsb = p[k];
                  msb = p[k+1];
                  val16 = ((msb<<8)) | (lsb&0x00FF);k+=16;
                  y = val16*0.000152;
                  ui->Oscillo1->graph(7)->addData(x,y);
              }*/


      /*
              k=0;adc_t=0;j=0;
              while(adc_t <= 3.0)
              {
                  adc_t+=0.000005; //5 us
                  x = adc_t;

                  lsb = p[k];
                  msb = p[k+1];
                  val16 = ((msb<<8)) | (lsb&0x00FF);k+=4;
                  y = val16*0.000152;
                  if(j<200000)
                      ch1[j++]= val16*0.000152;

                  ui->Oscillo1->graph(0)->addData(x,y);
              }

              k=2;adc_t=0;j=0;
              while(adc_t <= 3.0)
              {
                  adc_t+=0.000005; //5 us
                  x = adc_t;

                  lsb = p[k];
                  msb = p[k+1];
                  val16 = ((msb<<8)) | (lsb&0x00FF);k+=4;
                  y = val16*0.000152;
                  if(j<200000)
                      ch2[j++]= val16*0.000152;

                  ui->Oscillo1->graph(1)->addData(x,y);
              }*/




              //double fk;
              //double pi = 3.14159265359;
             // float v1[200000], v2[200000];

             /* adc_t=0;
              fk=2*pi*(50.0/200000.0);
              for(int i=0;i<200000;i++)
              {
                  adc_t+=0.000005; //5 us
                  x = adc_t;
                  y=3*sin(i*fk-((pi/180.0)*0.0));
                  v1[i] = (float)y;
                  //ui->Oscillo1->graph(0)->addData(x,y);
              }

              adc_t=0;
              fk=2*pi*(50.0/200000.0);
              for(int i=0;i<200000;i++)
              {
                  adc_t+=0.000005; //5 us
                  x = adc_t;
                  y=3*sin(i*fk+((pi/180.0)*39.0));
                  v2[i]=(float)y;
                  //ui->Oscillo1->graph(1)->addData(x,y);
              }


              int ch1_zero, ch2_zero;
              float degree;

              if( (signf(v1[0]) > 0) || (signf(v2[0]) > 0) ) // поиск с + на -
              {
                   for(int i=0; i<200000; i++)
                   {
                       if( (signf(v1[i]) > signf(v1[i+1])) )
                       {
                           ch1_zero = i;
                           break;
                       }
                   }

                   for(int i=0; i<200000; i++)
                   {
                       if( (signf(v2[i]) > signf(v2[i+1])) )
                       {
                           ch2_zero = i;
                           break;
                       }
                   }
              }
              else if( (signf(v1[0]) < 0) || (signf(v2[0]) < 0) ) // поиск с - на +
              {
                  for(int i=0; i<200000; i++)
                  {
                      if( (signf(v1[i]) < signf(v1[i+1])) )
                      {
                          ch1_zero = i;
                          break;
                      }
                  }

                  for(int i=0; i<200000; i++)
                  {
                      if( (signf(v2[i]) < signf(v2[i+1])) )
                      {
                          ch2_zero = i;
                          break;
                      }
                  }
              }




              //Определение периода и частоты сигнала
              int p1,p2;
              float fn, tn;

              for(int i=0; i<200000; i++)
              {
                  if( (signf(v1[i]) < signf(v1[i+1])) )
                  {
                      p1 = i+1;
                      break;
                  }
              }

              for(int i=p1; i<200000; i++)
              {
                  if( (signf(v1[i]) < signf(v1[i+1])) )
                  {
                      p2 = i+1;
                      break;
                  }
              }

              tn = (p2-p1)*0.000005;
              fn = 1/tn;
              ui->label_8->setText(QString::number(fn));
              ui->label_9->setText(QString::number(tn));


              degree = qRound(((360.0*50.0)/200000.0)*(abs(ch1_zero-ch2_zero)));



    unsigned char buffer[2];

    buffer[0]=(char)0x72;
    buffer[1]=(char)0X71;
    if(udpSocket->writeDatagram((const char*)buffer,sizeof(buffer),QHostAddress("192.168.0.2"), 8080)<0)
    {
        qDebug()<<"Error: Discovery: writeDatagram failed "<<udpSocket->errorString();
        return;
    }
    udpSocket->flush();*/



    /*p = (unsigned char *) calloc(800000000, sizeof(unsigned char)); //800 Mb
    if(!p)
    {
        qDebug()<<"Allocation memory failure";
        exit (1);
    }
    else
        qDebug()<<"Allocation memory ok!";*/

    //udpSocket = new QUdpSocket(this);
    //udpSocket->bind(QHostAddress("127.0.0.1"), 5000);
   // udpSocket->bind(QHostAddress("192.168.2.196"), 1024);
    //udpSocket->bind(QHostAddress("192.168.2.1"), 5000);

    //udpSocket = new QUdpSocket(this);
    //udpSocket->bind(QHostAddress("192.168.2.196"), 48247);

    //udpSocket = new QUdpSocket(this);
    //udpSocket->bind(QHostAddress("192.168.2.100"), 48247);









//**************************************************************************************************/
//Scope
//              auto scope_ch1 = new QCustomPlot(this);
//              auto scope_ch2 = new QCustomPlot(this);
//              auto scope_ch3 = new QCustomPlot(this);
//              auto scope_ch4 = new QCustomPlot(this);
//              auto scope_ch5 = new QCustomPlot(this);
//              auto scope_ch6 = new QCustomPlot(this);
//              auto scope_ch7 = new QCustomPlot(this);
//              auto scope_ch8 = new QCustomPlot(this);

//              scope_ch1->setObjectName("ch"+QString::number(j+1));
//              scope_ch1->addGraph();
//              scope_ch1->xAxis->setRange(0,2047);
//              scope_ch1->yAxis->setRange(-6,6);
//              scope_ch1->setMinimumHeight(ZoomVal);
//              scope_ch1->graph(0)->setName("Канал "+QString::number(j+1));
//              scope_ch1->legend->setVisible(true);
//              scope_ch1->legend->setFont(QFont(QFont().family(), 8));
//              scope_ch1->legend->setBrush(QColor(255, 255, 255, 150));
//              scope_ch1->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              scope_ch1->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              scope_ch1->setBackground(QBrush(Qt::black));
//              scope_ch1->graph(0)->setPen(QPen(Qt::green));
//              scope_ch1->xAxis->setBasePen(QPen(Qt::white, 1));
//              scope_ch1->yAxis->setBasePen(QPen(Qt::white, 1));
//              scope_ch1->xAxis->setTickPen(QPen(Qt::white, 1));
//              scope_ch1->yAxis->setTickPen(QPen(Qt::white, 1));
//              scope_ch1->xAxis->setSubTickPen(QPen(Qt::white, 1));
//              scope_ch1->yAxis->setSubTickPen(QPen(Qt::white, 1));
//              scope_ch1->xAxis->setTickLabelColor(Qt::white);
//              scope_ch1->yAxis->setTickLabelColor(Qt::white);
//              scope_ch1->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              scope_ch1->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              scope_ch1->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              scope_ch1->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              scope_ch1->xAxis->grid()->setSubGridVisible(true);
//              scope_ch1->yAxis->grid()->setSubGridVisible(false);
//              scope_ch1->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//              scope_ch1->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//              scope_ch1->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//              scope_ch1->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//              scope_ch2->setObjectName("ch"+QString::number(j+2));
//              scope_ch2->addGraph();
//              scope_ch2->xAxis->setRange(0,2047);
//              scope_ch2->yAxis->setRange(-6,6);
//              scope_ch2->setMinimumHeight(ZoomVal);
//              scope_ch2->graph(0)->setName("Канал "+QString::number(j+2));
//              scope_ch2->legend->setVisible(true);
//              scope_ch2->legend->setFont(QFont(QFont().family(), 8));
//              scope_ch2->legend->setBrush(QColor(255, 255, 255, 150));
//              scope_ch2->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              scope_ch2->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              scope_ch2->setBackground(QBrush(Qt::black));
//              scope_ch2->graph(0)->setPen(QPen(Qt::green));
//              scope_ch2->xAxis->setBasePen(QPen(Qt::white, 1));
//              scope_ch2->yAxis->setBasePen(QPen(Qt::white, 1));
//              scope_ch2->xAxis->setTickPen(QPen(Qt::white, 1));
//              scope_ch2->yAxis->setTickPen(QPen(Qt::white, 1));
//              scope_ch2->xAxis->setSubTickPen(QPen(Qt::white, 1));
//              scope_ch2->yAxis->setSubTickPen(QPen(Qt::white, 1));
//              scope_ch2->xAxis->setTickLabelColor(Qt::white);
//              scope_ch2->yAxis->setTickLabelColor(Qt::white);
//              scope_ch2->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              scope_ch2->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              scope_ch2->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              scope_ch2->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              scope_ch2->xAxis->grid()->setSubGridVisible(true);
//              scope_ch2->yAxis->grid()->setSubGridVisible(false);
//              scope_ch2->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//              scope_ch2->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//              scope_ch2->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//              scope_ch2->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//              scope_ch3->setObjectName("ch"+QString::number(j+3));
//              scope_ch3->addGraph();
//              scope_ch3->xAxis->setRange(0,2047);
//              scope_ch3->yAxis->setRange(-6,6);
//              scope_ch3->setMinimumHeight(ZoomVal);
//              scope_ch3->graph(0)->setName("Канал "+QString::number(j+3));
//              scope_ch3->legend->setVisible(true);
//              scope_ch3->legend->setFont(QFont(QFont().family(), 8));
//              scope_ch3->legend->setBrush(QColor(255, 255, 255, 150));
//              scope_ch3->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              scope_ch3->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              //scope_ch3->xAxis->setLabel("Напряжение");
//              //scope_ch3->yAxis->setLabel("Напряжение");
//              //scope_ch3->xAxis->setLabelColor(Qt::white);
//              //scope_ch3->yAxis->setLabelColor(Qt::white);
//              //scope_ch3->xAxis->setLabelFont(QFont(QFont().family(), 8));
//              //scope_ch3->yAxis->setLabelFont(QFont(QFont().family(), 8));
//              scope_ch3->setBackground(QBrush(Qt::black));
//              scope_ch3->graph(0)->setPen(QPen(Qt::green));
//              scope_ch3->xAxis->setBasePen(QPen(Qt::white, 1));
//              scope_ch3->yAxis->setBasePen(QPen(Qt::white, 1));
//              scope_ch3->xAxis->setTickPen(QPen(Qt::white, 1));
//              scope_ch3->yAxis->setTickPen(QPen(Qt::white, 1));
//              scope_ch3->xAxis->setSubTickPen(QPen(Qt::white, 1));
//              scope_ch3->yAxis->setSubTickPen(QPen(Qt::white, 1));
//              scope_ch3->xAxis->setTickLabelColor(Qt::white);
//              scope_ch3->yAxis->setTickLabelColor(Qt::white);
//              scope_ch3->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              scope_ch3->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              scope_ch3->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              scope_ch3->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              scope_ch3->xAxis->grid()->setSubGridVisible(true);
//              scope_ch3->yAxis->grid()->setSubGridVisible(false);
//              scope_ch3->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//              scope_ch3->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//              scope_ch3->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//              scope_ch3->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//              scope_ch4->setObjectName("ch"+QString::number(j+4));
//              scope_ch4->addGraph();
//              scope_ch4->xAxis->setRange(0,2047);
//              scope_ch4->yAxis->setRange(-6,6);
//              scope_ch4->setMinimumHeight(ZoomVal);
//              scope_ch4->graph(0)->setName("Канал "+QString::number(j+4));
//              scope_ch4->legend->setVisible(true);
//              scope_ch4->legend->setFont(QFont(QFont().family(), 8));
//              scope_ch4->legend->setBrush(QColor(255, 255, 255, 150));
//              scope_ch4->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              scope_ch4->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              scope_ch4->setBackground(QBrush(Qt::black));
//              scope_ch4->graph(0)->setPen(QPen(Qt::green));
//              scope_ch4->xAxis->setBasePen(QPen(Qt::white, 1));
//              scope_ch4->yAxis->setBasePen(QPen(Qt::white, 1));
//              scope_ch4->xAxis->setTickPen(QPen(Qt::white, 1));
//              scope_ch4->yAxis->setTickPen(QPen(Qt::white, 1));
//              scope_ch4->xAxis->setSubTickPen(QPen(Qt::white, 1));
//              scope_ch4->yAxis->setSubTickPen(QPen(Qt::white, 1));
//              scope_ch4->xAxis->setTickLabelColor(Qt::white);
//              scope_ch4->yAxis->setTickLabelColor(Qt::white);
//              scope_ch4->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              scope_ch4->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              scope_ch4->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              scope_ch4->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              scope_ch4->xAxis->grid()->setSubGridVisible(true);
//              scope_ch4->yAxis->grid()->setSubGridVisible(false);
//              scope_ch4->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//              scope_ch4->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//              scope_ch4->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//              scope_ch4->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//              scope_ch5->setObjectName("ch"+QString::number(j+5));
//              scope_ch5->addGraph();
//              scope_ch5->xAxis->setRange(0,2047);
//              scope_ch5->yAxis->setRange(-6,6);
//              scope_ch5->setMinimumHeight(ZoomVal);
//              scope_ch5->graph(0)->setName("Канал "+QString::number(j+5));
//              scope_ch5->legend->setVisible(true);
//              scope_ch5->legend->setFont(QFont(QFont().family(), 8));
//              scope_ch5->legend->setBrush(QColor(255, 255, 255, 150));
//              scope_ch5->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              scope_ch5->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              scope_ch5->setBackground(QBrush(Qt::black));
//              scope_ch5->graph(0)->setPen(QPen(Qt::green));
//              scope_ch5->xAxis->setBasePen(QPen(Qt::white, 1));
//              scope_ch5->yAxis->setBasePen(QPen(Qt::white, 1));
//              scope_ch5->xAxis->setTickPen(QPen(Qt::white, 1));
//              scope_ch5->yAxis->setTickPen(QPen(Qt::white, 1));
//              scope_ch5->xAxis->setSubTickPen(QPen(Qt::white, 1));
//              scope_ch5->yAxis->setSubTickPen(QPen(Qt::white, 1));
//              scope_ch5->xAxis->setTickLabelColor(Qt::white);
//              scope_ch5->yAxis->setTickLabelColor(Qt::white);
//              scope_ch5->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              scope_ch5->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              scope_ch5->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              scope_ch5->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              scope_ch5->xAxis->grid()->setSubGridVisible(true);
//              scope_ch5->yAxis->grid()->setSubGridVisible(false);
//              scope_ch5->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//              scope_ch5->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//              scope_ch5->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//              scope_ch5->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//              scope_ch6->setObjectName("ch"+QString::number(j+6));
//              scope_ch6->addGraph();
//              scope_ch6->xAxis->setRange(0,2047);
//              scope_ch6->yAxis->setRange(-6,6);
//              scope_ch6->setMinimumHeight(ZoomVal);
//              scope_ch6->graph(0)->setName("Канал "+QString::number(j+6));
//              scope_ch6->legend->setVisible(true);
//              scope_ch6->legend->setFont(QFont(QFont().family(), 8));
//              scope_ch6->legend->setBrush(QColor(255, 255, 255, 150));
//              scope_ch6->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              scope_ch6->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              scope_ch6->setBackground(QBrush(Qt::black));
//              scope_ch6->graph(0)->setPen(QPen(Qt::green));
//              scope_ch6->xAxis->setBasePen(QPen(Qt::white, 1));
//              scope_ch6->yAxis->setBasePen(QPen(Qt::white, 1));
//              scope_ch6->xAxis->setTickPen(QPen(Qt::white, 1));
//              scope_ch6->yAxis->setTickPen(QPen(Qt::white, 1));
//              scope_ch6->xAxis->setSubTickPen(QPen(Qt::white, 1));
//              scope_ch6->yAxis->setSubTickPen(QPen(Qt::white, 1));
//              scope_ch6->xAxis->setTickLabelColor(Qt::white);
//              scope_ch6->yAxis->setTickLabelColor(Qt::white);
//              scope_ch6->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              scope_ch6->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              scope_ch6->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              scope_ch6->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              scope_ch6->xAxis->grid()->setSubGridVisible(true);
//              scope_ch6->yAxis->grid()->setSubGridVisible(false);
//              scope_ch6->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//              scope_ch6->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//              scope_ch6->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//              scope_ch6->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//              scope_ch7->setObjectName("ch"+QString::number(j+7));
//              scope_ch7->addGraph();
//              scope_ch7->xAxis->setRange(0,2047);
//              scope_ch7->yAxis->setRange(-6,6);
//              scope_ch7->setMinimumHeight(ZoomVal);
//              scope_ch7->graph(0)->setName("Канал "+QString::number(j+7));
//              scope_ch7->legend->setVisible(true);
//              scope_ch7->legend->setFont(QFont(QFont().family(), 8));
//              scope_ch7->legend->setBrush(QColor(255, 255, 255, 150));
//              scope_ch7->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              scope_ch7->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              scope_ch7->setBackground(QBrush(Qt::black));
//              scope_ch7->graph(0)->setPen(QPen(Qt::green));
//              scope_ch7->xAxis->setBasePen(QPen(Qt::white, 1));
//              scope_ch7->yAxis->setBasePen(QPen(Qt::white, 1));
//              scope_ch7->xAxis->setTickPen(QPen(Qt::white, 1));
//              scope_ch7->yAxis->setTickPen(QPen(Qt::white, 1));
//              scope_ch7->xAxis->setSubTickPen(QPen(Qt::white, 1));
//              scope_ch7->yAxis->setSubTickPen(QPen(Qt::white, 1));
//              scope_ch7->xAxis->setTickLabelColor(Qt::white);
//              scope_ch7->yAxis->setTickLabelColor(Qt::white);
//              scope_ch7->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              scope_ch7->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              scope_ch7->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              scope_ch7->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              scope_ch7->xAxis->grid()->setSubGridVisible(true);
//              scope_ch7->yAxis->grid()->setSubGridVisible(false);
//              scope_ch7->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//              scope_ch7->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//              scope_ch7->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//              scope_ch7->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//              scope_ch8->setObjectName("ch"+QString::number(j+8));
//              scope_ch8->addGraph();
//              scope_ch8->xAxis->setRange(0,2047);
//              scope_ch8->yAxis->setRange(-6,6);
//              scope_ch8->setMinimumHeight(ZoomVal);
//              scope_ch8->graph(0)->setName("Канал "+QString::number(j+8));
//              scope_ch8->legend->setVisible(true);
//              scope_ch8->legend->setFont(QFont(QFont().family(), 8));
//              scope_ch8->legend->setBrush(QColor(255, 255, 255, 150));
//              scope_ch8->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              scope_ch8->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              scope_ch8->setBackground(QBrush(Qt::black));
//              scope_ch8->graph(0)->setPen(QPen(Qt::green));
//              scope_ch8->xAxis->setBasePen(QPen(Qt::white, 1));
//              scope_ch8->yAxis->setBasePen(QPen(Qt::white, 1));
//              scope_ch8->xAxis->setTickPen(QPen(Qt::white, 1));
//              scope_ch8->yAxis->setTickPen(QPen(Qt::white, 1));
//              scope_ch8->xAxis->setSubTickPen(QPen(Qt::white, 1));
//              scope_ch8->yAxis->setSubTickPen(QPen(Qt::white, 1));
//              scope_ch8->xAxis->setTickLabelColor(Qt::white);
//              scope_ch8->yAxis->setTickLabelColor(Qt::white);
//              scope_ch8->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              scope_ch8->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              scope_ch8->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              scope_ch8->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              scope_ch8->xAxis->grid()->setSubGridVisible(true);
//              scope_ch8->yAxis->grid()->setSubGridVisible(false);
//              scope_ch8->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//              scope_ch8->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//              scope_ch8->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//              scope_ch8->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//**************************************************************************************************/
//Harmonics
//              auto fft_ch1 = new QCustomPlot(this);
//              auto fft_ch2 = new QCustomPlot(this);
//              auto fft_ch3 = new QCustomPlot(this);
//              auto fft_ch4 = new QCustomPlot(this);
//              auto fft_ch5 = new QCustomPlot(this);
//              auto fft_ch6 = new QCustomPlot(this);
//              auto fft_ch7 = new QCustomPlot(this);
//              auto fft_ch8 = new QCustomPlot(this);


//              fft_ch1->setObjectName("fft"+QString::number(j+1));
//              fft_ch1->addGraph();
//              fft_ch1->xAxis->setRange(0,12500);
//              fft_ch1->yAxis->setRange(-150,10);
//              fft_ch1->graph(0)->setName("Канал "+QString::number(j+1));
//              fft_ch1->legend->setVisible(true);
//              fft_ch1->legend->setFont(QFont(QFont().family(), 8));
//              fft_ch1->legend->setBrush(QColor(255, 255, 255, 150));
//              fft_ch1->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              fft_ch1->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              fft_ch1->setBackground(QBrush(Qt::black));
//              fft_ch1->graph(0)->setPen(QPen(Qt::green));
//              fft_ch1->xAxis->setBasePen(QPen(Qt::white, 1));
//              fft_ch1->yAxis->setBasePen(QPen(Qt::white, 1));
//              fft_ch1->xAxis->setTickPen(QPen(Qt::white, 1));
//              fft_ch1->yAxis->setTickPen(QPen(Qt::white, 1));
//              fft_ch1->xAxis->setSubTickPen(QPen(Qt::white, 1));
//              fft_ch1->yAxis->setSubTickPen(QPen(Qt::white, 1));
//              fft_ch1->xAxis->setTickLabelColor(Qt::white);
//              fft_ch1->yAxis->setTickLabelColor(Qt::white);
//              fft_ch1->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              fft_ch1->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              fft_ch1->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              fft_ch1->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              fft_ch1->xAxis->grid()->setSubGridVisible(true);
//              fft_ch1->yAxis->grid()->setSubGridVisible(true);
//              fft_ch1->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//              fft_ch1->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//              fft_ch1->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//              fft_ch1->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
////              fft_ch1->xAxis->setLabel("Частота");
////              fft_ch1->yAxis->setLabel("дБ");
////              fft_ch1->xAxis->setLabelColor(Qt::white);
////              fft_ch1->yAxis->setLabelColor(Qt::white);
////              fft_ch1->xAxis->setLabelFont(QFont(QFont().family(), 8));
////              fft_ch1->yAxis->setLabelFont(QFont(QFont().family(), 8));
//              //fft_ch1->graph(0)->setPen(QPen(Qt::green));
//              //fft_ch1->graph(0)->setBrush(QBrush(QColor(0, 255, 255, 80)));
//              //fft_ch1->axisRect()->setupFullAxesBox(true);
//              //fft_ch1->yAxis->setScaleType(QCPAxis::stLogarithmic);
//              //fft_ch1->yAxis2->setScaleType(QCPAxis::stLogarithmic);
//              //QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
//              //fft_ch1->yAxis->setTicker(logTicker);
//              //fft_ch1->yAxis2->setTicker(logTicker);
//              //fft_ch1->yAxis->setNumberFormat("b"); // e = exponential, b = beautiful decimal powers
//              //fft_ch1->yAxis2->setRangeReversed ( true );
//              //fft_ch1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);


//              fft_ch2->setObjectName("fft"+QString::number(j+2));
//              fft_ch2->addGraph();
//              fft_ch2->xAxis->setRange(0,12500);
//              fft_ch2->yAxis->setRange(-150,10);
//              fft_ch2->graph(0)->setName("Канал "+QString::number(j+2));
//              fft_ch2->legend->setVisible(true);
//              fft_ch2->legend->setFont(QFont(QFont().family(), 8));
//              fft_ch2->legend->setBrush(QColor(255, 255, 255, 150));
//              fft_ch2->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              fft_ch2->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              fft_ch2->setBackground(QBrush(Qt::black));
//              fft_ch2->graph(0)->setPen(QPen(Qt::green));
//              fft_ch2->xAxis->setBasePen(QPen(Qt::white, 1));
//              fft_ch2->yAxis->setBasePen(QPen(Qt::white, 1));
//              fft_ch2->xAxis->setTickPen(QPen(Qt::white, 1));
//              fft_ch2->yAxis->setTickPen(QPen(Qt::white, 1));
//              fft_ch2->xAxis->setSubTickPen(QPen(Qt::white, 1));
//              fft_ch2->yAxis->setSubTickPen(QPen(Qt::white, 1));
//              fft_ch2->xAxis->setTickLabelColor(Qt::white);
//              fft_ch2->yAxis->setTickLabelColor(Qt::white);
//              fft_ch2->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              fft_ch2->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              fft_ch2->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              fft_ch2->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              fft_ch2->xAxis->grid()->setSubGridVisible(true);
//              fft_ch2->yAxis->grid()->setSubGridVisible(true);
//              fft_ch2->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//              fft_ch2->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//              fft_ch2->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//              fft_ch2->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);



////              fft_ch3->setNoAntialiasingOnDrag(true); // more performance/responsiveness during dragging
////              fft_ch3->setObjectName("fft"+QString::number(j+3));
////              fft_ch3->addGraph();
////              fft_ch3->graph(0)->setPen(QPen(Qt::green));
////              fft_ch3->graph(0)->setBrush(QBrush(QColor(0, 255, 255, 80)));
////              fft_ch3->axisRect()->setupFullAxesBox(true);
////              fft_ch3->xAxis->setRange(0,12500);
////              fft_ch3->yAxis->setRange(-150,10);
////              fft_ch3->graph(0)->setName("Канал "+QString::number(j+3));
////              fft_ch3->legend->setVisible(true);
////              fft_ch3->legend->setFont(QFont(QFont().family(), 8));
////              fft_ch3->legend->setBrush(QColor(255, 255, 255, 150));
////              fft_ch3->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
////              fft_ch3->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
////              fft_ch3->setBackground(QBrush(Qt::black));
////              //fft_ch3->graph(0)->setPen(QPen(Qt::green));
////              fft_ch3->xAxis->setBasePen(QPen(Qt::white, 1));
////              fft_ch3->yAxis->setBasePen(QPen(Qt::white, 1));
////              fft_ch3->xAxis->setTickPen(QPen(Qt::white, 1));
////              fft_ch3->yAxis->setTickPen(QPen(Qt::white, 1));
////              fft_ch3->xAxis->setSubTickPen(QPen(Qt::white, 1));
////              fft_ch3->yAxis->setSubTickPen(QPen(Qt::white, 1));
////              fft_ch3->xAxis->setTickLabelColor(Qt::white);
////              fft_ch3->yAxis->setTickLabelColor(Qt::white);
////              fft_ch3->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
////              fft_ch3->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
////              fft_ch3->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
////              fft_ch3->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
////              fft_ch3->xAxis->grid()->setSubGridVisible(true);
////              fft_ch3->yAxis->grid()->setSubGridVisible(true);
////              fft_ch3->xAxis->grid()->setZeroLinePen(Qt::NoPen);
////              fft_ch3->yAxis->grid()->setZeroLinePen(Qt::NoPen);
////              fft_ch3->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
////              fft_ch3->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);


//              fft_ch3->setObjectName("fft"+QString::number(j+3));
//              fft_ch3->addGraph();
//              fft_ch3->xAxis->setRange(0,12500);
//              fft_ch3->yAxis->setRange(-150,10);
//              fft_ch3->graph(0)->setName("Канал "+QString::number(j+3));
//              fft_ch3->legend->setVisible(true);
//              fft_ch3->legend->setFont(QFont(QFont().family(), 8));
//              fft_ch3->legend->setBrush(QColor(255, 255, 255, 150));
//              fft_ch3->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              fft_ch3->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              fft_ch3->setBackground(QBrush(Qt::black));
//              fft_ch3->graph(0)->setPen(QPen(Qt::green));
//              fft_ch3->xAxis->setBasePen(QPen(Qt::white, 1));
//              fft_ch3->yAxis->setBasePen(QPen(Qt::white, 1));
//              fft_ch3->xAxis->setTickPen(QPen(Qt::white, 1));
//              fft_ch3->yAxis->setTickPen(QPen(Qt::white, 1));
//              fft_ch3->xAxis->setSubTickPen(QPen(Qt::white, 1));
//              fft_ch3->yAxis->setSubTickPen(QPen(Qt::white, 1));
//              fft_ch3->xAxis->setTickLabelColor(Qt::white);
//              fft_ch3->yAxis->setTickLabelColor(Qt::white);
//              fft_ch3->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              fft_ch3->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              fft_ch3->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              fft_ch3->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              fft_ch3->xAxis->grid()->setSubGridVisible(true);
//              fft_ch3->yAxis->grid()->setSubGridVisible(true);
//              fft_ch3->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//              fft_ch3->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//              fft_ch3->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//              fft_ch3->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);




////                QCPGraph *fft3_graph1 = fft_ch3->addGraph();
////                fft3_graph1->setPen(QPen(QColor(120, 255, 120), 1));
////                fft_ch3->xAxis->setRange(0,12500);
////                fft_ch3->yAxis->setRange(-150,10);
////                fft_ch3->graph(0)->setName("Канал "+QString::number(j+3));

//                //fft3_graph1->setData(x, y);
//                //graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(Qt::white), 9));


////                fft_ch3->legend->setVisible(true);
////                fft_ch3->legend->setFont(QFont(QFont().family(), 8));
////                fft_ch3->legend->setBrush(QColor(255, 255, 255, 150));
////                fft_ch3->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
////                fft_ch3->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
////                fft_ch3->setBackground(QBrush(Qt::black));
////                fft_ch3->graph(0)->setPen(QPen(Qt::green));
////                fft_ch3->xAxis->setBasePen(QPen(Qt::white, 1));
////                fft_ch3->yAxis->setBasePen(QPen(Qt::white, 1));
////                fft_ch3->xAxis->setTickPen(QPen(Qt::white, 1));
////                fft_ch3->yAxis->setTickPen(QPen(Qt::white, 1));
////                fft_ch3->xAxis->setSubTickPen(QPen(Qt::white, 1));
////                fft_ch3->yAxis->setSubTickPen(QPen(Qt::white, 1));
////                fft_ch3->xAxis->setTickLabelColor(Qt::white);
////                fft_ch3->yAxis->setTickLabelColor(Qt::white);
////                fft_ch3->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
////                fft_ch3->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
////                fft_ch3->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
////                fft_ch3->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
////                fft_ch3->xAxis->grid()->setSubGridVisible(true);
////                fft_ch3->yAxis->grid()->setSubGridVisible(true);
////                fft_ch3->xAxis->grid()->setZeroLinePen(Qt::NoPen);
////                fft_ch3->yAxis->grid()->setZeroLinePen(Qt::NoPen);
////                fft_ch3->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
////                fft_ch3->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

////                QCPGraph *fft3_graph2 = fft_ch3->addGraph();
////                //graph2->setData(x, y2);
////                fft3_graph2->setPen(Qt::NoPen);
////                fft3_graph2->setBrush(QColor(0, 255, 0, 54));
////                fft3_graph2->setChannelFillGraph(fft3_graph1);
////                fft_ch3->legend->removeItem(1);






////              QVector<double> x(2048), y(2048);
////              for(int i=0; i<2048; ++i){
////                  x[i] = i;
////                  y[i] = exp(-i/150.0)*cos(i/10.0); // exponentially decaying cosiney1[i] = exp(-i/150.0); // exponential envelope
////              }


////              QCPGraph *newCurve = new QCPGraph(fft_ch3->xAxis , fft_ch3->yAxis);
////              newCurve->addData(x,y);

////              QBrush shadowBrush(QColor(50,50,0), Qt::Dense7Pattern);

////              newCurve->setBrush(shadowBrush);

////              QCPGraph *minGraph = new QCPGraph(fft_ch3->xAxis , fft_ch3->yAxis);
////              newCurve->setChannelFillGraph(minGraph);

////              QObject::connect(fft_ch3->xAxis, static_cast<void(QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), [fft_ch3, minGraph](const QCPRange & newRange){
////                  minGraph->setData(QVector<double>{newRange.lower, newRange.upper},
////                                    QVector<double>{fft_ch3->yAxis->range().lower,fft_ch3->yAxis->range().lower});
////              });

////              QObject::connect(fft_ch3->yAxis, static_cast<void(QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged), [fft_ch3, minGraph](const QCPRange & newRange){
////                  minGraph->setData(QVector<double>{fft_ch3->xAxis->range().lower,fft_ch3->xAxis->range().upper},
////                                    QVector<double>{newRange.lower, newRange.lower});
////              });






//              /*fft_ch3->axisRect()->setupFullAxesBox();
//              fft_ch3->yAxis->grid()->setSubGridVisible(false);
//              fft_ch3->xAxis->grid()->setSubGridVisible(true);
//              fft_ch3->yAxis->setScaleType(QCPAxis::stLogarithmic);
//              fft_ch3->yAxis2->setScaleType(QCPAxis::stLogarithmic);
//              QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
//              fft_ch3->yAxis->setTicker(logTicker);
//              fft_ch3->yAxis2->setTicker(logTicker);
//              fft_ch3->yAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
//              fft_ch3->xAxis->setRange(0, 12500);*/
//              //fft_ch3->yAxis->setRange(1e-6, 1e6);


//              //fft_ch3->yAxis->setScaleType(QCPAxis::stLogarithmic);
//              //fft_ch3->yAxis->grid()->setSubGridVisible(true);
//              //fft_ch3->xAxis->grid()->setSubGridVisible(true);
//              //fft_ch3->yAxis->setScaleType(QCPAxis::stLogarithmic);
//              //fft_ch3->yAxis2->setScaleType(QCPAxis::stLogarithmic);
//              //QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
//              //fft_ch3->yAxis->setTicker(logTicker);
//              //fft_ch3->yAxis2->setTicker(logTicker);
//              //fft_ch3->yAxis->setNumberFormat("b"); // e = exponential, b = beautiful decimal powers
//              //fft_ch3->yAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
//              //fft_ch3->xAxis->setRange(0, 1024);
//              //fft_ch3->yAxis->setRange(1e-6, 1e6);
//              // make range draggable and zoomable:
//              //fft_ch3->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
//              // make top right axes clones of bottom left axes:
//              //fft_ch3->axisRect()->setupFullAxesBox();
//              // connect signals so top and right axes move in sync with bottom and left axes:
//              //connect(fft_ch3->xAxis, SIGNAL(rangeChanged(QCPRange)), fft_ch3->xAxis2, SLOT(setRange(QCPRange)));
//              //connect(fft_ch3->yAxis, SIGNAL(rangeChanged(QCPRange)), fft_ch3->yAxis2, SLOT(setRange(QCPRange)));
//              //fft_ch3->legend->setVisible(true);
//              //fft_ch3->legend->setBrush(QBrush(QColor(255,255,255,150)));
//              //fft_ch3->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop); // make legend align in top





//              fft_ch4->setObjectName("fft"+QString::number(j+4));
//              fft_ch4->addGraph();
//              fft_ch4->xAxis->setRange(0,12500);
//              fft_ch4->yAxis->setRange(-150,10);
//              fft_ch4->graph(0)->setName("Канал "+QString::number(j+4));
//              fft_ch4->legend->setVisible(true);
//              fft_ch4->legend->setFont(QFont(QFont().family(), 8));
//              fft_ch4->legend->setBrush(QColor(255, 255, 255, 150));
//              fft_ch4->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              fft_ch4->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              fft_ch4->setBackground(QBrush(Qt::black));
//              fft_ch4->graph(0)->setPen(QPen(Qt::green));
//              fft_ch4->xAxis->setBasePen(QPen(Qt::white, 1));
//              fft_ch4->yAxis->setBasePen(QPen(Qt::white, 1));
//              fft_ch4->xAxis->setTickPen(QPen(Qt::white, 1));
//              fft_ch4->yAxis->setTickPen(QPen(Qt::white, 1));
//              fft_ch4->xAxis->setSubTickPen(QPen(Qt::white, 1));
//              fft_ch4->yAxis->setSubTickPen(QPen(Qt::white, 1));
//              fft_ch4->xAxis->setTickLabelColor(Qt::white);
//              fft_ch4->yAxis->setTickLabelColor(Qt::white);
//              fft_ch4->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              fft_ch4->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              fft_ch4->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              fft_ch4->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              fft_ch4->xAxis->grid()->setSubGridVisible(true);
//              fft_ch4->yAxis->grid()->setSubGridVisible(true);
//              fft_ch4->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//              fft_ch4->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//              fft_ch4->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//              fft_ch4->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//              fft_ch5->setObjectName("fft"+QString::number(j+5));
//              fft_ch5->addGraph();
//              fft_ch5->xAxis->setRange(0,12500);
//              fft_ch5->yAxis->setRange(-150,10);
//              fft_ch5->graph(0)->setName("Канал "+QString::number(j+5));
//              fft_ch5->legend->setVisible(true);
//              fft_ch5->legend->setFont(QFont(QFont().family(), 8));
//              fft_ch5->legend->setBrush(QColor(255, 255, 255, 150));
//              fft_ch5->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              fft_ch5->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              fft_ch5->setBackground(QBrush(Qt::black));
//              fft_ch5->graph(0)->setPen(QPen(Qt::green));
//              fft_ch5->xAxis->setBasePen(QPen(Qt::white, 1));
//              fft_ch5->yAxis->setBasePen(QPen(Qt::white, 1));
//              fft_ch5->xAxis->setTickPen(QPen(Qt::white, 1));
//              fft_ch5->yAxis->setTickPen(QPen(Qt::white, 1));
//              fft_ch5->xAxis->setSubTickPen(QPen(Qt::white, 1));
//              fft_ch5->yAxis->setSubTickPen(QPen(Qt::white, 1));
//              fft_ch5->xAxis->setTickLabelColor(Qt::white);
//              fft_ch5->yAxis->setTickLabelColor(Qt::white);
//              fft_ch5->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              fft_ch5->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              fft_ch5->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              fft_ch5->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              fft_ch5->xAxis->grid()->setSubGridVisible(true);
//              fft_ch5->yAxis->grid()->setSubGridVisible(true);
//              fft_ch5->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//              fft_ch5->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//              fft_ch5->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//              fft_ch5->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//              fft_ch6->setObjectName("fft"+QString::number(j+6));
//              fft_ch6->addGraph();
//              fft_ch6->xAxis->setRange(0,12500);
//              fft_ch6->yAxis->setRange(-150,10);
//              fft_ch6->graph(0)->setName("Канал "+QString::number(j+6));
//              fft_ch6->legend->setVisible(true);
//              fft_ch6->legend->setFont(QFont(QFont().family(), 8));
//              fft_ch6->legend->setBrush(QColor(255, 255, 255, 150));
//              fft_ch6->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              fft_ch6->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              fft_ch6->setBackground(QBrush(Qt::black));
//              fft_ch6->graph(0)->setPen(QPen(Qt::green));
//              fft_ch6->xAxis->setBasePen(QPen(Qt::white, 1));
//              fft_ch6->yAxis->setBasePen(QPen(Qt::white, 1));
//              fft_ch6->xAxis->setTickPen(QPen(Qt::white, 1));
//              fft_ch6->yAxis->setTickPen(QPen(Qt::white, 1));
//              fft_ch6->xAxis->setSubTickPen(QPen(Qt::white, 1));
//              fft_ch6->yAxis->setSubTickPen(QPen(Qt::white, 1));
//              fft_ch6->xAxis->setTickLabelColor(Qt::white);
//              fft_ch6->yAxis->setTickLabelColor(Qt::white);
//              fft_ch6->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              fft_ch6->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              fft_ch6->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              fft_ch6->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              fft_ch6->xAxis->grid()->setSubGridVisible(true);
//              fft_ch6->yAxis->grid()->setSubGridVisible(true);
//              fft_ch6->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//              fft_ch6->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//              fft_ch6->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//              fft_ch6->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//              fft_ch7->setObjectName("fft"+QString::number(j+7));
//              fft_ch7->addGraph();
//              fft_ch7->xAxis->setRange(0,12500);
//              fft_ch7->yAxis->setRange(-150,10);
//              fft_ch7->graph(0)->setName("Канал "+QString::number(j+7));
//              fft_ch7->legend->setVisible(true);
//              fft_ch7->legend->setFont(QFont(QFont().family(), 8));
//              fft_ch7->legend->setBrush(QColor(255, 255, 255, 150));
//              fft_ch7->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              fft_ch7->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              fft_ch7->setBackground(QBrush(Qt::black));
//              fft_ch7->graph(0)->setPen(QPen(Qt::green));
//              fft_ch7->xAxis->setBasePen(QPen(Qt::white, 1));
//              fft_ch7->yAxis->setBasePen(QPen(Qt::white, 1));
//              fft_ch7->xAxis->setTickPen(QPen(Qt::white, 1));
//              fft_ch7->yAxis->setTickPen(QPen(Qt::white, 1));
//              fft_ch7->xAxis->setSubTickPen(QPen(Qt::white, 1));
//              fft_ch7->yAxis->setSubTickPen(QPen(Qt::white, 1));
//              fft_ch7->xAxis->setTickLabelColor(Qt::white);
//              fft_ch7->yAxis->setTickLabelColor(Qt::white);
//              fft_ch7->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              fft_ch7->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              fft_ch7->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              fft_ch7->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              fft_ch7->xAxis->grid()->setSubGridVisible(true);
//              fft_ch7->yAxis->grid()->setSubGridVisible(true);
//              fft_ch7->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//              fft_ch7->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//              fft_ch7->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//              fft_ch7->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//              fft_ch8->setObjectName("fft"+QString::number(j+8));
//              fft_ch8->addGraph();
//              fft_ch8->xAxis->setRange(0,12500);
//              fft_ch8->yAxis->setRange(-150,10);
//              fft_ch8->graph(0)->setName("Канал "+QString::number(j+8));
//              fft_ch8->legend->setVisible(true);
//              fft_ch8->legend->setFont(QFont(QFont().family(), 8));
//              fft_ch8->legend->setBrush(QColor(255, 255, 255, 150));
//              fft_ch8->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              fft_ch8->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
//              fft_ch8->setBackground(QBrush(Qt::black));
//              fft_ch8->graph(0)->setPen(QPen(Qt::green));
//              fft_ch8->xAxis->setBasePen(QPen(Qt::white, 1));
//              fft_ch8->yAxis->setBasePen(QPen(Qt::white, 1));
//              fft_ch8->xAxis->setTickPen(QPen(Qt::white, 1));
//              fft_ch8->yAxis->setTickPen(QPen(Qt::white, 1));
//              fft_ch8->xAxis->setSubTickPen(QPen(Qt::white, 1));
//              fft_ch8->yAxis->setSubTickPen(QPen(Qt::white, 1));
//              fft_ch8->xAxis->setTickLabelColor(Qt::white);
//              fft_ch8->yAxis->setTickLabelColor(Qt::white);
//              fft_ch8->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              fft_ch8->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//              fft_ch8->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              fft_ch8->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//              fft_ch8->xAxis->grid()->setSubGridVisible(true);
//              fft_ch8->yAxis->grid()->setSubGridVisible(true);
//              fft_ch8->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//              fft_ch8->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//              fft_ch8->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//              fft_ch8->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//Module 1
/*int16_t ch1_t;
int16_t ch2_t;
int16_t ch3_t;
int16_t ch4_t;
int16_t ch5_t;
int16_t ch6_t;
int16_t ch7_t;
int16_t ch8_t;
int16_t ch9_t;
int16_t ch10_t;
int16_t ch11_t;
int16_t ch12_t;
int16_t ch13_t;
int16_t ch14_t;
int16_t ch15_t;
int16_t ch16_t;
int16_t ch17_t;
int16_t ch18_t;
int16_t ch19_t;
int16_t ch20_t;
int16_t ch21_t;
int16_t ch22_t;
int16_t ch23_t;
int16_t ch24_t;
int16_t ch25_t;
int16_t ch26_t;
int16_t ch27_t;
int16_t ch28_t;
int16_t ch29_t;
int16_t ch30_t;
int16_t ch31_t;
int16_t ch32_t;


//Module 2
int16_t ch33_t;
int16_t ch34_t;
int16_t ch35_t;
int16_t ch36_t;
int16_t ch37_t;
int16_t ch38_t;
int16_t ch39_t;
int16_t ch40_t;
int16_t ch41_t;
int16_t ch42_t;
int16_t ch43_t;
int16_t ch44_t;
int16_t ch45_t;
int16_t ch46_t;
int16_t ch47_t;
int16_t ch48_t;
int16_t ch49_t;
int16_t ch50_t;
int16_t ch51_t;
int16_t ch52_t;
int16_t ch53_t;
int16_t ch54_t;
int16_t ch55_t;
int16_t ch56_t;
int16_t ch57_t;
int16_t ch58_t;
int16_t ch59_t;
int16_t ch60_t;
int16_t ch61_t;
int16_t ch62_t;
int16_t ch63_t;
int16_t ch64_t;

//Module 3
int16_t ch65_t;
int16_t ch66_t;
int16_t ch67_t;
int16_t ch68_t;
int16_t ch69_t;
int16_t ch70_t;
int16_t ch71_t;
int16_t ch72_t;
int16_t ch73_t;
int16_t ch74_t;
int16_t ch75_t;
int16_t ch76_t;
int16_t ch77_t;
int16_t ch78_t;
int16_t ch79_t;
int16_t ch80_t;
int16_t ch81_t;
int16_t ch82_t;
int16_t ch83_t;
int16_t ch84_t;
int16_t ch85_t;
int16_t ch86_t;
int16_t ch87_t;
int16_t ch88_t;
int16_t ch89_t;
int16_t ch90_t;
int16_t ch91_t;
int16_t ch92_t;
int16_t ch93_t;
int16_t ch94_t;
int16_t ch95_t;
int16_t ch96_t;

//Module 4
int16_t ch97_t;
int16_t ch98_t;
int16_t ch99_t;
int16_t ch100_t;
int16_t ch101_t;
int16_t ch102_t;
int16_t ch103_t;
int16_t ch104_t;
int16_t ch105_t;
int16_t ch106_t;
int16_t ch107_t;
int16_t ch108_t;
int16_t ch109_t;
int16_t ch110_t;
int16_t ch111_t;
int16_t ch112_t;
int16_t ch113_t;
int16_t ch114_t;
int16_t ch115_t;
int16_t ch116_t;
int16_t ch117_t;
int16_t ch118_t;
int16_t ch119_t;
int16_t ch120_t;
int16_t ch121_t;
int16_t ch122_t;
int16_t ch123_t;
int16_t ch124_t;
int16_t ch125_t;
int16_t ch126_t;
int16_t ch127_t;
int16_t ch128_t;*/
