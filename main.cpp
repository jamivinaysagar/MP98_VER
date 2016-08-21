#include <QApplication>

#include "mainwindow.h"
#include "myinputpanelcontext.h"

//#define QT_NO_DEBUG_OUTPUT

float block_temp,full_percentage;//vinay
double ADCnorm1;
double ADCprevcal1,ADCcal1;
double Rcal1,Rprevcal1;
float SCALE1,ROFFSET1;
int day,month,year,hour,minute,sec;
QString widget_name,current_date; //vinay
QDate current_date1;//vinay
#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<time.h>
#include<unistd.h>
#include<stdlib.h>
//bool startmelt_enable_flag;
/*#include <QtCore/QDebug>

void noMessageOutput(QtMsgType, const char *)
{}*/

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MyInputPanelContext *ic = new MyInputPanelContext;
    a.setInputContext(ic);

    MainWindow w;
    //w.setContextMenuPolicy(Qt::NoContextMenu);
    w.setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    w.setWindowFlags(Qt::FramelessWindowHint);
    w.setFixedSize(800,510);
 //  qInstallMsgHandler(noMessageOutput);
    //w.setAttribute(Qt::WA_OpaquePaintEvent);

   /*QFile filess("/results/style/style.qss");    //setting a stylesheet--vinay
    filess.open(QFile::ReadOnly); // copy the stylesheet.qss in root of mini2440
    w.setStyleSheet(QLatin1String(filess.readAll()));*/
  // w.setStyleSheet("QScrollBar{padding:2px;}");


    w.show();
//    //specify a new font.
//    QFont newFont("Courier", 14, QFont::Bold, true);
//    //set font of application
//    a.setFont(newFont);

    return a.exec();
}
