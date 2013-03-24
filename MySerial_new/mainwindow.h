#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QTimer>

#include "readport.h"

typedef struct readArray {
    ReadPort *newRead;
    QThread *newThread;
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
    
private slots:
    void on_openPushButton_clicked();

    void on_closePushButton_clicked();

    void on_exitPushButton_clicked();

private:
    Ui::MainWindow *ui;
    //ReadPort *newReadPort;
    //Posix_QextSerialPort *mycom;
    ReadArray myRead[MAX_BUF_LEN];
    int count;
};

#endif // MAINWINDOW_H
