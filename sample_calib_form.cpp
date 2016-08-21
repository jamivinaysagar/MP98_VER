#include "sample_calib_form.h"
#include "ui_sample_calib_form.h"

#include <QtGui>
sample_calib_form::sample_calib_form(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sample_calib_form)
{
    ui->setupUi(this);
    sample_calib_form::setFixedSize(800,510);
    sample_calib_form::setWindowFlags(Qt::FramelessWindowHint);
    standard1_hide();
    standard2_hide();
    standard3_hide();
    standard4_hide();
    ui->range4->hide();
    ui->offset4->hide();

}

sample_calib_form::~sample_calib_form()
{
    delete ui;
}

void sample_calib_form::receive_sample_profile(float x,float y,float z, int no_of_samples,bool k ,int p)
{
    factory=k;
    single=p;
    QString str;
    qDebug()<<"no_of_samples"<<no_of_samples;
    qDebug()<<"x"<<x;
    qDebug()<<"y"<<y;
    qDebug()<<"factory"<<factory;
    qDebug()<<"single"<<single;

    if(no_of_samples>=0)
       {
        standard1_show();
        standard2_hide();
        standard3_hide();
        standard4_hide();
        ui->range4->hide();
        ui->offset4->hide();

        if(factory)
        {
            str.clear();
        str = res_adding1(x);
        range1=str;
        str.append("<");
        str.append(range1);
        ui->cmp_fac1->setText(str);
        str = res_adding1(y);
        ui->slope1->setText(str);
        str = res_adding1(z);
        ui->offset1->setText(str);
        }
        else
        {
            if(single==1)
            {
               str = res_adding1(x);
               ui->cmp_fac1->setText(str);
            }
            else
            {
             str = res_adding1(x);
             range_1=str;
             str=res_adding1(y);
             range_2=str;
             str=range1;
             str.append(range_1);
             str.append("-");
             str.append(range_2);
             str = res_adding1(z);
             ui->offset1->setText(str);

            }
        }

       }

     if(no_of_samples>=1)
     {
         standard1_show();
         standard2_show();
         standard3_hide();
         standard4_hide();
         ui->range4->hide();
         ui->offset4->hide();
        if(factory)
        {

            str = res_adding1(x);
            range_1=str;
            str =range_1;
            str.append("to");
            str.append(range_2);
            ui->cmp_fac2->setText(str);
            str = res_adding1(y);
            ui->slope2->setText(str);
            str = res_adding1(z);
            ui->offset2->setText(str);
        }
        else
        {
             if(single==1)
            {
               str = res_adding1(x);
               ui->cmp_fac2->setText(str);
            }
            else
            {
             str = res_adding1(x);
             range_1=str;
             str=res_adding1(y);
             range_2=str;
             str=range1;
             str.append(range_1);
             str.append("-");
             str.append(range_2);
             str = res_adding1(z);
             ui->offset2->setText(str);

            }
        }
    }

   if(no_of_samples>=2)
   {
       standard1_show();
       standard2_show();
       standard3_show();
       standard4_hide();
       ui->range4->hide();
       ui->offset4->hide();
          if(single==1)
         {

         str = res_adding1(x);
         range3=str;
         str =range2;
         str.append("to");
         str.append(range3);
         ui->cmp_fac3->setText(str);
         str = res_adding1(y);
         ui->slope3->setText(str);
         str = res_adding1(z);
         ui->offset3->setText(str);
      }
         else
         {
              if(single==1)
             {
                str = res_adding1(x);
                ui->cmp_fac2->setText(str);
             }
             else
             {
              str = res_adding1(x);
              range_1=str;
              str=res_adding1(y);
              range_2=str;
              str=range1;
              str.append(range_1);
              str.append("-");
              str.append(range_2);
              str = res_adding1(z);
              ui->offset2->setText(str);
         }

    }
    }
     if(no_of_samples>=3)
    {
         standard1_show();
         standard2_show();
         standard3_show();
         standard4_show();
         ui->range4->show();
         ui->offset4->show();

          if(single==1)
         {
            str = res_adding1(x);
            ui->range4->setText(str);
         }
         else
         {
          str = res_adding1(x);
          range_1=str;
          str=res_adding1(y);
          range_2=str;
          str=range1;
          str.append(range_1);
          str.append("-");
          str.append(range_2);
          str = res_adding1(z);
          ui->offset4->setText(str);
         }

    }

}

void sample_calib_form::on_pushButton_clicked()
{
    QString errMsg;
    QMessageBox msgBox;
     int length = ui->calib_interval->text().toInt();
     if (factory)
     {
    if (length > 365 || length < 15)
    {
        errMsg.append("Calibration time interval in between 15 to 365 days");
        msgBox.critical(0,"Error",errMsg);
        msgBox.setFixedSize(500,200);
    }
    else
    {
        emit send_from_sample_form(true,length);
    }
    }
     else
     {
    if (length > 365 || length < 15)
    {
        errMsg.append("Calibration time interval in between 15 to 365 days");
        msgBox.critical(0,"Error",errMsg);
        msgBox.setFixedSize(500,200);
    }
    else
    {
        emit send_from_sample_form(true,length);
    }
    }
    //Noofsamples=ui->lineEdit_21->text().toInt();
   /* QMessageBox msgBox(this);
    msgBox.setWindowTitle("User Alert");
    msgBox.setText("samples calibration completed. Please click ok to store the calibration results");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    {
        case QMessageBox::Ok:
             emit send_from_sample_form(true);
             break;
        case QMessageBox::Cancel:
            emit send_from_sample_form(false);
            break;
        default:
            // should never be reached
            break;
     }
    }
    bool valid_flag;
    valid_flag=sample_profile_check();
    if(valid_flag)
    {
        str  = "polmon";
        rgl = ui->lineEdit_2->text().toFloat();
        rgh = ui->lineEdit_3->text().toFloat();
        m_rgl = ui->lineEdit_4->text().toFloat();
        m_rgh = ui->lineEdit_5->text().toFloat();
        emit send_from_sample_form(true);
    }
    else
    {
        ;
    }*/
}
bool sample_calib_form::sample_profile_check()
{
    /*QMessageBox msgBox;
    QString errMsg;
    bool error_bit = false;

    int length = ui->lineEdit->text().size();
    if (length > 20 || length <=0)
    {
        error_bit = true;
        errMsg.append("Chemical name cannot be empty and must be less than 20 characters in size.\n");
    }

    float start_temp = ui->lineEdit_2->text().toFloat();
    if (start_temp > 399.0 || start_temp < 25)
    {
        error_bit = true;
        errMsg.append("Start temperature must be with in the range 25 to 399 degC.\n");
    }

    float stop_temp = ui->lineEdit_3->text().toFloat();
    if (stop_temp > 400.0 || stop_temp < start_temp)
    {
        error_bit = true;
        errMsg.append("Stop temperature must be greater than start temperature and be less than 400degC.\n");
    }

    float start_temp1 = ui->lineEdit_4->text().toFloat();
    if (start_temp1 > 399.0 || start_temp1 < 25)
    {
        error_bit = true;
        errMsg.append("Start temperature must be with in the range 25 to 399 degC.\n");
    }

    float stop_temp1 = ui->lineEdit_5->text().toFloat();
    if (stop_temp1 > 400.0 || stop_temp1 < start_temp1)
    {
        error_bit = true;
        errMsg.append("Stop temperature must be greater than start temperature and be less than 400degC.\n");
    }

    if(error_bit)
    {
        msgBox.critical(0,"Error",errMsg);
        msgBox.setFixedSize(500,200);
        return false;
    }
    else
    {
        return true;
    }*/
   return true;

}

void sample_calib_form::on_pushButton_2_clicked()
{

    //emit send_from_sample_form(false);
    QString errMsg;
    QMessageBox msgBox;
     int length = ui->calib_interval->text().toInt();
  /*  if (length > 24 || length < 6)
    {
        errMsg.append("Calibration time interval in between 6 to 24 months");
        msgBox.critical(0,"Error",errMsg);
        msgBox.setFixedSize(500,200);
    }
    else*/
    {
    //Noofsamples=ui->lineEdit_21->text().toInt();
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("User Alert");
    msgBox.setText("Do you want Discard the calibration data?.\n");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    {
        case QMessageBox::Ok:
             emit send_from_sample_form(false,length);
             break;
        case QMessageBox::Cancel:
            break;
        default:
            // should never be reached
            break;
     }
    }

}
void sample_calib_form::standard1_show()
{
    ui->cmp_fac1->show();
    ui->slope1->setEnabled(false);
    ui->offset1->show();
    ui->offset1->setEnabled(false);

    if(factory)
    ui->slope1->show();
    /*ui->label_90->show();
    ui->lineEdit_2->show();
    ui->lineEdit_2->setEnabled(false);
    ui->lineEdit_3->show();
    ui->lineEdit_3->setEnabled(false);
    ui->lineEdit_4->show();
    ui->lineEdit_4->setEnabled(false);
    ui->lineEdit_5->show();
    ui->lineEdit_5->setEnabled(false);
    ui->lineEdit_18->show();
    ui->lineEdit_18->setEnabled(false);
    ui->label_78->setEnabled(false);
    ui->label_78->show();
    ui->label_79->setEnabled(false);
    ui->label_79->show();*/

}
void sample_calib_form::standard2_show()
{
    ui->cmp_fac2->show();
    ui->slope2->setEnabled(false);
    ui->offset2->show();
    ui->offset2->setEnabled(false);
    if(factory)
    ui->slope2->show();

   /* ui->label_91->show();
    ui->lineEdit->show();
    ui->lineEdit->setEnabled(false);
    ui->lineEdit_6->show();
    ui->lineEdit_6->setEnabled(false);
    ui->lineEdit_7->show();
    ui->lineEdit_7->setEnabled(false);
    ui->lineEdit_8->show();
    ui->lineEdit_8->setEnabled(false);
    ui->lineEdit_20->show();
    ui->lineEdit_20->setEnabled(false);
     ui->label_96->setEnabled(false);
     ui->label_96->show();
     ui->label_97->setEnabled(false);
     ui->label_97->show();*/

}
void sample_calib_form::standard3_show()
{
    ui->cmp_fac3->show();

    ui->slope3->setEnabled(false);
    ui->offset3->show();
    ui->offset3->setEnabled(false);
    if(factory)
    {
    ui->slope3->show();
    }
    /*ui->label_92->show();
    ui->lineEdit_9->show();
    ui->lineEdit_9->setEnabled(false);
    ui->lineEdit_10->show();
    ui->lineEdit_10->setEnabled(false);
    ui->lineEdit_11->show();
    ui->lineEdit_11->setEnabled(false);
    ui->lineEdit_12->show();
    ui->lineEdit_12->setEnabled(false);
    ui->lineEdit_19->show();
    ui->lineEdit_19->setEnabled(false);
     ui->label_98->setEnabled(false);
     ui->label_98->show();
     ui->label_99->setEnabled(false);
     ui->label_99->show();*/
}
void sample_calib_form::standard4_show()
{
    ui->cmp_fac3->show();

    ui->slope3->setEnabled(false);
    ui->offset3->show();
    ui->offset3->setEnabled(false);
    ui->range4->show();
    ui->offset4->show();
    ui->offset4->setEnabled(false);
    if(factory)
    {
    ui->slope3->show();

    }
   /* ui->label_93->show();
    ui->lineEdit_13->show();
    ui->lineEdit_13->setEnabled(false);
    ui->lineEdit_14->show();
    ui->lineEdit_14->setEnabled(false);
    ui->lineEdit_15->show();
    ui->lineEdit_15->setEnabled(false);
    ui->lineEdit_16->show();
    ui->lineEdit_16->setEnabled(false);
    ui->lineEdit_17->show();
    ui->lineEdit_17->setEnabled(false);
    ui->label_100->setEnabled(false);
    ui->label_100->show();
    ui->label_101->setEnabled(false);
    ui->label_101->show();*/
}
void sample_calib_form::standard1_hide()
{


     ui->cmp_fac1->hide();
     ui->slope1->hide();
     ui->offset1->hide();

}
void sample_calib_form::standard2_hide()
{

     ui->cmp_fac1->hide();
     ui->slope1->hide();
     ui->offset1->hide();


}
void sample_calib_form::standard3_hide()
{


     ui->cmp_fac2->hide();
     ui->slope2->hide();
     ui->offset2->hide();
}
void sample_calib_form::standard4_hide()
{

     ui->cmp_fac3->hide();
     ui->slope3->hide();
     ui->offset3->hide();
     ui->range4->hide();
     ui->offset4->hide();
}
QString sample_calib_form::res_adding1(float receive_temp)
{
    int dummy=0;
    float dummy1=0.0;
    QString res_str;
    res_str.clear();
    dummy=receive_temp*10;
    dummy1=(float)dummy/10.0;
    res_str=QString::number(dummy1);
    if((dummy%10)==0)
    {
        res_str.append(".0");
    }
    else
    {
        ;
    }
    return res_str;

}
