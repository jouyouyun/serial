#include "readport.h"

ReadPort::ReadPort( Posix_QextSerialPort *mycom )
{
    //qDebug()<<"New Port: "<<QThread::currentThreadId();
    myPort = mycom;
    mycom = NULL;
    qDebug() << "Opening port successfully";
}

/* Set timer */
void ReadPort::slot_read_timer()
{
    readTimer = new QTimer(this);
    //设置延时为100ms
    readTimer->start(10);

    //信号和槽函数关联，延时一段时间，进行读串口操作
    connect(readTimer,SIGNAL(timeout()),this, SLOT(slot_read_com()));
}

/* Reading datas */
void ReadPort::slot_read_com()
{
    int len;
    char str[MAX_BUF_LEN];
    QByteArray temp;
    QString datas;

    //qDebug() << "Receive...";
    while (myPort->bytesAvailable() > 0)
    {
        //qDebug() << "receive" << myPort->bytesAvailable();
        //QByteArray temp = myCom->readAll();
        memset(str, 0, MAX_BUF_LEN);
        len = myPort->readData(str,MAX_BUF_LEN);
        //qDebug()<<"len : "<<len<<endl;
        temp.append(str,len);
        //qDebug() << "receiver len = " << len << "msg:" << temp;
        temp.end();
        datas = QString(temp);
        //datas = QString(temp.toHex());
    }

    if ( (!datas.isEmpty()) && (datas.size() < 80) ) {
        qDebug()<<datas;
        emit sendDatas(datas);
    }
}

void ReadPort::slot_program_exit()
{
    myPort->close();
    qDebug()<<"slot_program_exit";
}
