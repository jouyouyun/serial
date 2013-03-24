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
    ReadPort(QString serialPort, QString baud, QString parity, QString dataBit,
            QString stopBit, QString flowControl);
    ~ReadPort();
    Posix_QextSerialPort *myPort;
    
private:
    QTimer *readTimer;

    BaudRateType getBaudRate(QString &baudRate);
    DataBitsType getDataBit(QString &dataBit);
    ParityType getParityType(QString &parityType);
    StopBitsType getStopBit(QString &stopBit);
    FlowType getFlowControl(QString &flowControl);
    
public slots:
    void slot_read_com();               //监听并读取串口数据
    void slot_read_timer();
    
};

#endif // READPORT_H
