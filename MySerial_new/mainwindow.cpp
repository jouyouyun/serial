#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("串口监控助手");
    count = 0;
}

void MainWindow::on_openPushButton_clicked()
{
    QString serialPort = QString("/dev/%1").arg(ui->serialComboBox->currentText());
    QString baud = ui->baudComboBox->currentText();
    QString parity = ui->parityComboBox->currentText();
    QString dataBit = ui->dataBitComboBox->currentText();
    QString stopBit = ui->stopBitComboBox->currentText();
    QString flowControl = ui->flowControlComboBox->currentText();

//    mycom = new Posix_QextSerialPort(serialPort);
//    mycom->open(QIODevice::ReadOnly);
//    mycom->setBaudRate(getBaudRate(baud));
//    mycom->setParity(getParityType(parity));
//    mycom->setDataBits(getDataBit(dataBit));
//    mycom->setStopBits(getStopBit(stopBit));
//    //数据流控制设置
//    mycom->setFlowControl(getFlowControl(flowControl));
//    //延时设置，我们设置为延时10ms
//    mycom->setTimeout(1, 0);

//    newReadPort = ReadPort(mycom);
//    myRead[count].newRead = newReadPort;
    //ReadPort *newReadPort = new ReadPort(serialPort, baud, parity, dataBit, stopBit, flowControl);
    myRead[count].newRead = new ReadPort(serialPort, baud, parity, dataBit, stopBit, flowControl);
    //myRead[count].newRead = newReadPort;
    //QThread *thread = new QThread(this);
    myRead[count].newThread = new QThread(this);
    //myRead[count].newThread = thread;
//    connect(thread, SIGNAL(started()), newReadPort, SLOT(slot_read_timer()));
//    connect(thread, SIGNAL(finished()), newReadPort, SLOT(deleteLater()));
//    newReadPort->moveToThread(thread);
//    // Starts an event loop, and emits thread->started()
//    thread->start();
    connect(myRead[count].newThread, SIGNAL(started()), myRead[count].newRead, SLOT(slot_read_timer()));
    connect(myRead[count].newThread, SIGNAL(finished()), myRead[count].newRead, SLOT(deleteLater()));
    myRead[count].newRead->moveToThread(myRead[count].newThread);
    // Starts an event loop, and emits thread->started()
    myRead[count].newThread->start();

    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, QString::number(count));
    item->setText(1, serialPort);
    item->setText(2, "2012-13-15");
    ui->treeWidget->addTopLevelItem(item);
    count++;

//    readTimer = new QTimer(this);
//    //设置延时为100ms
//    readTimer->start(100);

//    //信号和槽函数关联，延时一段时间，进行读串口操作
//    connect(readTimer,SIGNAL(timeout()),this,SLOT(slot_read_com()));
}

void MainWindow::on_closePushButton_clicked()
{
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    if ( item == NULL ) {
        return ;
    }

    int current = item->text(0).toInt();
    myRead[current].newRead->myPort->close();

    ui->treeWidget->takeTopLevelItem(current);
    delete item;
    item = NULL;
    myRead[current].newThread->deleteLater();
    //connect(myRead[current].newThread, SIGNAL(terminated()), myRead[current].newThread, SLOT(deleteLater()));
    //myRead[current].newThread->terminate();
}

void MainWindow::on_exitPushButton_clicked()
{
//    if ( isOpen ){
//        mycom->close();
//    }
    this->close();
}

//void MainWindow::slot_read_com()
//{
//    int len;
//    char str[MAX_BUF_LEN];
//    QByteArray temp;
//    QString datas;

//    while (mycom->bytesAvailable() > 0)
//    {
//        qDebug() << "receive" << mycom->bytesAvailable();
//        //QByteArray temp = myCom->readAll();
//        memset(str, 0, MAX_BUF_LEN);
//        len = mycom->readData(str,MAX_BUF_LEN);
//        qDebug()<<"len : "<<len<<endl;
//        temp.append(str,len);
//        //qDebug() << "receiver len = " << len << "msg:" << temp;
//        temp.end();
//        datas = QString(temp.toHex());
//    }

//    if (!datas.isEmpty()) {
//        qDebug()<<datas;QThread *thread = new QThread(this);
//    }
//}

//BaudRateType MainWindow::getBaudRate(QString &baudRate)
//{
//    BaudRateType baud;

//    if ( baudRate.compare("9600") == 0 ) {
//        baud = BAUD9600;
//    } else if ( baudRate.compare("19200") == 0 ) {
//        baud = BAUD19200;
//    } else if ( baudRate.compare("38400") == 0 ) {
//        baud = BAUD38400;
//    } else if ( baudRate.compare("115200") == 0 ) {
//        baud = BAUD115200;
//    } else {
//        baud = BAUD115200;
//    }

//    return baud;
//}

//DataBitsType MainWindow::getDataBit(QString &dataBit)
//{
//    DataBitsType data;

//    if (dataBit.compare("5") == 0 ) {
//        data = DATA_5;
//    } else if (dataBit.compare("6") == 0 ) {
//        data = DATA_6;
//    } else if (dataBit.compare("7") == 0 ) {
//        data = DATA_7;
//    } else if (dataBit.compare("8") == 0 ) {
//        data = DATA_8;
//    } else {
//        data = DATA_8;
//    }

//    return data;
//}

//ParityType MainWindow::getParityType(QString &parityType)
//{
//    ParityType parity;

//    if (parityType.compare("NONE") == 0) {
//        parity = PAR_NONE;
//    } else if (parityType.compare("ODD") == 0) {
//        parity = PAR_ODD;
//    } else if (parityType.compare("EVEN") == 0) {
//        parity = PAR_EVEN;
//    } else {
//        parity = PAR_NONE;
//    }

//    return parity;
//}

//StopBitsType MainWindow::getStopBit(QString &stopBit)
//{
//    StopBitsType stop;

//    if (stopBit.compare("1") == 0 ) {
//        stop = STOP_1;
//    } else if (stopBit.compare("2") == 0 ) {
//        stop = STOP_2;
//    } else {
//        stop = STOP_1_5;
//    }

//    return stop;
//}

//FlowType MainWindow::getFlowControl(QString &flowControl)
//{
//    FlowType flow;

//    if (flowControl.compare("HARDWARE") == 0 ) {
//        flow = FLOW_HARDWARE;
//    } else if (flowControl.compare("XONXOFF") == 0 ) {
//        flow = FLOW_XONXOFF;
//    } else {
//        flow = FLOW_OFF;
//    }

//    return flow;
//}

MainWindow::~MainWindow()
{
    delete ui;
//    delete mycom;
//    delete readTimer;
}
