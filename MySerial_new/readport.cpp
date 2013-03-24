#include "readport.h"

ReadPort::ReadPort(QString serialPort, QString baud, QString parity, QString dataBit,
                   QString stopBit, QString flowControl)
{
    qDebug()<<"New Port: "<<QThread::currentThreadId();
    myPort = new Posix_QextSerialPort(serialPort);
    myPort->open(QIODevice::ReadOnly);
    myPort->setBaudRate(getBaudRate(baud));
    myPort->setParity(getParityType(parity));
    myPort->setDataBits(getDataBit(dataBit));
    myPort->setStopBits(getStopBit(stopBit));
    //数据流控制设置
    myPort->setFlowControl(getFlowControl(flowControl));
    //延时设置，我们设置为延时10ms
    myPort->setTimeout(1, 0);
    qDebug() << "Opening port successfully";
}

void ReadPort::slot_read_timer()
{
    readTimer = new QTimer(this);
    //设置延时为100ms
    readTimer->start(100);

    //信号和槽函数关联，延时一段时间，进行读串口操作
    connect(readTimer,SIGNAL(timeout()),this, SLOT(slot_read_com()));
}

void ReadPort::slot_read_com()
{
    int len;
    char str[MAX_BUF_LEN];
    QByteArray temp;
    QString datas;

    //qDebug() << "Receive...";
    while (myPort->bytesAvailable() > 0)
    {
        qDebug() << "receive" << myPort->bytesAvailable();
        //QByteArray temp = myCom->readAll();
        memset(str, 0, MAX_BUF_LEN);
        len = myPort->readData(str,MAX_BUF_LEN);
        qDebug()<<"len : "<<len<<endl;
        temp.append(str,len);
        //qDebug() << "receiver len = " << len << "msg:" << temp;
        temp.end();
        datas = QString(temp.toHex());
    }

    if (!datas.isEmpty()) {
        qDebug()<<datas;
    }
}

BaudRateType ReadPort::getBaudRate(QString &baudRate)
{
    BaudRateType baud;

    if ( baudRate.compare("9600") == 0 ) {
        baud = BAUD9600;
    } else if ( baudRate.compare("19200") == 0 ) {
        baud = BAUD19200;
    } else if ( baudRate.compare("38400") == 0 ) {
        baud = BAUD38400;
    } else if ( baudRate.compare("115200") == 0 ) {
        baud = BAUD115200;
    } else {
        baud = BAUD115200;
    }

    return baud;
}

DataBitsType ReadPort::getDataBit(QString &dataBit)
{
    DataBitsType data;

    if (dataBit.compare("5") == 0 ) {
        data = DATA_5;
    } else if (dataBit.compare("6") == 0 ) {
        data = DATA_6;
    } else if (dataBit.compare("7") == 0 ) {
        data = DATA_7;
    } else if (dataBit.compare("8") == 0 ) {
        data = DATA_8;
    } else {
        data = DATA_8;
    }

    return data;
}

ParityType ReadPort::getParityType(QString &parityType)
{
    ParityType parity;

    if (parityType.compare("NONE") == 0) {
        parity = PAR_NONE;
    } else if (parityType.compare("ODD") == 0) {
        parity = PAR_ODD;
    } else if (parityType.compare("EVEN") == 0) {
        parity = PAR_EVEN;
    } else {
        parity = PAR_NONE;
    }

    return parity;
}

StopBitsType ReadPort::getStopBit(QString &stopBit)
{
    StopBitsType stop;

    if (stopBit.compare("1") == 0 ) {
        stop = STOP_1;
    } else if (stopBit.compare("2") == 0 ) {
        stop = STOP_2;
    } else {
        stop = STOP_1_5;
    }

    return stop;
}

FlowType ReadPort::getFlowControl(QString &flowControl)
{
    FlowType flow;

    if (flowControl.compare("HARDWARE") == 0 ) {
        flow = FLOW_HARDWARE;
    } else if (flowControl.compare("XONXOFF") == 0 ) {
        flow = FLOW_XONXOFF;
    } else {
        flow = FLOW_OFF;
    }

    return flow;
}

ReadPort::~ReadPort()
{
    delete myPort;
    delete readTimer;
}
