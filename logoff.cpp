#include "logoff.h"
#include "ui_logoff.h"

logoff::logoff(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::logoff)
{

    ui->setupUi(this);
    logoff::setWindowFlags(Qt::FramelessWindowHint);
    this->setStyleSheet("background-color: black;");
    /*login::setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    login::setWindowFlags(Qt::FramelessWindowHint);
    login::setGeometry(600,360,990,480);*/
    logoff::setFixedSize(800,510);


}

logoff::~logoff()
{
    delete ui;
}
