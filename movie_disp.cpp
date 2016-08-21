#include "movie_disp.h"
#include "ui_movie_disp.h"

#include <QDebug>
#include <QtGui>

movie_disp::movie_disp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::movie_disp)
{
    ui->setupUi(this);
  //  this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
//    this->setWindowFlags(Qt::FramelessWindowHint);
   // movie_disp::setWindowFlags(Qt::Dialog);
 //  movie_disp::setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
  // movie_disp::setFixedSize(785,410);
 // this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
   this->setWindowFlags(Qt::WindowStaysOnTopHint);
  this->setWindowFlags(Qt::FramelessWindowHint);
  this->setFixedSize(800,510);

  //  this->setWindowFlags(Qt::Window|Qt::CustomizeWindowHint);
    //this->setGeometry(QRect(220,80,541,360));
    movie_disp::setWindowFlags(Qt::Dialog);
    this->setGeometry(QRect(10,20,800,500));
    ui->pushButton->setText("Pause");
    ui->frame->setFrameShape(QFrame::Box);
    ui->frame->show();
    pause_mov_disp =0;
    complete_flag = false;
}

movie_disp::~movie_disp()
{
    delete ui;
}

void movie_disp::update_mov_image(QImage x,char y)
{
    qDebug() << "Inside Update movie & image";
    if(y == 0)
    {
        ui->pushButton->hide();
        ui->pushButton_2->hide();
        ui->pushButton_3->show();
    }
    else
    {
        ui->pushButton->show();
        ui->pushButton_2->show();
        ui->pushButton_3->hide();
    }
    ui->label->setPixmap(QPixmap::fromImage(x));
}

void movie_disp::on_pushButton_clicked()
{
    char var=0;
    pause_mov_disp ^= 1;
    if(pause_mov_disp)
    {
        ui->pushButton->setText("Play");
    }
    else
    {
        ui->pushButton->setText("Pause");
    }
    if(complete_flag == true)
    {
        complete_flag = false;
        var = 2;
        qDebug() << "Complete video after button press";
    }
    emit send_play_pause_movie_display(var);
}

void movie_disp::on_pushButton_2_clicked() // stop button
{
    ui->pushButton_2->hide();
    ui->pushButton->setText("Pause");
    ui->label->clear();
    pause_mov_disp = 0;
//    ui->frame->hide();
    emit send_stop_movie_display(true);
}

void movie_disp::receive_movie_completed()
{
    pause_mov_disp = 1;
    complete_flag = true;
    ui->pushButton->setText("Play");
}

void movie_disp::on_pushButton_3_clicked()
{
    ui->label->clear();
    emit send_stop_movie_display(false);
}

