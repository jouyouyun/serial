#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("串口监控助手");
    count = 0;
}

/*
 * 打开串口
 * 创建线程，定时读取数据
 */
void MainWindow::on_openPushButton_clicked()
{
    QString serialPort = QString("/dev/%1").arg(ui->serialComboBox->currentText());
    QString baud = ui->baudComboBox->currentText();
    QString parity = ui->parityComboBox->currentText();
    QString dataBit = ui->dataBitComboBox->currentText();
    QString stopBit = ui->stopBitComboBox->currentText();
    QString flowControl = ui->flowControlComboBox->currentText();

    mycom = new Posix_QextSerialPort(serialPort);
    mycom->open(QIODevice::ReadOnly);
    mycom->setBaudRate(getBaudRate(baud));
    mycom->setParity(getParityType(parity));
    mycom->setDataBits(getDataBit(dataBit));
    mycom->setStopBits(getStopBit(stopBit));
    //数据流控制设置
    mycom->setFlowControl(getFlowControl(flowControl));
    //延时设置，我们设置为延时10ms
    mycom->setTimeout(1, 0);

    myRead[count].newRead = new ReadPort(mycom);
    connect(this, SIGNAL(programExit()), myRead[count].newRead,
            SLOT(slot_program_exit()));
    connect(myRead[count].newRead, SIGNAL(sendDatas(QString)), this,
            SLOT(slots_parse_datas(QString)));

    myRead[count].newThread = new QThread(this);
    connect(myRead[count].newThread, SIGNAL(started()), myRead[count].newRead, SLOT(slot_read_timer()));
    connect(myRead[count].newThread, SIGNAL(finished()), myRead[count].newRead, SLOT(deleteLater()));
    myRead[count].newRead->moveToThread(myRead[count].newThread);
    myRead[count].newThread->start();
    myRead[count].flag = 1;
    mycom = NULL;

    QDateTime cur_time = QDateTime::currentDateTime();//获取系统现在的时间
    //QString str = time.toString("yyyy-MM-dd hh:mm:ss ddd"); //设置显示格式
    QString str = cur_time.toString("yyyy-MM-dd hh:mm:ss"); //设置显示格式
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, QString::number(count));
    item->setText(1, serialPort);
    item->setText(2, str);
    ui->treeWidget->addTopLevelItem(item);

    /* count */
    while (myRead[count].flag == 0) {
        if ( count >= MAX_BUF_LEN ) {
            count = 0;
        }
        count++;
    }
}

/*
 * 关闭已打开串口
 */
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
    //myRead[current].newThread->wait();

    delete myRead[current].newRead;
    myRead[current].newRead = NULL;
    delete myRead[current].newThread;
    myRead[current].newThread = NULL;
    myRead[current].flag = 0;
}

void MainWindow::on_exitPushButton_clicked()
{
    int i = 0;

    for ( ;i <= count; i++ ) {
        if (myRead[i].flag == 1) {
            myRead[i].newThread->deleteLater();
            delete myRead[i].newRead;
            myRead[i].newRead = NULL;
            delete myRead[i].newThread;
            myRead[i].newThread = NULL;
        }
    }

    emit programExit();
    this->close();
}

/*
 * 清除数据
 */
void MainWindow::on_clearButton_clicked()
{
    ui->textBrowser->clear();
}

/*
 * 处理读取到的数据
 */
void MainWindow::slots_parse_datas(QString strDatas)
{
    /* 将读取到的数据显示在界面中 */
    ui->textBrowser->append(strDatas);
}

/*
 * 获取波特率
 */
BaudRateType MainWindow::getBaudRate(QString &baudRate)
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

/*
 * 获取数据位
 */
DataBitsType MainWindow::getDataBit(QString &dataBit)
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

/*
 * 获取校验和
 */
ParityType MainWindow::getParityType(QString &parityType)
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

/*
 * 获取停止位
 */
StopBitsType MainWindow::getStopBit(QString &stopBit)
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

/*
 * 获取流控制
 */
FlowType MainWindow::getFlowControl(QString &flowControl)
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

MainWindow::~MainWindow()
{
    delete ui;
}
