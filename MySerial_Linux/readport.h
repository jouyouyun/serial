#ifndef READPORT_H
#define READPORT_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QDebug>

#include "posix_qextserialport.h"

#define MAX_BUF_LEN 256

class ReadPort : public QObject
{
    Q_OBJECT
public:
    ReadPort();
    ReadPort( Posix_QextSerialPort *mycom );
    Posix_QextSerialPort *myPort;
    
private:
    QTimer *readTimer;

signals:
    void sendDatas(QString);
    
public slots:
    void slot_read_com();               //监听并读取串口数据
    void slot_read_timer();
    void slot_program_exit();
    
};

#endif // READPORT_H
