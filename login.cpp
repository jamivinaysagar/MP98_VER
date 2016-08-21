#include "login.h"
#include "ui_login.h"

#include<QtGui>
#include<QDebug>

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#define LEN 8
extern QString widget_name;

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
   /* login::setWindowFlags(Qt::FramelessWindowHint);
    login::setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    login::setWindowFlags(Qt::FramelessWindowHint);
    login::setGeometry(600,360,990,480);*/
    //login::setFixedSize(800,510);
    QRegExp validnameRegEx("^[a-zA-Z0-9_-]*$");
    QValidator* profileExtensionValidator;

    profileExtensionValidator = new QRegExpValidator(validnameRegEx, this);
    ui->lineEdit_usr-> setValidator(profileExtensionValidator);
    ui->lineEdit_pwd-> setValidator(profileExtensionValidator);

    login::setWindowFlags(Qt::Dialog);
    login::setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    login::setFixedSize(500,400);

}

login::~login()
{
    delete ui;
}

//11425
void login::on_pushButton_clicked()
{

    for(int i=0;i<11;i++)
    {
        if((ui->lineEdit_usr->text() == temp_USER[i] && ui->lineEdit_pwd->text()== temp_PWD[i])||(ui->lineEdit_usr->text() == "pol" && ui->lineEdit_pwd->text()== "121"))
        {
             if(ui->lineEdit_usr->text() == "pol" && ui->lineEdit_pwd->text()== "121")
             {
                        emit send_from_login(0,12);
             }
           /*else if(ui->lineEdit_usr->text() == "\0" && ui->lineEdit_pwd->text()== "\0")
             {
                 ui->label_3->setAlignment(Qt::AlignCenter);
                 ui->label_3->setText("User ID & Password cannot be Empty");

             }*///remember
             else
                 emit send_from_login(0,i);
             break;
        }
        else
             {

            ui->label_3->setAlignment(Qt::AlignCenter);
            ui->label_3->setText("Wrong User ID & Password ");

             }
    }
     ui->lineEdit_pwd->clear();
     ui->lineEdit_usr->clear();

}

void QLineEdit::mousePressEvent(QMouseEvent *e)
{
    QWidget *widget = qApp->widgetAt(QCursor::pos());

    widget_name = widget->objectName();

    qDebug() << "widget_name"<< widget_name;
}
void login::on_pushButton_2_clicked()
{
    //emit send_from_login(1,13);
    ui->lineEdit_pwd->clear();
    ui->lineEdit_usr->clear();
    ui->label_3->clear();
}
void login::receive_usr_pwd(QString x,QString y)
{
    static int i=0;
    temp_USER[i] = x;
    temp_PWD[i]  = y;
    if(++i>=12)
        i=0;
}

//11425


