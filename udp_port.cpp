#include "udp_port.h"
#include <QFile>        // Подключаем класс QFile
#include <QTextStream>  // Подключаем класс QTextStream


UDP_port::UDP_port(QObject *parent) : QObject(parent)
{
    //thisImage = QImage(SIZE_X, SIZE_Y, QImage::Format_RGB32);
    modImage = QImage(1000, 576, QImage::Format_RGB32);
    srcImage = QImage(1000, 576, QImage::Format_RGB32);
}

UDP_port::~UDP_port()
{
    qDebug("By in Thread!");
    //emit finished_Port();//Сигнал о завершении работы
}

void UDP_port::init()
{
//    line=0;

//    column=0;
//    row=0;
//    pix_cnt=0;
//    fon_state=0;
//    file_line_cnt=0;
//    cnt_min_max=0;
//    alg_fone=0;

//    array_sub.resize(576);
//    array_fone.resize(576);

//    cnt_frame=0;


//    memset(&buf_mod[0],0x00,sizeof(buf_mod));
//    memset(&buf_min[0],0x00,sizeof(buf_min));
//    memset(&buf_max[0],0x00,sizeof(buf_max));
//    memset(&buf_sub[0],0x00,sizeof(buf_sub));


//    memset(&buf_temp_fon[0],0x00,sizeof(buf_temp_fon));
//    memset(&buf_temp_fon2[0],0x00,sizeof(buf_temp_fon2));






    p = (unsigned char *) calloc(50000000, sizeof(unsigned char));
    if(!p)
    {
        qDebug()<<"Allocation memory failure";
        exit (1);
    }
    else
        qDebug()<<"Allocation memory ok!";

    pointer = p;
    j=0;
    i=0;


    //thisPort.bind(48000);
    thisPort.bind(QHostAddress::AnyIPv4,48000,QUdpSocket::ShareAddress);
    connect(&thisPort, SIGNAL(readyRead()), this, SLOT(readUDP()));

    //int sz = 131072;
    //setsockopt( thisPort.socketDescriptor(), SOL_SOCKET, SO_RCVBUF, reinterpret_cast< char* >( &sz ), sizeof( sz ) );

    tmr = new QTimer();
    tmr->setInterval(1000);
    connect(tmr, SIGNAL(timeout()), this, SLOT(tmrOut()));
    tmr->start();

    packet_alg=1;

}

void UDP_port::AlgFone(int val)
{
    alg_fone=val;
}

void UDP_port::ArrayFone(QByteArray array)
{
    array.resize(576);
    array_fone.setRawData(array.data(),576);
}

void UDP_port::readUDP()
{
    QHostAddress sender;
    quint16      senderPort;
    QByteArray   datagram;
    quint16      pct_cnt;

    datagram.resize(1408);

    while (thisPort.pendingDatagramSize() != -1)
    {
        thisPort.readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        if(sender == QHostAddress("192.168.4.3") )
        {
           //memcpy(&pct_cnt,&datagram.data()[0], 2);
           //emit(pktTotable(pct_cnt));

            if(j<=20000)//10000 packets
            {
                memcpy(&p[j],&datagram.data()[0], 2);
                j+=2;
            }
            else if(i==0)
            {
                qDebug()<<"Write Ok!";
                emit(pktTotable(pointer));
                i=1;
            }



        }

    }
}


//void UDP_port::ChangeBrightness(quint8 value)
//{
//   //qDebug() << "UDP_port::ChangeBrightness()";
//   //qDebug() << "value: " << QString::number(value);
//    Brightness = value;
//}


//void UDP_port::FON()
//{
//   fon_state=1;
//   qDebug() << "UDP_port::FON():";
//   qDebug() << "fon_state: " << QString::number(fon_state);
//}


void UDP_port::tmrOut()
{
    qDebug() << "fon_state: " << QString::number(fon_state);
    //emit UDPspeed(QString(" UDP video stream: %1 kb/s, Packets: %2(Good) / %3(Bad), FPS: %4, ErrLine: %5") .arg(cntRXspeed * 1024 / 1000) .arg(cntRX) .arg(cntBADpacket) .arg(cntFrame) .arg(errLine) );

    //cntRXspeed = 0;
    //cntFrame = 0;
    //data_cnt=0;
}



/*
        cntRXspeed++;

        //packet_cnt++;
        data_cnt+=datagram.size();


        if( datagram.size() == 578 ) //(1024+4)
            cntRX++;

        //for file log
        j=0;
        for(i=0;i<290; i++)
        {
           //memcpy(&buf_file[i], &datagram.data()[j],2);
           memcpy(&buf_mod[i], &datagram.data()[j],2);
           j+=2;
        }


        memcpy(&buf_file[0], &datagram.data()[0],578);

        quint16 t_val1, t_val2, cur_frame;

        memcpy(&cur_frame,&buf_mod[0],2);



        //прямой буфер
        pix_cnt+=288;

        j=0;
        for(i=0;i<288; i++)
        {
           memcpy(&buf[i], &datagram.data()[j],2);
           j+=2;
        }




        unsigned int cur_pix, source_cnt,  fon_cnt;
        cur_pix=0;
        fon_cnt=0;
        source_cnt=0;
        qint32 ARGB_temp;



        if(cur_frame<=499) //row = 0;
        {
            row=0;
            column = cur_frame*2;
        }
        else if( (cur_frame>=500) && (cur_frame<=999) )
        {
            row=144;
            column = (cur_frame-500)*2;
        }
        else if( (cur_frame>=1000) && (cur_frame<=1499) )
        {
            row=288;
            column = (cur_frame-1000)*2;
        }
        else if( (cur_frame>=1500) && (cur_frame<=1999) )
        {
            row=432;
            column = (cur_frame-1500)*2;
        }



        //смещение будет зависить от cur_frame

        for(j=row;j<(144+row);j++)
        {
            QRgb*lineptr1 = (QRgb*)modImage.scanLine(j);
            lineptr1+= column; //смещение

            QRgb*lineptr2 = (QRgb*)srcImage.scanLine(j);
            lineptr2+= column; //смещение


            if(alg_fone==0)
            {
                //pixel1 = ((buf[cur_pix]   - 8192)/32.0)*Brightness;
                //pixel2 = ((buf[cur_pix+1] - 8192)/32.0)*Brightness;

                pixel1 = ((buf[cur_pix] )/32.0)*Brightness;
                pixel2 = ((buf[cur_pix+1])/32.0)*Brightness;
            }

            else //для режима по выбранному кадру или вычета из текущего
            {
                pixel1 = ((buf[cur_pix]   - buf_temp_fon[cur_pix])/32.0)*Brightness;
                pixel2 = ((buf[cur_pix+1] - buf_temp_fon[cur_pix+1])/32.0)*Brightness;
            }

            if(pixel1<0) pixel1=0;
            else if(pixel1>255) pixel1=255;

            if(pixel2<0) pixel2=0;
            else if(pixel2>255) pixel2=255;

            *lineptr1 = qRgb(pixel1,pixel1,pixel1); lineptr1+= 1;
            *lineptr1 = qRgb(pixel2,pixel2,pixel2);
            cur_pix+=2;

            pixel1_source = buf[source_cnt];
            pixel2_source = buf[source_cnt+1];

            ARGB_temp = 0xFF000000;
            ARGB_temp|= pixel1_source;

            *lineptr2 = ARGB_temp; lineptr2+= 1;

            ARGB_temp = 0xFF000000;
            ARGB_temp|= pixel2_source;

            *lineptr2 = ARGB_temp;
            source_cnt+=2;
        }



        //поиск максимального и минимального элемента
        switch(cnt_min_max)
        {
            case 0:
            {
                for(i=0;i<288;i++)
                {
                    memcpy(&buf_min[i], &buf_mod[i], 2);
                    memcpy(&buf_max[i], &buf_mod[i], 2);
                }
                cnt_min_max++;
            }break;

            case 1:
            {
                for(i=0;i<288;i++)
                {
                    if(buf_mod[i] > buf_max[i])
                        buf_max[i] = buf_mod[i];

                    if(buf_mod[i] < buf_min[i])
                        buf_min[i] = buf_mod[i];
                }
            }break;
        }


            //Запись в файл
            if(file_line_cnt<10000)
            {
                QFile fileOut("fileout.txt"); // Связываем объект с файлом fileout.txt
                QTextStream writeStream(&fileOut);

                if(fileOut.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
                {
                    j=0;
                    for(i=0; i<288; i+=2)
                    {
                        //t_val1 = ((buf_file[j] - (buf_temp_fon2[j])) /32.0) * Brightness;
                        //t_val2 = ((buf_file[j+1] - (buf_temp_fon2[j+1])) /32.0) * Brightness;

                        t_val1 = buf_file[j];
                        t_val2 = buf_file[j+1];

                        //if(t_val1<0) t_val1=0;
                        //else if(t_val1>255) t_val1=255;

                        //if(t_val2<0) t_val2=0;
                        //else if(t_val2>255) t_val2=255;

                        writeStream << QString::number(t_val1) <<";"<< QString::number(t_val2)<<";";
                        j+=2;
                    }
                    writeStream << "\n";
                    fileOut.close(); // Закрываем файл
                    file_line_cnt++;
                }
            }
        //}

        //Сохраняем для вычета фона


        switch(alg_fone)
        {
            case 0:
            {
                memset(&buf_temp_fon[0],0x00,sizeof(buf_temp_fon));
                memset(&buf_temp_fon2[0],0x00,sizeof(buf_temp_fon2));
                //qDebug() << "Фон: не активен";
            }break;

            case 1:
            {
                //qDebug() << "Фон: по выбранному кадру";
                memcpy(&buf_temp_fon[0],   &array_fone.data()[0],   576);
            }break;

            case 2:
            {
                memcpy(&buf_temp_fon[0],&buf[0],576);
                memcpy(&buf_temp_fon2[0],&buf_file[0],576);
                //qDebug() << "Фон: вычитание из текущего";
            }break;
        }



        //if(pix_cnt>=576000)
        if(cur_frame==1999)
        {
            //pix_cnt=0;
            cntFrame++;
            //разница мин и макс элемента
            for(i=0;i<288;i++)
            {
                buf_sub[i] = buf_max[i] - buf_min[i];
                //qDebug() << "buf_sub[" << QString::number(i) << "]=" << QString::number(buf_sub[i]);
            }


            memcpy(array_sub.data(),&buf_sub[0],576);
            //memcpy(&t_val1, &array_sub.data()[0],2);
            //qDebug() << "array_sub" << QString::number(t_val1);
            //qDebug() << "\n";

            emit(redrawScreen(modImage, srcImage, array_sub));
            cnt_min_max=0;
        }
*/
