#include "edit_profile.h"
#include "ui_edit_profile.h"
#include "profile.h"
#include<QDebug>
edit_profile::edit_profile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::edit_profile)
{
    ui->setupUi(this);
    temp_profile = new Profile();
    QRegExp validnameRegExx("^[a-zA-Z0-9_]*$");    // vinay
    QValidator* Validator123 = new QRegExpValidator(validnameRegExx, this);
    ui->lineEdit-> setValidator(Validator123);
    QIntValidator *val = new QIntValidator(0, 100, this);
    ui->lineEdit_5->setValidator(val);
    ui->lineEdit_6->setValidator(val);
    ui->lineEdit_7->setValidator(val);

    QDoubleValidator *val1 = new QDoubleValidator(0,400,1,this);

    ui->lineEdit_2->setValidator(val1);
    ui->lineEdit_3->setValidator(val1);
    QIntValidator *val2 = new QIntValidator(0, 20, this);
    ui->lineEdit_4->setValidator(val2);

}

void edit_profile::update_form(Profile* temp)
{
    qDebug()<<"in update form";
    QString str;

    ui->lineEdit->setText(temp->Chemical_Name);


    str = QString::number(temp->Start_temp);
    ui->lineEdit_2->setText(str);

    str = QString::number(temp->Stop_temp);
    ui->lineEdit_3->setText(str);

    str = QString::number(temp->Ramp_rate);
    ui->lineEdit_4->setText(str);

    str = QString::number(temp->On_set);
    ui->lineEdit_5->setText(str);

    str = QString::number(temp->Single);
    ui->lineEdit_7->setText(str);

    str = QString::number(temp->Clear);
    ui->lineEdit_6->setText(str);

    ui->automode->setChecked(temp->auto_mode);
    automatic=temp->auto_mode;
    qDebug()<<"automatic"<<temp->auto_mode;

    ui->manualmode->setChecked(temp->manual_mode);
    manual=temp->manual_mode;
    qDebug()<<"manual"<<temp->manual_mode;

    //11425
    onset_clear_sel=temp->single_sel;
    ui->lineEdit_5->setEnabled(!onset_clear_sel);
    ui->lineEdit_6->setEnabled(!onset_clear_sel);
    ui->lineEdit_7->setEnabled(onset_clear_sel);
    ui->onset_clear_button->setChecked(onset_clear_sel);
    //11425
}

edit_profile::~edit_profile()
{
    delete ui;
}

void edit_profile::on_pushButton_clicked() // upon clikcing ok from edit profile screen
{

    temp_profile->Chemical_Name = ui->lineEdit->text();
    temp_profile->Start_temp = ui->lineEdit_2->text().toFloat();
    temp_profile->Stop_temp = ui->lineEdit_3->text().toFloat();
    temp_profile->Ramp_rate = ui->lineEdit_4->text().toFloat();
    temp_profile->On_set = ui->lineEdit_5->text().toFloat();
    temp_profile->Single = ui->lineEdit_7->text().toFloat();
    temp_profile->Clear = ui->lineEdit_6->text().toFloat();
    temp_profile->single_sel=onset_clear_sel;//11425
    temp_profile->auto_mode=automatic;
    temp_profile->manual_mode=manual;

    qDebug()<<"automatic"<<automatic;
    qDebug()<<"manual"<<manual;
    emit form_Data(temp_profile);
}

void edit_profile::on_pushButton_2_clicked() // upon clicking cnacel from edit profile screen
{
    //close this window
    this->close();
}
//11425
void edit_profile::on_onset_clear_button_clicked()
{
    //static bool onset_clear_sel= temp->onset_clear_sel;
   onset_clear_sel^=1;
   ui->lineEdit_5->setEnabled(!onset_clear_sel);
   ui->lineEdit_6->setEnabled(!onset_clear_sel);
   ui->lineEdit_7->setEnabled(onset_clear_sel);
}
//11425



void edit_profile::on_automode_clicked()
{
   automatic=1;
   manual=0;
   ui->automode->setChecked(automatic);
   ui->manualmode->setChecked(manual);

}

void edit_profile::on_manualmode_clicked()
{
    manual=1;
    automatic=0;
    ui->automode->setChecked(automatic);
    ui->manualmode->setChecked(manual);
}
