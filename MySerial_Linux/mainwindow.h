#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextBrowser>
#include <QDebug>
#include <QTimer>

#include "readport.h"

#define MAX_BUF_LEN 256

typedef struct readArray {
    ReadPort *newRead;
    QThread *newThread;
    int flag;
}ReadArray;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
signals:
    void programExit();

private slots:
    void on_openPushButton_clicked();

    void on_closePushButton_clicked();

    void on_exitPushButton_clicked();

    void slots_parse_datas(QString);

    void on_clearButton_clicked();

private:
    Ui::MainWindow *ui;
    //ReadPort *newReadPort;
    Posix_QextSerialPort *mycom;
    ReadArray myRead[MAX_BUF_LEN];
    int count;

    BaudRateType getBaudRate(QString &baudRate);
    DataBitsType getDataBit(QString &dataBit);
    ParityType getParityType(QString &parityType);
    StopBitsType getStopBit(QString &stopBit);
    FlowType getFlowControl(QString &flowControl);
};

#endif // MAINWINDOW_H
