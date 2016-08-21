
//http://docs.opencv.org/modules/imgproc/doc/histograms.html

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "profile.h"
#include "camera.h"
#include "edit_profile.h"
#include "temp_ctrl.h"
#include "logoff.h"
#include <QDebug>
#include <QImage>
#include <QtGui>
#include <QPainter>
#include <QRegExpValidator>
#include <QWSServer>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/rtc.h>
#include <fcntl.h>
#include <qpainter.h>

#define A   0.0039083      // A term of forward transfer functions
#define B  -0.0000005775      // B term of forward transfer functions
#define C  -0.000000000004183     // C term of forward transfer functions

#define x_axis 200
#define y_axis 2200

#define width_rect 2000
#define height_rect 300

#define storedevice "/results"
#define usbdrive "/udisk"
extern float block_temp,full_percentage;//vinay
extern double ADCnorm1;
extern double ADCprevcal1,ADCcal1;
extern double Rcal1,Rprevcal1;
extern float SCALE1,ROFFSET1;
extern int day,month,year,hour,minute,sec;
//extern bool startmelt_enable_flag;
extern QDate current_date1; //vinay
extern QString current_date;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->hide();

    ui->pushButton_16->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_77->hide();

    qDebug() << "In mainwindow";

    camera1 = new camera();
    model = new QDirModel(this);
    //  model = new File
    //QFileSystemModel *model = new QFileSystemModel;
    model->setReadOnly(false);
    model->setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    model->setSorting(QDir::IgnoreCase | QDir::Name);
    ui->treeView->setModel(model);

    index5 = model->index("/results");

      ui->treeView->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {min-width: 25px;;padding:2px;}");//;padding:2px;
      QFile filess("/style.qss");    //setting a stylesheet--vinay
      filess.open(QFile::ReadOnly);
      ui->treeView->setStyleSheet(QLatin1String(filess.readAll()));


    ui->treeView->setRootIndex(index5);
    ui->treeView->expand(index5);
    ui->treeView->scrollTo(index5);
    ui->treeView->setCurrentIndex(index5);
    ui->treeView->resizeColumnToContents(0);
    ui->treeView->setIconSize(QSize(25,25));
    ui->treeView->setExpandsOnDoubleClick(false);
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);    // Turn off double-click for edit
    QFont f1("Arial",15);
    ui->treeView->setFont(f1);
    ui->open_button->show();

    ui->treeView->setColumnHidden(1,true);
    ui->treeView->setColumnHidden(2,true);

    ui->treeView->header()->stretchSectionCount();

    current_date.clear();
    current_date.append(QDate::currentDate().toString("dd/MM/yyyy"));//format of date
    QString str1=current_date;
    QDate converted_date1(QDate::fromString(str1, "dd/MM/yyyy"));
    current_date1 = converted_date1;
    qDebug() << "current date = " << current_date1;


   /* QString xyz;
    QString commandx1="df -h >> out.txt";
    system(qPrintable(commandx1));

    QString commandx2="grep root /out.txt >> out1.txt";
    system(qPrintable(commandx2));
    QFile file("/out1.txt");
    if(!file.open(QIODevice::ReadOnly))
    {
   qDebug() << "error";
    }
    else
    {
    QTextStream in(&file);
    xyz = in.readLine();
    file.close();
    }
    qDebug() << "out.txt_ only root"<< xyz;
    xyz.remove(" ");
    xyz.remove("/dev/root");
    qDebug() <<"after removing"<< xyz;
    QStringList str;
    str = xyz.split("M");
    qDebug() <<"after removing- string list="<< str;

    qDebug() <<" string list_0"<< str.at(0);
    qDebug() <<" string list_0"<< str.at(1);
    qDebug() <<" string list_0"<< str.at(2);

    float size_capacity = (str.at(0)).toFloat();
    float size_used = str.at(1).toFloat();
    float size_available = str.at(2).toFloat();

    qDebug() << "size capacity"<< size_capacity;
    qDebug() << "size used"<< size_used;
    qDebug() << "size available"<< size_available;
    qDebug() << "memory size"<< memory_size;*/
    float memory_size =  750;
    float default_size =1;
    QDirIterator y("/results", QDirIterator::Subdirectories);
    unsigned long long sum1=0;

    while(y.hasNext())
    {
        sum1 = (sum1 + y.fileInfo().size());
        y.next();
    }
    qDebug() << "size used in  sdcard = " << sum1/(1024*1024);

    //val1=memory_size-((float)sum1/(1024*1024));
    QString disk_total_size = QString::number(memory_size);
    QString disk_available = QString::number(memory_size-((float)sum1/(1024*1024)));

      QString txt;
      txt.append(disk_available);
      txt.append(" MB / ");
      txt.append(disk_total_size);
      txt.append("MB");
      ui->label_134->setText(txt);
      ui->progressBar->setMinimum(default_size);
      qDebug() << "minimum"<<ui->progressBar->minimum();
     ui->progressBar->setMaximum(memory_size);
      qDebug() << "maximum"<<ui->progressBar->maximum();
      ui->progressBar->setValue(memory_size-((float)sum1/(1024*1024)));
      ui->progressBar->setAlignment(Qt::AlignCenter);
      float value =ui->progressBar->value();

       float compare = (((value-default_size))/(memory_size-default_size))*100;

      if(compare <= 10)
      {
          qDebug() << "Red";
          QString myStyleSheet_g = QString(" QProgressBar::chunk { background: red; }");
          QString s = ui->progressBar->styleSheet().append(myStyleSheet_g);
          ui->progressBar->setStyleSheet(s);
      }
      else if((compare > 10)&&(compare < 90))
      {
          qDebug() << "yellow";
          QString myStyleSheet_g = QString(" QProgressBar::chunk { background: yellow; }");
          QString s = ui->progressBar->styleSheet().append(myStyleSheet_g);
          ui->progressBar->setStyleSheet(s);
      }
      else
      {
          qDebug() << "green";
          QString myStyleSheet_g = QString(" QProgressBar::chunk { background: green; }");
          QString s = ui->progressBar->styleSheet().append(myStyleSheet_g);
          ui->progressBar->setStyleSheet(s);
      }

      full_percentage = (((float)sum1/(1024*1024))/(float)memory_size)*100;

       ui->factory_calib_button->hide();
       ui->field_calib_button->hide();
       ui->reset->hide();

       ui->radioButton->setChecked(false);
       ui->radioButton_2->setChecked(false);
       ui->radioButton_3->setChecked(false);                //vinay
       ui->radioButton_4->setChecked(false);
       ui->radioButton_5->setChecked(false);
       ui->radioButton_6->setChecked(false);
       ui->radioButton_7->setChecked(false);
       ui->radioButton_8->setChecked(false);


       ui->Initial_melt1->hide();
       ui->Initial_melt2->hide();
       ui->Initial_melt3->hide();


      ui->label_147->hide();
      ui->label_148->hide();
      ui->label_145->hide();
      ui->label_146->hide();
      ui->lineEdit_77->hide();
      ui->lineEdit_78->hide();
      ui->lineEdit_79->hide();
      ui->lineEdit_81->hide();
      ui->lineEdit_82->hide();
      ui->lineEdit_84->hide();
      ui->lineEdit_85->hide();
      ui->lineEdit_87->hide();
    ui->sample1_label1->hide();
    ui->sample1_label2->hide();
    ui->sample1_label3->hide();
    ui->sample1_label4->hide();
    ui->sample1_label5->hide();
    ui->sample1_label6->hide();
    ui->label1_err_message->hide();

    ui->sample2_label1->hide();
    ui->sample2_label2->hide();
    ui->sample2_label3->hide();
    ui->sample2_label4->hide();
    ui->sample2_label5->hide();
    ui->sample2_label6->hide();
    ui->label2_err_message->hide();

    ui->sample3_label1->hide();
    ui->sample3_label2->hide();
    ui->sample3_label3->hide();
    ui->sample3_label4->hide();
    ui->sample3_label5->hide();
    ui->sample3_label6->hide();
    ui->label3_err_message->hide();
    calculate_offset_mulfactor();
    //ui->pushButton_30->hide();            //vinay


    ui->tabWidget->setStyleSheet("QTabBar::tab { height: 50px; width: 157px; color:black;font: 12pt }");
    ui->tabWidget_2->setStyleSheet("QTabBar::tab { height: 40px; font: 11pt }");
    ui->tabWidget_3->setStyleSheet("QTabBar::tab { height: 40px; width: 131px; font: 11pt }");
    ui->tabWidget_4->setStyleSheet("QTabBar::tab { height: 40px; width: 262px;font: 11pt }");
    ui->tabWidget_5->setStyleSheet("QTabBar::tab { height: 40px; width: 262px;font: 11pt }");
    ui->tabWidget_8->setStyleSheet("QTabBar::tab { height: 40px; width: 196px;font: 11pt }");

    ui->tabWidget_8->setTabEnabled(1,false);
    ui->tabWidget_8->setTabEnabled(2,false);                 //vinay
    ui->tabWidget_8->setTabEnabled(3,false);
   /* ui->tabWidget_8->removeTab(1);
    ui->tabWidget_8->removeTab(2);
    ui->tabWidget_8->removeTab(3);*/




    ////qdebug() << "In mainwindow1";
    results_saved = false;
    recording = false;
    time_cycle = ui->lineEdit_8->text().toInt();
    camera_timer = new QTimer();

    time_cycle = 100;

    profile_form = new edit_profile();
    curr_profile = new Profile();

    num_of_profiles = 12;

    camera_once_flag = false;

    //.........................
    customPlot = ui->customPlot;
    customPlot->setNoAntialiasingOnDrag(true);


    // configure right and top axis to show ticks but no labels (could've also just called customPlot->setupFullAxesBox):
    customPlot->xAxis2->setVisible(true);
    customPlot->xAxis2->setTickLabels(true);
    customPlot->yAxis2->setVisible(true);
    customPlot->yAxis2->setTickLabels(true);
    customPlot->addGraph(customPlot->xAxis, customPlot->yAxis);
    customPlot->addGraph(customPlot->xAxis, customPlot->yAxis);
    customPlot->addGraph(customPlot->xAxis, customPlot->yAxis);
    customPlot->addGraph(customPlot->xAxis2, customPlot->yAxis2);
    customPlot->addGraph(customPlot->xAxis2, customPlot->yAxis2);
    customPlot->addGraph(customPlot->xAxis2, customPlot->yAxis2);


    connect(this,SIGNAL(send_Start_Recording()),this,SLOT(recording_Start()));
    connect(this,SIGNAL(send_Stop_Recording()),this,SLOT(recording_Stop()));
    temp_timer  = new QTimer();
    login_form  = new login();//11425
    login_timer = new QTimer();//11425
    blink_timer = new QTimer();//11425
    logoff_form =new logoff();
    logoff_timer = new QTimer();
    beep_timer =new QTimer();
    delay_timer=new QTimer();



    connect(login_timer,SIGNAL(timeout()),this,SLOT(raise_login_form()));//11425

    connect(login_form,SIGNAL(send_from_login(char,int)),this,SLOT(receive_from_login(char,int)));//11425
    connect(this,SIGNAL(send_usr_psw(QString,QString)),login_form,SLOT(receive_usr_pwd(QString,QString)));//11425

    ///////////////////////////////////////////////////////////////////////////

    connect(this,SIGNAL(send_from_sample_form(bool,int)),this,SLOT(receive_from_sample_form(bool,int)));//11425

    save_mov = false;



    pFormatCtx = NULL;

    pause = false;

    movie_display = new movie_disp();
    stop_clicked = false;
    mov_timer = new QTimer();
    ovrsht_timer = new QTimer;



    connect(mov_timer,SIGNAL(timeout()),this,SLOT(loop_player()));
    connect(this,SIGNAL(frameready(QImage,char)),movie_display,SLOT(update_mov_image(QImage,char)));
    connect(movie_display,SIGNAL(send_play_pause_movie_display(char)),this,SLOT(receive_play_pause_movie_display(char)));
    connect(movie_display,SIGNAL(send_stop_movie_display(bool)),this,SLOT(receive_stop_movie_display(bool)));
    connect(this,SIGNAL(movie_completed()),movie_display,SLOT(receive_movie_completed()));



    //---------------------
    temp_control = new Temp_Ctrl();

    //---------thread Init---------------
    thread_spi = new QThread();


    connect(thread_spi, SIGNAL(started()), temp_control, SLOT(control_loop()));

    connect(temp_control, SIGNAL(finished()), thread_spi, SLOT(quit()));

    connect(temp_control, SIGNAL(finished()), temp_control, SLOT(deleteLater()));

    connect(thread_spi, SIGNAL(finished()), thread_spi, SLOT(deleteLater()));

    connect(this,SIGNAL(send_setpoint(float,float,float,float,float,float,float,int,bool)),temp_control,SLOT(receive_setpoint(float,float,float,float,float,float,float,int,bool)));
    connect(this,SIGNAL(send_pid_signal(bool)),temp_control,SLOT(receive_pid_signal(bool)));

    connect(temp_control,SIGNAL(emit_start_temp_reached()),this,SLOT(start_temp_reached()));
    connect(temp_control,SIGNAL(emit_end_temp_reached()),this,SLOT(end_temp_reached()));
    connect(this,SIGNAL(send_pwm_signal(bool)),temp_control,SLOT(receive_pwm_signal(bool)));
    connect(this,SIGNAL(pid_strt(bool)),temp_control,SLOT(receive_pid_strt(bool)));
    connect(this,SIGNAL(sbt_cntrl(bool)),temp_control,SLOT(receive_sbt_cntrl(bool)));//14547


    connect(this,SIGNAL(send_sample_calib_factors(double,double,bool)),temp_control,SLOT(receive_sample_calib_factors(double,double,bool)));
    //11425
    connect(this,SIGNAL(send_field_calib_factors(double,bool)),temp_control,SLOT(receive_field_calib_factors(double,bool)));
    connect(temp_control,SIGNAL(display_pv()),this,SLOT(receive_display_pv()));
    connect(this,SIGNAL(emit_start_loop(bool)),temp_control,SLOT(start_loop(bool)));
    connect(this,SIGNAL(fan_setpoint(float)),temp_control,SLOT(receive_fan_setpoint(float)));
    connect(this,SIGNAL(start_melting(bool)),temp_control,SLOT(receive_melting(bool)));



    emit emit_start_loop(true);
    temp_control->moveToThread(thread_spi);
    thread_spi->start();
    //thread_spi->

    //-------------------------------
    //   connect(this,SIGNAL(send_to_temp_ctrl(bool)),temp_control,SLOT(receive_from_mw(bool)));
    initialize_profiles();
    calib_pdf_report=false;

    ui->lineEdit_31->setEnabled(false);//11425
    ui->tabWidget->setTabEnabled(1,false);//11425
    ui->tabWidget->setTabEnabled(2,false);//11425
    ui->tabWidget->setTabEnabled(3,false);//11425
    ui->tabWidget->setTabEnabled(4,false);//11425
    ui->tabWidget->setTabEnabled(5,false);//11425
    user_hide();//11425

    ramprate_configuration_screen_hide();
    hide_sample_pushbuttons();//11425
    //11425
    stop_button_flag = false;
    treeview_open_flag = true;
    QRegExp validnameRegEx1("^[a-zA-Z0-9_]*$");    // vinay
    QValidator* Validator123 = new QRegExpValidator(validnameRegEx1, this);
    ui->lineEdit_25-> setValidator(Validator123);
    ui->lineEdit_35-> setValidator(Validator123);


    QRegExp validnameRegEx("^[a-zA-Z0-9_-]*$");    // vinay
    QValidator* Validator1 = new QRegExpValidator(validnameRegEx, this);
    ui->lineEdit_20-> setValidator(Validator1);
    ui->lineEdit_21-> setValidator(Validator1);
    ui->lineEdit_22-> setValidator(Validator1);
    ui->lineEdit_23-> setValidator(Validator1);
    ui->lineEdit_24-> setValidator(Validator1);
    ui->lineEdit_32-> setValidator(Validator1);
    ui->lineEdit_33-> setValidator(Validator1);
    ui->lineEdit_36-> setValidator(Validator1);
    ui->lineEdit_89-> setValidator(Validator1);
    ui->lineEdit_88-> setValidator(Validator1);
    ui->lineEdit_40-> setValidator(Validator1);
    ui->lineEdit_76-> setValidator(Validator1);
    ui->lineEdit_80-> setValidator(Validator1);
    ui->lineEdit_83-> setValidator(Validator1);
    ui->lineEdit_86-> setValidator(Validator1);
   // ui->lineEdit_62-> setValidator(Validator1);
    ui->lineEdit_67-> setValidator(Validator1);
    ui->lineEdit_70-> setValidator(Validator1);  //vinay



    QIntValidator *val = new QIntValidator(0, 100, this);
    ui->lineEdit_29->setValidator(val);
    ui->lineEdit_31->setValidator(val);
    ui->lineEdit_30->setValidator(val);
   // ui->lineEdit_37->setValidator(val);

    QIntValidator *val8 = new QIntValidator(0, 10000, this);
    ui->lineEdit_8->setValidator(val8);


    QIntValidator *val3 = new QIntValidator(0, 12, this);
    ui->lineEdit_month->setValidator(val3);


    QIntValidator *val7 = new QIntValidator(0, 23, this);
    ui->lineEdit_hour->setValidator(val7);

    QIntValidator *val4 = new QIntValidator(0, 59, this);
    ui->lineEdit_minutes->setValidator(val4);
    ui->lineEdit_seconds->setValidator(val4);

    QIntValidator *visa = new QIntValidator(2, 4, this);

    ui->lineEdit_90->setValidator(visa);


    QIntValidator *val5 = new QIntValidator(0, 31, this);
    ui->lineEdit_day->setValidator(val5);

   // QIntValidator *val6 = new QIntValidator(2000, 3000, this);
    QString year_val =  "(?:20[0-9][0-9])";
    QValidator* val6 = new QRegExpValidator(QRegExp("^" + year_val + "$"), this);

    ui->lineEdit_year->setValidator(val6);


    QDoubleValidator *val1 = new QDoubleValidator(0,400,1,this);

    ui->lineEdit_26->setValidator(val1);
    ui->lineEdit_27->setValidator(val1);    
    ui->lineEdit_77->setValidator(val1);      //---vinay
    ui->lineEdit_78->setValidator(val1);
    ui->lineEdit_79->setValidator(val1);
    ui->lineEdit_81->setValidator(val1);
    ui->lineEdit_82->setValidator(val1);
    ui->lineEdit_84->setValidator(val1);
    ui->lineEdit_85->setValidator(val1);
    ui->lineEdit_87->setValidator(val1);


    QDoubleValidator *val2 = new QDoubleValidator(0,20,1,this);

    ui->lineEdit_28->setValidator(val2);
    ui->lineEdit_4->setValidator(val2);
    ui->lineEdit_5->setValidator(val2);
    ui->lineEdit_6->setValidator(val2);
    ui->lineEdit_7->setValidator(val2);


    QDoubleValidator *val9 = new QDoubleValidator(0,405,1,this);

    ui->lineEdit->setValidator(val9);
    ui->lineEdit_2->setValidator(val9);

    QString Octet = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QValidator* val10 = new QRegExpValidator(QRegExp("^" + Octet + "$"), this);
            // QRegExp("^" + Octet + "\\." + Octet + "\\." + Octet + "\\." + Octet + "$"), this));

    ui->lineEdit_IP1->setValidator(val10);
    ui->lineEdit_IP2->setValidator(val10);
    ui->lineEdit_IP3->setValidator(val10);
    ui->lineEdit_IP4->setValidator(val10);
    ui->lineEdit_MASK1->setValidator(val10);
    ui->lineEdit_MASK2->setValidator(val10);
    ui->lineEdit_MASK3->setValidator(val10);
    ui->lineEdit_MASK4->setValidator(val10);
    ui->lineEdit_GATEWAY1->setValidator(val10);
    ui->lineEdit_GATEWAY2->setValidator(val10);
    ui->lineEdit_GATEWAY3->setValidator(val10);
    ui->lineEdit_GATEWAY4->setValidator(val10);
    stop_check_calib=false;
    factory_signin=false;
    authorized_signin=false;
    password_ok_flag = false;
    sample_profile_sel=avg_var=0;
    index_tabwidget = ui->tabWidget->currentIndex();
    emit update_status(2);//remember
    ui->label_62->show();//11425
    ui->label_70->show();//11425
    calib_flag=false;//11425
    ui->pushButton_97->hide();//11425
    ui->pushButton_99->setEnabled(true);
    ui->pushButton_102->setEnabled(true);
    ui->pushButton_101->setEnabled(true);
    ui->pushButton_98->setEnabled(true);
    ui->pushButton_100->hide();//11425
    ui->single_sel_button->setChecked(0);
    user_push_buttons_hide();//11425
    ramp_cnt=calib_cnt=0;
    first_flag=second_flag=flag_array[0]=flag_array[1]=0;
    QIcon icon3;
    icon3.addFile(QString::fromUtf8("/login.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->pushButton_32->setIcon(icon3);
    ui->pushButton_32->setIconSize(QSize(90,90));
    onset_clear=false;
    toogle_status=0;

    Noof_standards_hide();
    ui->pushButton_75->hide();
    ui->pushButton_74->hide();
    calculate_offset_mulfactor();//11425


    login_timer->start(1);//11425
    fanon=false;
    start=false;//pid cntrl after end temp 14547
    end=false;
    fan=false;


    clear1=false;
    clear2=false;
    clear3=false;
    toggle_graph1=false;
    toggle_graph2=false;
    toggle_graph3=false;
    factory_pdf=false;
    field_pdf=false;
    reset=false;
    field_reset=false;
    melt1=melt2=melt3=false;
    qDebug()<<"clear and onset";


}

void MainWindow::initialize_profiles()
{

    for(int i=0; i<num_of_profiles; i++)
        profiles[i] = new Profile();
    //Reading profile data from init.txt
    QFile file("/profile/init.txt");

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);

        for(int i=0; i < num_of_profiles; i++)
        {
            profiles[i]->Chemical_Name = in.readLine();
            profiles[i]->Start_temp = in.readLine().toFloat();
            profiles[i]->Stop_temp = in.readLine().toFloat();
            profiles[i]->Ramp_rate = in.readLine().toFloat();
            profiles[i]->On_set = in.readLine().toFloat();
            profiles[i]->Single = in.readLine().toFloat();
            profiles[i]->Clear = in.readLine().toFloat();
            profiles[i]->single_sel=in.readLine().toFloat();//11425
            profiles[i]->auto_mode=in.readLine().toInt();
            qDebug() <<"auto_mode" << profiles[i]->auto_mode;
            profiles[i]->manual_mode=in.readLine().toInt();
            qDebug() <<"manual_mode" << profiles[i]->manual_mode;
                     }
                    //ijk = in.readLine();
                     p = in.readLine().toFloat();
                    /*//qdebug() << "string p = " << p;
                    //qdebug() << "p = " << QString::number(p);*/
                     ui->lineEdit_4->setText(QString::number(p));
                    //qdebug() << "cvRect1.y :"<< p;
                    //ijk = in.readLine();
                   /*MainWindow::*/Ti = in.readLine().toFloat();
                    //ijk = in.readLine();
                    //qdebug() << "cvRect1.y :"<< Ti;
                     ui->lineEdit_5->setText(QString::number(Ti));
                     sbt = in.readLine().toFloat();
                    //ijk = in.readLine();
                     ui->lineEdit_7->setText(QString::number(sbt));
                     time_cycle = in.readLine().toInt();
                    //ijk = in.readLine();
                     ui->lineEdit_8->setText(QString::number(time_cycle));
                     SCALE1 =in.readLine().toFloat();
                    //ijk = in.readLine();
                     ROFFSET1 = in.readLine().toFloat();
                    //ijk = in.readLine();
             //       //qdebug() << "offset :"<<ROFFSET1;
                     cvRect1.x = in.readLine().toInt();
                    //ijk = in.readLine();
                    //qdebug() << "cvRect1.x :"<<cvRect1.x;
                     cvRect1.y = in.readLine().toInt();
                    //ijk = in.readLine();
                    //qdebug() << "cvRect1.y :"<<cvRect1.y;
                     cvRect1.width = in.readLine().toInt();
                    //ijk = in.readLine();
                    //qdebug() << "cvRect1.width :"<<cvRect1.width;
                     cvRect1.height = in.readLine().toInt();
                   // ijk = in.readLine();
                    //qdebug() << "cvRect1.height :"<<cvRect1.height;
                     cvRect2.x = in.readLine().toInt();
                    //ijk = in.readLine();
                 //qdebug() << "cvRect2.x :"<<cvRect2.x;
                     cvRect2.y = in.readLine().toInt();
                    //ijk = in.readLine();
                 //qdebug() << "cvRect2.y :"<<cvRect2.y;
                     cvRect2.width = in.readLine().toInt();
                    //ijk = in.readLine();
                  //qdebug() << "cvRect2.width :"<<cvRect2.width;
                     cvRect2.height = in.readLine().toInt();
                    //ijk = in.readLine();
                //qdebug() << "cvRect2.height :"<<cvRect2.height;
                     cvRect3.x = in.readLine().toInt();
                    //ijk = in.readLine();
                //qdebug() << "cvRect3.x :"<<cvRect3.x;
                     cvRect3.y = in.readLine().toInt();
                    //ijk = in.readLine();
          //qdebug() << "cvRect3.y :"<<cvRect3.y;
                     cvRect3.width = in.readLine().toInt();
                    //ijk = in.readLine();
         //qdebug() << "cvRect3.width :"<<cvRect3.width;
                     cvRect3.height = in.readLine().toInt();
                   // ijk = in.readLine();
              //qdebug() << "cvRect3.height :"<<cvRect3.height;
        ip1=in.readLine();//.toInt();//11425
        ip2=in.readLine();//.toInt();//11425
        ip3=in.readLine();//.toInt();//11425
        ip4=in.readLine();//.toInt();//11425
        mk1=in.readLine();//.toInt();//11425
        mk2=in.readLine();//.toInt();//11425
        mk3=in.readLine();//.toInt();//11425
        mk4=in.readLine();//.toInt();//11425
        gw1=in.readLine();//.toInt();//11425
        gw2=in.readLine();//.toInt();//11425
        gw3=in.readLine();//.toInt();//11425
        gw4=in.readLine();//.toInt();//11425
        username=in.readLine();//11425
        password=in.readLine();//11425
                 for(int i=0;i<4;i++)
                 {
                   sample_calib_name[i]= in.readLine();
                   sample_calib_rgl[i] = in.readLine().toFloat();
                   sample_calib_rgh[i] = in.readLine().toFloat();
                    qDebug()<<"sample_calib_rgl"<<sample_calib_rgl[i];
                    qDebug()<<"sample_calib_rgh"<<sample_calib_rgh[i];
                 }
                for(int i=0;i<4;i++)
               {
                    final_sample_avg_ini_calib[i]=in.readLine().toFloat();
                    final_sample_avg_final_calib[i]=in.readLine().toFloat();
                }
                avg_offset=in.readLine().toFloat();
                avg_mulfactor=in.readLine().toFloat();
                 for(int i=0;i<6;i++)
                 {
                     thermo_ramp_rate[i]=in.readLine().toFloat();
        }
        No_ramp_cycles=in.readLine().toInt();
        thermo_chemical_name=in.readLine();
        thermo_start_temp=in.readLine().toFloat();
        thermo_stop_temp=in.readLine().toFloat();
        thermo_correction_factor=in.readLine().toFloat();
        final_Noofsamples=in.readLine().toInt();
        for(int i=0;i<11;i++)
        {
            user_name[i]=in.readLine();//11425
            user_password[i]=in.readLine();//11425
        }
        calib_date=in.readLine();
        area_x =in.readLine().toInt();
        area_y =in.readLine().toInt();
        serial_no=in.readLine();
        equ_id=in.readLine();
        sl_date_flag=in.readLine().toInt();
        calib_days=in.readLine().toInt();//vinay
        calib_usr=in.readLine().toInt();
        calib_due_date=in.readLine();
        Company_name=in.readLine();//14547
        company_add1=in.readLine();
        company_add2=in.readLine();
        company_add3=in.readLine();
        left_tag=in.readLine();
        right_tag=in.readLine();//14547
        model_no=in.readLine().toInt();//14547
        calib_alert_date=in.readLine();//vinay
        factory_calib=in.readLine().toInt();
        field_calib=in.readLine().toInt();
        calib_cmplt=in.readLine().toInt();
        operator_name=in.readLine();
         for(int i=0;i<4;i++)
         {
          Mmp_fac[i]=in.readLine().toFloat();
          cmp_fac[i]=in.readLine().toFloat();
         }
         for(int i=0;i<3;i++)
          {
           mfactory[i]=in.readLine().toFloat();
           cfactory[i]=in.readLine().toFloat();
          }
         for(int i=0;i<4;i++)
          {
             cfield[i]=in.readLine().toFloat();
           }
         for(int i=0;i<4;i++)
         {
          calib_measure_values[i]=in.readLine();
         }

         calib_factory_date=in.readLine();
         calib_factory_months=in.readLine().toInt();
         calib_factory_duedate=in.readLine();
         for(int i=0;i<4;i++)
         {
          cfield_past[i]=in.readLine().toFloat();
         }

        }

    file.close();
   /* for(int i=0;i<4;i++)
    {
        sample_calib_rgl[i]=final_sample_calib_rgl[i];
        sample_calib_rgh[i]=final_sample_calib_rgh[i];
        sample_avg_ini_calib[i]=final_sample_avg_ini_calib[i];
        sample_avg_final_calib[i]=final_sample_avg_final_calib[i];
    }*/
    for(int i=0;i<final_Noofsamples-1;i++)
        {
        qDebug()<<"mfactory"<<i<<mfactory[i];
        qDebug()<<"cfactory"<<i<<cfactory[i];
        }
        for(int i=0;i<final_Noofsamples;i++)
        {
             qDebug()<<"cfield_past"<<i<<cfield_past[i];
             qDebug()<<"Mmp_fac"<<i<<Mmp_fac[i];
        }

    Noofsamples=final_Noofsamples;
    temp_model_no=model_no;//14547
    ui->lineEdit_90->setText(QString::number(Noofsamples));
    ui->lineEdit_36->setText(serial_no);
    ui->lineEdit_70->setText(equ_id);
    ui->lineEdit_56->setText(Company_name);
    ui->lineEdit_58->setText(company_add1);
    ui->lineEdit_60->setText(company_add2);
    ui->lineEdit_66->setText(company_add3);
    ui->lineEdit_68->setText(left_tag);
    ui->lineEdit_69->setText(right_tag);

    if(sl_date_flag==1)
    {
        ui->lineEdit_36->setEnabled(false);
        //ui->lineEdit_37->setEnabled(false);
    }
    thermo_sample_display();
    update_password_screen();
    //Done reading profile data.
    qr1.setRect(cvRect1.x,cvRect1.y,cvRect1.width,cvRect1.height);
    qr2.setRect(cvRect2.x,cvRect2.y,cvRect2.width,cvRect2.height);
    qr3.setRect(cvRect3.x,cvRect3.y,cvRect3.width,cvRect3.height);

    update_profile_screen();
    calibration_History();

    connect(this,SIGNAL(send_image_area_para(int,int)),camera1,SLOT(send_image_area_para(int,int)));
    emit send_image_area_para(area_x,area_y);//11425
    //11425

    connect(camera1, SIGNAL(sendImage(QImage)), this, SLOT(receiveImage(QImage)));

    connect(camera1,SIGNAL(sendIntensity(double,double,double)),this,SLOT(updateGray(double,double,double)));
    connect(this, SIGNAL(emit_start_stop_cam(bool)), camera1, SLOT(camera_on_off(bool)));
    emit emit_start_stop_cam(false); // active low state for camera

    connect(camera_timer,SIGNAL(timeout()),camera1,SLOT(loop()));


    connect(this,SIGNAL(call_setup_camera()),camera1,SLOT(setup()));
    connect(this,SIGNAL(call_stop_camera()),camera1,SLOT(stop()));

    connect(this,SIGNAL(emit_update_roi(CvRect,CvRect,CvRect)),camera1,SLOT(update_ROI(CvRect,CvRect,CvRect)));

    connect(this,SIGNAL(call_update_form(Profile*)), profile_form, SLOT(update_form(Profile*)) );
    connect(profile_form, SIGNAL(form_Data(Profile*)), this, SLOT(ok_profile_form(Profile*)));

    connect(this,SIGNAL(send_hist_cmp(bool)),camera1,SLOT(hist_cmp_on_off_c(bool)));

    //connect(this,SIGNAL(send_rtc_values(bool)),camera1,SLOT(receive_rtc_values(bool)));



    emit emit_update_roi(cvRect1,cvRect2,cvRect3);
    for(int i=0;i<11;i++)
        emit send_usr_psw(user_name[i],user_password[i]);//11425
    read_rtc();

    start_temp_mp1=start_temp_mp2=start_temp_mp3=0;
    hide_standard1();
    hide_standard2();
    hide_standard3();
    hide_standard4();
}

void MainWindow::update_profile_screen()
{
    ui->pushButton_39->setText(profiles[0]->Chemical_Name);
    ui->pushButton_40->setText(profiles[1]->Chemical_Name);
    ui->pushButton_41->setText(profiles[2]->Chemical_Name);
    ui->pushButton_42->setText(profiles[3]->Chemical_Name);
    ui->pushButton_43->setText(profiles[4]->Chemical_Name);
    ui->pushButton_44->setText(profiles[5]->Chemical_Name);
    ui->pushButton_45->setText(profiles[6]->Chemical_Name);
    ui->pushButton_46->setText(profiles[7]->Chemical_Name);
    ui->pushButton_47->setText(profiles[8]->Chemical_Name);
    ui->pushButton_48->setText(profiles[9]->Chemical_Name);
    ui->pushButton_49->setText(profiles[10]->Chemical_Name);
    ui->pushButton_50->setText(profiles[11]->Chemical_Name);

    ui->pushButton_4->setText(profiles[0]->Chemical_Name);
    ui->pushButton_5->setText(profiles[1]->Chemical_Name);
    ui->pushButton_6->setText(profiles[2]->Chemical_Name);
    ui->pushButton_7->setText(profiles[3]->Chemical_Name);
    ui->pushButton_8->setText(profiles[4]->Chemical_Name);
    ui->pushButton_9->setText(profiles[5]->Chemical_Name);
    ui->pushButton_10->setText(profiles[6]->Chemical_Name);
    ui->pushButton_11->setText(profiles[7]->Chemical_Name);
    ui->pushButton_12->setText(profiles[8]->Chemical_Name);
    ui->pushButton_13->setText(profiles[9]->Chemical_Name);
    ui->pushButton_14->setText(profiles[10]->Chemical_Name);
    ui->pushButton_15->setText(profiles[11]->Chemical_Name);
}

bool MainWindow::new_profile_check(Profile* check_profile)
{
    QMessageBox msgBox;
    QString errMsg;
    bool error_bit = false;

    int length = check_profile->Chemical_Name.size();
    if (length > 20 || length <=0)
    {
        error_bit = true;
        errMsg.append("Chemical name cannot be empty and must be less than 20 characters in size.\n");
    }

    float start_temp = check_profile->Start_temp;
    if (start_temp > 399.0 || start_temp < 25)
    {
        error_bit = true;
        errMsg.append("Start temperature must be with in the range 25 to 399 degC.\n");
    }

    float stop_temp = check_profile->Stop_temp;
    if (stop_temp > 400.0 || stop_temp < start_temp)
    {
        error_bit = true;
        errMsg.append("Stop temperature must be greater than start temperature and be less than 400degC.\n");
    }

    float ramp_rate = check_profile->Ramp_rate;
    if (ramp_rate > 20.0 || ramp_rate < 0)
    {
        error_bit = true;
        errMsg.append("Ramp rate must be with in the range 0.0 to 20.0 degC/min.\n");
    }

    // set limits for these following variables
    float onset = check_profile->On_set;
    if (onset > 100.0 || onset < 0.0)
    {
        error_bit = true;
        errMsg.append("Onset percentage must be within the range 0 to 100.\n");
    }

    float single = check_profile->Single;
    if (single > 100.0 || single < 0.0 || single < onset )
    {
        error_bit = true;
        errMsg.append("Single percentage must be with in the range 0 to 100 and be greater than onset percentage.\n");
    }
    float clear = check_profile->Clear;
    if (clear > 100.0 || clear < 0.0 || clear < single || clear < onset)
    {
        error_bit = true;
        errMsg.append("Clear percentage must be with in the range 0 to 100, be greater than both onset and single percentages.\n");
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
    }
}

void MainWindow::update_Profile()
{
    // update txt file with profile data
    // this function needs to be called when pushButton is cliked from Add profile screen

    QFile file("/profile/init.txt");
    if(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QTextStream out(&file);
        for(int i=0; i < num_of_profiles; i++)
        {
            out << profiles[i]->Chemical_Name;
            out << "\n";
            out << profiles[i]->Start_temp;
            out << "\n";
            out << profiles[i]->Stop_temp;
            out << "\n";
            out << profiles[i]->Ramp_rate;
            out << "\n";
            out << profiles[i]->On_set;
            out << "\n";
            out << profiles[i]->Single;
            out << "\n";
            out << profiles[i]->Clear;
            out << "\n";
            out << profiles[i]->single_sel;//11425
            out << "\n";//11425
            out << profiles[i]->auto_mode;
            out << "\n";
            out << profiles[i]->manual_mode;
            out << "\n";
        }
        out << p << "\n";
        //out << "\n";
        out << Ti << "\n";
        //out << "\n";
        out << sbt << "\n";
        //out << "\n";
        out << time_cycle << "\n";
       // out << "\n";
        out << SCALE1 << "\n";
       // out << "\n";
        out << ROFFSET1 << "\n";
       // out << "\n";
        out << cvRect1.x <<"\n";
        out << cvRect1.y <<"\n";
        out << cvRect1.width <<"\n";
        out << cvRect1.height <<"\n";
       // out << "\n";
        out << cvRect2.x <<"\n";
        out << cvRect2.y <<"\n";
        out << cvRect2.width <<"\n";
        out << cvRect2.height <<"\n";
        //out << "\n";
        out << cvRect3.x <<"\n";
        out << cvRect3.y <<"\n";
        out << cvRect3.width <<"\n";
        out << cvRect3.height <<"\n";
        //11425
        out << ip1 <<"\n";
        out << ip2 <<"\n";
        out << ip3 <<"\n";
        out << ip4 <<"\n";
        out << mk1 <<"\n";
        out << mk2 <<"\n";
        out << mk3 <<"\n";
        out << mk4 <<"\n";
        out << gw1 <<"\n";
        out << gw2 <<"\n";
        out << gw3 <<"\n";
        out << gw4 <<"\n";
        out<<username<<"\n";
        out<<password<<"\n";
                for(int i=0;i<4;i++)
                {
                   out << sample_calib_name[i];
                   out << "\n";
                   out << sample_calib_rgl[i];
                   out << "\n";
                   out << sample_calib_rgh[i];
                    out << "\n";
                }
               for(int i=0;i<4;i++)
                {
                   out << final_sample_avg_ini_calib[i];
                   out << "\n";
                   out << final_sample_avg_final_calib[i];
                   out << "\n";
                }
               out<<avg_offset<<"\n";
               out<<avg_mulfactor<<"\n";
                //11425
                for(int i=0;i<6;i++)
                {
            out << thermo_ramp_rate[i];
            out << "\n";
        }
        out<<No_ramp_cycles<<"\n";
        out<<thermo_chemical_name<<"\n";
        out<<thermo_start_temp<<"\n";
        out<<thermo_stop_temp<<"\n";
        out<<thermo_correction_factor<<"\n";
        out<<final_Noofsamples<<"\n";
        for(int i=0;i<11;i++)
        {
            out << user_name[i];
            out << "\n";
            out << user_password[i];
            out << "\n";
        }
        out<<calib_date<<"\n";
        out<<area_x<<"\n";
        out<<area_y<<"\n";
        out<<serial_no<<"\n";
        out<<equ_id<<"\n";
        out<<sl_date_flag<<"\n";
        out<<calib_days<<"\n"; //vinay
        out<<calib_usr<<"\n";
        out<<calib_due_date<<"\n";
        out<<Company_name<<"\n";
        out<<company_add1<<"\n";
        out<<company_add2<<"\n";
        out<<company_add3<<"\n";
        out<<left_tag<<"\n";
        out<<right_tag<<"\n";
        out<<model_no<<"\n";
        out<<calib_alert_date<<"\n";
        out<<factory_calib<<"\n";
                 out<<field_calib<<"\n";
                 out<<calib_cmplt<<"\n";
                 out<<operator_name<<"\n";
                for(int i=0;i<4;i++)
                {
                    out << Mmp_fac[i];
                    out << "\n";
                    out << cmp_fac[i];
                    out << "\n";

                }
                for(int i=0;i<3;i++)
                {
                    out << mfactory[i];
                    out << "\n";
                    out << cfactory[i];
                    out << "\n";
                }
                for(int i=0;i<4;i++)
                {
                    out << cfield[i];
                    out << "\n";

                }
                for(int i=0;i<4;i++)
                {
                 out <<calib_measure_values[i];
                 out << "\n";
                }
                out<<calib_factory_date<<"\n"; //vinay
                out<<calib_factory_months<<"\n"; //vinay
                out<<calib_factory_duedate<<"\n"; //vinay
                for(int i=0;i<4;i++)
                {
                 out <<cfield_past[i];
                 out << "\n";
                }

             }


             file.close();
}


void MainWindow::receiveImage(QImage image)
{
    QPainter painter(&image);

    //   //qdebug() << "Start receive image time" << QTime::currentTime()<<"   "<<QTime::currentTime().msec();

    if(ui->tabWidget->currentIndex() != 0)
    {
        painter.setPen(Qt::red);
        painter.drawRect(qr1);
        painter.setPen(Qt::green);
        painter.drawRect(qr2);
        painter.setPen(Qt::blue);
        painter.drawRect(qr3);
        painter.end();
        ui->label_16->setPixmap(QPixmap::fromImage(image));
    }
    else
    {
        if(manual_operation==1)
        {
           ui->label->setPixmap(QPixmap::fromImage(image));
           QString block_res_str;
           painter.setPen(Qt::white);
           painter.setFont(QFont("Arial", 15));
           painter.drawText(60,250,(block_res_str=res_adding(block_temp)));
           painter.drawText(60,250,QString::number(block_temp));
           painter.end();
           ui->label->setPixmap(QPixmap::fromImage(image));

           if(recording)
           {
               // 480000
               ////qdebug() << "image size" <<image.byteCount();//11425
               //fwrite(image.bits(),1,image.byteCount(),ffmpeg);// to improve performace.. hardcode bytecount val
               fwrite(image.bits(),1,480000,ffmpeg);//remember
           }
        }
        else
        {
        //***************03-07-14*******************/
        // //qdebug() << "start time" <<QTime::currentTime().toString();
        QImage image1(cvRect1.width,cvRect1.height, QImage::Format_RGB32);
        image1 = image.copy(cvRect1.x,cvRect1.y,cvRect1.width,cvRect1.height);
        QImage image2(cvRect2.width,cvRect2.height,QImage::Format_RGB32);
        image2 = image.copy(cvRect2.x,cvRect2.y,cvRect2.width,cvRect2.height);
        QImage image3(cvRect3.width,cvRect3.height,QImage::Format_RGB32);
        image3 = image.copy(cvRect3.x,cvRect3.y,cvRect3.width,cvRect3.height);
        image.fill(1);//black colour
        painter.drawImage(QRect(qr1),image1);   // Image---Logo
        painter.drawImage(QRect(qr2),image2);   // Image---Logo
        painter.drawImage(QRect(qr3),image3);   // Image---Logo*/

        QString block_res_str;
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 15));
        painter.drawText(60,250,(block_res_str=res_adding(block_temp)));
        painter.drawText(60,250,QString::number(block_temp));
        painter.end();
        ui->label->setPixmap(QPixmap::fromImage(image));


        if(recording)
        {
            // 480000
            ////qdebug() << "image size" <<image.byteCount();//11425
            //fwrite(image.bits(),1,image.byteCount(),ffmpeg);// to improve performace.. hardcode bytecount val
            fwrite(image.bits(),1,480000,ffmpeg);//remember
        }


        //        if(hist_cmp)
        //            sendImageX(image);
    }
    //   //qdebug() << "End receive image time" << QTime::currentTime()<<"   "<<QTime::currentTime().msec();
    }

}
MainWindow::~MainWindow()
{
    delete ui;
    //spi_timer->stop();
    //  close(fd_spi);
}

void MainWindow::on_pushButton_39_clicked() // edit profile 0 button
{
    update_profile_form(0);
    profile_shown = 0;
    profile_form->show();
    profile_form->raise();
}

void MainWindow::on_pushButton_40_clicked() // edit profile 0 button
{
    update_profile_form(1);
    profile_shown = 1;
    profile_form->show();
    profile_form->raise();
}

void MainWindow::on_pushButton_41_clicked() // edit profile 0 button
{
    update_profile_form(2);
    profile_shown = 2;
    profile_form->show();
    profile_form->raise();
}

void MainWindow::on_pushButton_42_clicked() // edit profile 0 button
{
    update_profile_form(3);
    profile_shown = 3;
    profile_form->show();
    profile_form->raise();
}

void MainWindow::on_pushButton_43_clicked() // edit profile 0 button
{
    update_profile_form(4);
    profile_shown = 4;
    profile_form->show();
    profile_form->raise();
}

void MainWindow::on_pushButton_44_clicked() // edit profile 0 button
{
    update_profile_form(5);
    profile_shown = 5;
    profile_form->show();
    profile_form->raise();
}

void MainWindow::on_pushButton_45_clicked() // edit profile 0 button
{
    update_profile_form(6);
    profile_shown = 6;
    profile_form->show();
    profile_form->raise();
}

void MainWindow::on_pushButton_46_clicked() // edit profile 0 button
{
    update_profile_form(7);
    profile_shown = 7;
    profile_form->show();
    profile_form->raise();
}

void MainWindow::on_pushButton_47_clicked() // edit profile 0 button
{
    update_profile_form(8);
    profile_shown = 8;
    profile_form->show();
    profile_form->raise();
}

void MainWindow::on_pushButton_48_clicked() // edit profile 0 button
{
    update_profile_form(9);
    profile_shown = 9;
    profile_form->show();
    profile_form->raise();
}

void MainWindow::on_pushButton_49_clicked() // edit profile 0 button
{
    update_profile_form(10);
    profile_shown = 10;
    profile_form->show();
    profile_form->raise();
}

void MainWindow::on_pushButton_50_clicked() // edit profile 0 button
{
    update_profile_form(11);
    profile_shown = 11;
    profile_form->show();
    profile_form->raise();
}

void MainWindow::update_profile_form(int i)
{
    emit call_update_form(profiles[i]);
}

void MainWindow::ok_profile_form(Profile* temp_profile) // will be called from edit profile upon pressing OK.
{


    bool form_validated = new_profile_check(temp_profile);

    if(form_validated)
    {
        profiles[profile_shown]->Chemical_Name = temp_profile->Chemical_Name;
        profiles[profile_shown]->Start_temp = temp_profile->Start_temp;
        profiles[profile_shown]->Stop_temp = temp_profile->Stop_temp ;
        profiles[profile_shown]->Ramp_rate = temp_profile->Ramp_rate;
        profiles[profile_shown]->On_set = temp_profile->On_set;
        profiles[profile_shown]->Single = temp_profile->Single;
        profiles[profile_shown]->Clear =  temp_profile->Clear;
        profiles[profile_shown]->single_sel =  temp_profile->single_sel;//11425
        profiles[profile_shown]->auto_mode = temp_profile->auto_mode;
        profiles[profile_shown]->manual_mode =temp_profile->manual_mode;
        update_Profile();
        qDebug()<<"in ok profile form"<<temp_profile->manual_mode;
        if(temp_profile->manual_mode)
        manual_operation=1;
        else
        manual_operation=0;
         // updates the ini.txt file
        update_profile_screen();
        profile_form->close();
    }
    else
    {
        QMessageBox msgBox;
        QString errMsg;
        errMsg.append("Low value should be greater than or equal to 0.\n");
        msgBox.critical(0,"Error",errMsg);
        msgBox.setFixedSize(500,200);
    }


}



void MainWindow::on_pushButton_clicked() // temperature sensor calibration
{
    QMessageBox msgBox;
    QString errMsg;
    float tempr1,actual_high_value, actual_low_value,measured_low_high_value;
    bool err_bit = false;
    bool exit_loop = false;
    static int step_flag=0;
    float low_value = ui->lineEdit->text().toFloat();

    float high_value = ui->lineEdit_2->text().toFloat();
    QString str1,str2;
    str1=ui->lineEdit->text();
    str2=ui->lineEdit_2->text();
    if(str1[0]=='\0')
    {
        errMsg.append("Please enter Low value!\n");
        err_bit = true;
    }
    else
    {
        if(low_value < -5.0)
        {
            errMsg.append("Low value be greater than 0!\n");
            err_bit = true;
        }
    }
    if(str2[0]=='\0')
    {
        errMsg.append("Please enter High value!\n");
        err_bit = true;
    }
    else
    {
        if(high_value > 405)
        {
            errMsg.append("High value must be less than 400!\n");
            err_bit = true;
        }
    }
    if (low_value > high_value)
    {
        errMsg.append("High value must be greater than the Low value\n");
        err_bit = true;
    }

    if(err_bit)
    {
        msgBox.critical(0,"Error",errMsg);
        msgBox.setFixedSize(500,200);
    }
    else
    {
        msgBox.setText("Step 1 : Disconnect the internal sensor.");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        switch (ret)
        {
        case QMessageBox::Ok:
            // Save was clicked
            break;
        case QMessageBox::Cancel:
            exit_loop = true;
            break;
        default:
            // should never be reached
            break;
        }

        if(!exit_loop)
        {

            msgBox.setText("Step 2 : Connect the calibrator.");
            int ret = msgBox.exec();
            switch (ret)
            {
            case QMessageBox::Ok:
                // Save was clicked
                break;
            case QMessageBox::Cancel:
                exit_loop = true;
                break;
            default:
                // should never be reached
                break;
            }
        }

        if(!exit_loop)
        {

            msgBox.setText("Step 3 : Set calibrator value to low value.");
check_low:
            int ret = msgBox.exec();
            switch (ret)
            {
            case QMessageBox::Ok:
                measured_low_high_value=ui->label_17->text().toFloat();
                actual_low_value = ui->lineEdit->text().toFloat();
                if((measured_low_high_value>=(actual_low_value-5.0))&&(measured_low_high_value<=(actual_low_value+5.0)))
                {
                    if(ADCnorm1 < 0.7)
                    {
                        ADCprevcal1 = ADCnorm1;
                        //qdebug() <<"ADC preCAL1" << ADCprevcal1;
                    }
                }
                else
                {
                    msgBox.setText("Input OverRange\n");
                    goto check_low;
                }
                break;
            case QMessageBox::Cancel:
                exit_loop = true;
                break;
            default:
                // should never be reached
                break;
            }
        }

        if(!exit_loop)
        {

            msgBox.setText("Step 4 : Set calibrator value to high value.");
check_high:
            int ret = msgBox.exec();
            switch (ret)
            {
            case QMessageBox::Ok:
                actual_high_value = ui->lineEdit_2->text().toFloat();
                measured_low_high_value=ui->label_17->text().toFloat();
                if((measured_low_high_value>=(actual_high_value-5.0))&&(measured_low_high_value<=(actual_high_value+5.0)))
                {
                    if((ADCnorm1<0.7)&&(ADCnorm1>ADCprevcal1))
                    {
                        ADCcal1 = ADCnorm1;
                        //qdebug() <<"ADC CAL1" << ADCcal1;
                    }
                }
                else
                {
                    msgBox.setText("Input OverRange\n");
                    goto check_high;
                }
                break;
            case QMessageBox::Cancel:
                exit_loop = true;
                break;
            default:
                // should never be reached
                break;
            }
        }


        if(!exit_loop)
        {
            tempr1 = actual_low_value;
            if(tempr1>0)
            {
                Rprevcal1=100.0*(1.0+(0.0039083*tempr1)+((-0.0000005775)*(tempr1*tempr1)));
            }
            else
            {
                Rprevcal1=100.0*(1.0+(A*tempr1)+(B*(tempr1*tempr1))+(C*(tempr1-100.0)*(tempr1*tempr1*tempr1)));
            }
            tempr1=actual_high_value;
            if(tempr1>0)
            {
                Rcal1=100.0*(1.0+(0.0039083*tempr1)+((-0.0000005775)*tempr1*tempr1));
            }
            else
            {
                Rcal1=100.0*(1.0+(A*tempr1)+(B*(tempr1*tempr1))+(C*(tempr1-100.0)*(tempr1*tempr1*tempr1)));
            }
            //qdebug()<<"in calibration";
            SCALE1=(Rcal1-Rprevcal1)/(ADCcal1-ADCprevcal1);
            ROFFSET1=Rprevcal1-(ADCprevcal1*SCALE1);
            update_Profile();
            QMessageBox::information(this,"User Alert","Temperature sensor calibration Completed sucessfully.");

           /* msgBox.setText("Temperature sensor calibration complete.");
            msgBox.setStandardButtons(QMessageBox::Ok );*/
            step_flag=0;
        }
        else
        {
            msgBox.setText("Temperature sensor calibration aborted!.");
            msgBox.setStandardButtons(QMessageBox::Ok );
            step_flag=0;
        }
    }
}

bool MainWindow::check_setup_params()
{
    QMessageBox msgBox;
    QString errMsg;
    bool error_bit = false;

    float p1 = ui->lineEdit_4->text().toFloat();
    if(p1<=0 || p1>100)
    {
        error_bit = true;
        errMsg.append("Proportional constant out of range!");
    }

    float i1 = ui->lineEdit_5->text().toFloat();
    if(i1 <= 0 || i1 > 100)
    {
        error_bit = true;
        errMsg.append("Integral constant out of range!");
    }
    //float d1 = ui->lineEdit_6->text().toFloat(); // we currently dont use d1

    float sbt1 = ui->lineEdit_7->text().toFloat();
    if(sbt1 <=0 || sbt1 >100)
    {
        error_bit = true;
        errMsg.append("sbt constant out of range!");
    }
    int time_cycle1 = ui->lineEdit_8->text().toInt();
    if(time_cycle1<=0 || time_cycle1>1000)
    {
        error_bit = true;
        errMsg.append("time cycle constant out of range!");
    }

    if(error_bit)
    {
        msgBox.critical(0,"Error",errMsg);
        msgBox.setFixedSize(500,200);
        return false;
    }
    return true;

}

void MainWindow::on_pushButton_18_clicked() // pre heat button
{
    customPlot->graph(0)->clearData();
    customPlot->graph(1)->clearData();
    customPlot->graph(2)->clearData();                     //vinay
    ui->sample1_label2->text().clear();
    ui->sample1_label5->text().clear();
    ui->sample2_label2->text().clear();
    ui->sample2_label5->text().clear();
    ui->sample3_label2->text().clear();
    ui->sample3_label5->text().clear();

    if (full_percentage >= 90)    //check2 sdcard checkup
          {
             QMessageBox::information(this,"User Alert","Memory Full:To continue, ensure sufficient Disk Space");
             ui->pushButton_18->setEnabled(true);
          }

else
{


    temp_timer->stop();
    QMessageBox msgBox;
    QString errMsg,batchno;
    // verfiy validity of all parametes
    curr_profile->Chemical_Name = ui->lineEdit_25->text();
    curr_profile->Start_temp = ui->lineEdit_26->text().toFloat();
    curr_profile->Stop_temp = ui->lineEdit_27->text().toFloat();
    curr_profile->Ramp_rate = ui->lineEdit_28->text().toFloat();
    curr_profile->On_set = ui->lineEdit_29->text().toFloat();
    curr_profile->Single = ui->lineEdit_31->text().toFloat();
    curr_profile->Clear = ui->lineEdit_30->text().toFloat();
    curr_profile->auto_mode= manual_operation;
    curr_profile->manual_mode=manual_operation;

    bool form_validated = new_profile_check(curr_profile);
    bool setup_param_valid = check_setup_params();


    batchno=ui->lineEdit_35->text();
    // we need to check all setup paramenters such as pidbsp timecuycle for correctness before we start
    if(!form_validated || !setup_param_valid)
    {
        errMsg.append("Cannot start!!");
        msgBox.critical(0,"Error",errMsg);
        msgBox.setFixedSize(500,200);
        return;
    }
    else if((batchno=='\0'))
    {
        errMsg="Enter The Batch Number";
        msgBox.critical(0,"Error",errMsg);
        msgBox.setFixedSize(500,200);
        return;
    }
    else if(!index5.isValid())
    {
        errMsg="Memory card is not inserted";
        msgBox.critical(0,"Error",errMsg);
        msgBox.setFixedSize(500,200);
        return;
    }
    else
    {
        start_temp = curr_profile->Start_temp;
        end_temp = curr_profile->Stop_temp;
        ramp_rate = curr_profile->Ramp_rate;
        //qdebug() << "Inside Pre heat set point";
        //  thread_spi->quit();
        ////qdebug() << "thread status : " <<  ttt;
        //ramp_rate = 20;

        start=false;//pid cntrl after end temp 14547
        end =false;
        fanon=true;
        fan=false;
        preheat = true;
       disconnect(temp_timer,SIGNAL(timeout()),this,SLOT(cool_mode()));
        emit send_setpoint(start_temp,start_temp,end_temp,(start_temp/10),p,((Ti-5)+(18-((8/350)*start_temp))),sbt,time_cycle,false);
        emit send_pid_signal(true);
        emit send_pwm_signal(true);
        emit send_hist_cmp(false);
        emit update_status(2);
        emit sbt_cntrl(true);//14547
        emit fan_setpoint(end_temp);
        emit pid_strt(false);

        ui->pushButton_18->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_16->setEnabled(true);
        ui->lineEdit_35->setEnabled(false);



        mainscreen();//11425


        if(!camera_timer->isActive())
        {

            emit call_setup_camera();

            camera_timer->start(100);

        }
        sample_1_onset_detected_bool = false;

        sample_2_onset_detected_bool = false;

        sample_3_onset_detected_bool= false;


        sample_1_single_detected_bool= false;

        sample_2_single_detected_bool= false;

        sample_3_single_detected_bool= false;


        sample_1_clear_detected_bool= false;

        sample_2_clear_detected_bool= false;

        sample_3_clear_detected_bool= false;

        if(manual_operation==1)
        {
            man1=man2=man3=0;
            ui->Initial_melt1->show();
            ui->Initial_melt2->show();
            ui->Initial_melt3->show();

            ui->Initial_melt1->setText("Intial Melt1");
            ui->Initial_melt1->setStyleSheet("QPushButton {color: grey;}");
            ui->Initial_melt2->setText("Intial Melt2");
            ui->Initial_melt2->setStyleSheet("QPushButton {color: grey;}");
            ui->Initial_melt3->setText("Intial Melt3");
            ui->Initial_melt3->setStyleSheet("QPushButton {color: grey;}");

            ui->Initial_melt1->setEnabled(false);
            ui->Initial_melt2->setEnabled(false);
            ui->Initial_melt3->setEnabled(false);


            manual_strt1=manual_strt2=manual_strt3=0;
            manual_end1=manual_end2=manual_end3=0;
        }

    }
    stop_button_flag = false;
}

}


void MainWindow::start_temp_reached()
{

    buzzer_on();
    beep_timer->singleShot(1500, this, SLOT(buzzer_off()));
//   beep_timer->start(2000);
    fan=false;

    emit fan_setpoint(end_temp);
    emit start_melting(false);

    if((fanon==true)&&(preheat==false))
    {

        temp_timer->start(900000);
        connect(temp_timer,SIGNAL(timeout()),this,SLOT(cool_mode()));
    }

    emit update_status(0);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_16->setEnabled(true);

}

void MainWindow::end_temp_reached()
{
    buzzer_on();

     beep_timer->singleShot(1500, this, SLOT(buzzer_off()));
     melt1=melt2=melt3=false;
    end=false;
    fan=true;
    start=true;//pid cntrl after end temp 14547

       stop_button_flag = false;
       ui->Initial_melt1->hide();
       ui->Initial_melt2->hide();
       ui->Initial_melt3->hide();
        emit send_setpoint(start_temp,start_temp,end_temp,(start_temp/10),p,((Ti-5)+(18-((8/350)*start_temp))),sbt,time_cycle,false);
        emit send_pid_signal(true);// required for enabling start button after melting process completed
        emit send_pwm_signal(true);
        emit fan_setpoint(start_temp);
        emit send_Stop_Recording();
        emit update_status(3);
        emit send_hist_cmp(false);
        emit sbt_cntrl(false);//14547
        emit start_melting(true);

        ui->pushButton_18->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_16->setEnabled(true);
        ui->label->clear();
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("User Alert");
        msgBox.setText("Melting process completed.\nPress Ok to save the data.\nPress cancel to discard the data.\n");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();

        switch (ret)
        {
         case QMessageBox::Ok:
         save_Results();
         break;
         case QMessageBox::Cancel:
         conformation_delete_data();
         break;
         default:
                                  // should never be reached
          break;
        }



                               // if ack then cool down singal to be sent. use split ranger

}
void MainWindow::on_pushButton_2_clicked() //start melt button
{

    temp_array_final.clear();
    sample1_norm.clear();
    sample2_norm.clear();
    sample3_norm.clear();
    new_sample1.clear();
    new_sample2.clear();
    new_sample3.clear();
    new_sample1_slope.clear();
    new_sample2_slope.clear();
    new_sample3_slope.clear();
    new_temp.clear();
    end=false;
    fan=false;
    start=false;//pid cntrl after end temp 14547

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("User Alert");
    msgBox.setText("Have you inserted the samples?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = msgBox.exec();
    switch (ret)
    {
    case QMessageBox::Ok:

        emit send_setpoint(end_temp,start_temp,end_temp,ramp_rate,p,((Ti-5)+(18-((8/350)*start_temp))),0.1,time_cycle,true);
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_18->setEnabled(false);
        if(manual_operation==1)
        {
        ui->Initial_melt1->show();
        ui->Initial_melt2->show();
        ui->Initial_melt3->show();
        ui->Initial_melt1->setEnabled(true);
        ui->Initial_melt1->setText("Initial Melt1");
        ui->Initial_melt1->setStyleSheet("QPushButton {color: red;}");
        ui->Initial_melt2->setEnabled(true);
        ui->Initial_melt2->setText("Initial Melt2");
        ui->Initial_melt2->setStyleSheet("QPushButton {color: green;}");
        ui->Initial_melt3->setEnabled(true);
        ui->Initial_melt3->setText("Initial Melt3");
        ui->Initial_melt3->setStyleSheet("QPushButton {color: blue;}");
        }

        emit send_hist_cmp(true);
        emit pid_strt(true);
        temp_timer->stop();
        fanon=true;
        preheat=false;
        stop_button_flag = true;

        if(camera_timer->isActive())
        {
            camera_timer->stop();
            emit call_stop_camera();
        }


        ui->pushButton_2->setEnabled(false);
        ui->single_sel_button->setEnabled(false);

        emit update_status(1);


        //11425time_array.clear();

        // --added by dhana-------



        one_time_flag = false; //for finding the min & max samples

        emit send_hist_cmp(true); //  do we need this now?

        if(!camera_timer->isActive())
        {

            emit call_setup_camera();

            camera_timer->start(500);

        }
        if(recording == false)
            emit send_Start_Recording(); //enable this
        //    }
        break;

    case QMessageBox::Cancel:

        ui->pushButton_2->setEnabled(true);
        ui->pushButton_16->setEnabled(true);

        break;
    default:
        // should never be reached
        break;
    }
}

void MainWindow::on_pushButton_16_clicked() // stop button
{
        end=true;
        fanon=true;

        start=false;//pid cntrl after end temp 14547
        if(preheat==true)
        {
            emit sbt_cntrl(true);//14547
        }
        else
        {
            emit sbt_cntrl(false);
        }
        emit pid_strt(false);
        emit send_setpoint(40,start_temp,end_temp,ramp_rate,p,Ti,sbt,time_cycle,false);
        emit send_pid_signal(false);
        emit send_pwm_signal(false);//14547
        emit fan_setpoint(40);
        emit send_hist_cmp(false);

        //11425
        ui->pushButton_18->setEnabled(true);//pre heat
        ui->pushButton_2->setEnabled(false);//start
        ui->pushButton_16->setEnabled(false);//stop
        ui->lineEdit_35->clear();//11425

        if(camera_timer->isActive())
        {
            camera_timer->stop();
            emit call_stop_camera();
        }


        emit update_status(3);
        // pid_timer->stop();

        if(recording)
            emit send_Stop_Recording();
        ui->Initial_melt1->hide();
        ui->Initial_melt2->hide();
        ui->Initial_melt3->hide();
        melt1=melt2=melt3=false;
        // CAN WE DO THIS LATER?
        ui->pushButton_18->setEnabled(true);
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_16->setEnabled(false);
        ui->lineEdit_35->setEnabled(true);//11425
        ui->tabWidget->setTabEnabled(1,true);
        ui->tabWidget->setTabEnabled(2,true);
        ui->tabWidget->setTabEnabled(3,true);
        ui->tabWidget->setTabEnabled(4,true);
        ui->tabWidget->setTabEnabled(5,true);
        if(screen == true)//14547 07-10-14
        {
            //qdebug()<<"hello";
            ui->single_sel_button->setEnabled(true);
            ui->lineEdit_25->setEnabled(true);
            ui->lineEdit_26->setEnabled(true);
            ui->lineEdit_27->setEnabled(true);
            ui->lineEdit_28->setEnabled(true);
            ui->lineEdit_29->setEnabled(true);
            ui->lineEdit_31->setEnabled(true);
            ui->lineEdit_30->setEnabled(true);
            ui->manual->setEnabled(true);
            ui->automatic->setEnabled(true);
            ui->manual->setChecked(false);
            ui->automatic->setChecked(false);


        }
        else
        {
            //qdebug()<<"hello11";
            ui->single_sel_button->setEnabled(false);
            ui->lineEdit_25->setEnabled(false);
            ui->lineEdit_26->setEnabled(false);
            ui->lineEdit_27->setEnabled(false);
            ui->lineEdit_28->setEnabled(false);
            ui->lineEdit_29->setEnabled(false);
            ui->lineEdit_31->setEnabled(false);
            ui->lineEdit_30->setEnabled(false);
            ui->manual->setEnabled(false);
            ui->automatic->setEnabled(false);

        }
        //model->refresh(model->index("/sdcard/results"));

        model->refresh(model->index(storedevice));
        ui->label->clear();

        emit send_hist_cmp(false);

        QFile().remove(filename);
        QDir().rmdir(filename11);
        if(calib_flag == true)
        {
            //ui->tabWidget->setTabEnabled(2,false);
           // ui->tabWidget_8->setCurrentIndex(2);

            hiding_calib(!calib_flag);
            calib_flag=false;
            stop_check_calib=true;
            qDebug()<< "insideeeeeeeeeeeeee";
           if(ui->tabWidget->currentIndex() != 2)
                //ui->tabWidget_8->setCurrentIndex(2);
                ui->tabWidget->setCurrentIndex(4);



        }

}


void MainWindow::shutdown()
{
    emit send_pid_signal(false);
    emit send_pwm_signal(false);
    QString command;
    command = "echo \"high\" > /sys/class/gpio/gpio160/direction";

    system(qPrintable(command));

    command = "echo \"0\" > /sys/class/gpio/gpio160/value";

    system(qPrintable(command));
    QMessageBox::information(this,"User Alert"," System will shutdown");
    system("/sbin/shutdown");


}

void MainWindow::update_mw_params(int i)
{
    ui->tabWidget->setCurrentIndex(0);
    QString str;
    bool single_sel;//11425
    ui->lineEdit_25->setText(profiles[i]->Chemical_Name);

    str = QString::number(profiles[i]->Start_temp);
    ui->lineEdit_26->setText(str);

    str = QString::number(profiles[i]->Stop_temp);
    ui->lineEdit_27->setText(str);

    str = QString::number(profiles[i]->Ramp_rate);
    ui->lineEdit_28->setText(str);

    str = QString::number(profiles[i]->On_set);
    ui->lineEdit_29->setText(str);

    str = QString::number(profiles[i]->Single);
    ui->lineEdit_31->setText(str);

    str = QString::number(profiles[i]->Clear);
    ui->lineEdit_30->setText(str);
    single_sel=profiles[i]->single_sel;//11425
    ui->lineEdit_29->setEnabled(!single_sel);//11425
    ui->lineEdit_30->setEnabled(!single_sel);//11425
    ui->lineEdit_31->setEnabled(single_sel);//11425
    ui->single_sel_button->setChecked(single_sel);//11425
    onset_clear = single_sel;
    ui->manual->setChecked(profiles[i]->manual_mode);
    ui->automatic->setChecked(profiles[i]->auto_mode);

    qDebug()<<"in available profiles"<<profiles[i]->manual_mode;
    if(profiles[i]->manual_mode)
    manual_operation=1;
    else
    manual_operation=0;

    if(screen==true)
    {
        ui->lineEdit_31->setEnabled(true);
        ui->lineEdit_30->setEnabled(true);
        ui->lineEdit_29->setEnabled(true);
        ui->manual->setEnabled(true);
        ui->automatic->setEnabled(true);
    }
    else
    {
        ui->lineEdit_31->setEnabled(false);
        ui->lineEdit_30->setEnabled(false);
        ui->lineEdit_29->setEnabled(false);
        ui->manual->setEnabled(false);
        ui->automatic->setEnabled(false);
    }

}

void MainWindow::on_pushButton_4_clicked() // load from profile[i] to curr_profile 08942288414
{
    update_mw_params(0); // we also have to shift focus to firs tab. how do we do that?
}

void MainWindow::on_pushButton_5_clicked() // load from profile[i] to curr_profile
{
    update_mw_params(1);
}

void MainWindow::on_pushButton_6_clicked() // load from profile[i] to curr_profile
{
    update_mw_params(2);
}

void MainWindow::on_pushButton_7_clicked() // load from profile[i] to curr_profile
{
    update_mw_params(3);
}

void MainWindow::on_pushButton_8_clicked() // load from profile[i] to curr_profile
{
    update_mw_params(4);
}

void MainWindow::on_pushButton_9_clicked() // load from profile[i] to curr_profile
{
    update_mw_params(5);
}

void MainWindow::on_pushButton_10_clicked() // load from profile[i] to curr_profile
{
    update_mw_params(6);
}

void MainWindow::on_pushButton_11_clicked() // load from profile[i] to curr_profile
{
    update_mw_params(7);
}

void MainWindow::on_pushButton_12_clicked() // load from profile[i] to curr_profile
{
    update_mw_params(8);
}

void MainWindow::on_pushButton_13_clicked() // load from profile[i] to curr_profile
{
    update_mw_params(9);
}

void MainWindow::on_pushButton_14_clicked() // load from profile[i] to curr_profile
{
    update_mw_params(10);
}

void MainWindow::on_pushButton_15_clicked() // load from profile[i] to curr_profile
{
    update_mw_params(11);
}


/*-------------------------------------------------------------------------------------*/
//graph of intensity code

void MainWindow::updateGray(double sample_1, double sample_2, double sample_3 )
{


    float temp = block_temp;

    sample1_norm.append(sample_1); // need to clear these arrarys at appropriate places
    sample2_norm.append(sample_2);
    sample3_norm.append(sample_3);
    temp_array_final.append(temp);

}

void MainWindow::on_pushButton_17_clicked() // update pid params from setup
{
    p = ui->lineEdit_4->text().toFloat();
    Ti = ui->lineEdit_5->text().toFloat();
    d = ui->lineEdit_6->text().toFloat();
    sbt = ui->lineEdit_7->text().toFloat();
    time_cycle = ui->lineEdit_8->text().toInt();
    update_Profile();
    QMessageBox::information(this,"User Alert","Data saved");
    //    if(pid_timer->isActive())
    //        pid_timer->start(time_cycle);
    //    spi_timer->start(time_cycle);
}





void MainWindow::pdf_report()
{
    QTextDocument doc;
    doc.setHtml( "<p>A QTextDocument can be used to present formatted text "
                 "in a nice way.</p>"
                 "<p align=center>It can be <b>formatted</b> "
                 "<font size=+2>in</font> <i>different</i> ways.</ p>"
                 "<p>The text can be really long and contain many "
                 "paragraphs. It is properly wrapped and such...</p>" );
    if(calib_pdf_report==false)
    {
        filename_pdf.clear();
        // filename_pdf.append("/sdcard/results/");//remember 14547
        //filename_pdf.append("/udisk/results/");//manual checking
        filename_pdf.append(filename);//remember to uncomment
        filename_pdf.append("Melt");
        filename_pdf.append("_Report");
        filename_pdf.append(".pdf");
        //qdebug()<<"melt title";

    }

    else
    {
        QDateTime dateTime = QDateTime::currentDateTime();
        QString dateTimeString = dateTime.toString(Qt::ISODate);
        dateTimeString.remove(" ");
        dateTimeString.remove("-");
        dateTimeString.remove(":");
        dateTimeString.replace("T","_");//date and time addition in calib report name


        filename_pdf.clear();
        filename_pdf.append("/results/");//remember 14547
        // filename_pdf.append(filename);
        filename_pdf.append(dateTimeString); // Date & Time
        filename_pdf.append("calib");
        filename_pdf.append("_Report");
        filename_pdf.append(".pdf");

        //qdebug()<<"calib title";
    }
    //results_saved = true;
    //customPlot->savePdf(filename);
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFileName(filename_pdf);
    //printer.setOutputFileName("/udisk/report.pdf");
    printer.setOutputFormat(QPrinter::PdfFormat);



    QPainter p( &printer );
    //p.setWindow(0,0,900,900);
    QImage maskk = QImage("/company_logo/logo.PNG");//11425
    QImage graph = QImage(fname);
    p.setPen(Qt::black);
    p.setRenderHints(QPainter::Antialiasing,false);
    p.setRenderHints(QPainter::TextAntialiasing,false);
    p.setRenderHints(QPainter::SmoothPixmapTransform,false);
    p.setFont(QFont("Arial",10));


    /*********************14547***********************/
    p.setFont(QFont("Arial",14,QFont::Bold));//14547

    p.drawText(QRect(60, 0,8600,350) , Qt::AlignCenter, Company_name);
    p.setFont(QFont("Arial",10));
    p.drawText( QRect(60, 380,8600,180), Qt::AlignCenter, company_add1);
    p.drawText( QRect(60,580, 8600,180), Qt::AlignCenter, company_add2);
    p.drawText( QRect(60,780,8600,180), Qt::AlignCenter, company_add3);
    int y_var1;
    y_var1=50;



      smallest=0;
      largest=0;



    /**************14547*******************************/

    int width_var;
    if(calib_pdf_report==false)
    {


        //qdebug()<<"melt report";
        p.setRenderHints(QPainter::Antialiasing,false);
        p.setRenderHints(QPainter::TextAntialiasing,false);
        p.setRenderHints(QPainter::SmoothPixmapTransform,false);
        p.setFont(QFont("Arial",12 ,QFont::Bold));//14547
        p.drawText(QRect(0,y_axis-1050-y_var1,9000,450), Qt::AlignCenter, "Melting Point Report");

        p.setFont(QFont("Arial",12));

        p.drawText(x_axis-50, y_axis-100-y_var1, "Configuration Details:");
        if(manual_operation==1)
         p.drawText(x_axis-50, y_axis+3000-y_var1, "Results:Manual Mode");
        else
        p.drawText(x_axis-50, y_axis+3000-y_var1, "Results:");

        // p.drawText(x_axis-50, y_axis+5500, "Traceability Details:");//14547
        p.drawText(x_axis-50, y_axis+5500-y_var1, "Graph:");
        p.drawText(x_axis-50, y_axis+9000-y_var1, "Remarks:");
        p.drawText(x_axis-50, y_axis+8500-y_var1, "Range:");


        p.setFont(QFont("Arial",10));

        //p.drawImage(QRect(x_axis,y_axis+9500,1200,750),maskk);   // Image---Logo//14547
        p.drawImage(QRect(x_axis+7550,0,1250,1100),maskk);//14547
        p.drawImage(QRect(x_axis+730,y_axis+5500-y_var1,5000,2650),graph);//14547

        if(temp_model_no)//14547
        {
            p.drawText(x_axis+6000, y_axis, "Model No ");
            p.drawText(x_axis+7000, y_axis,"MP98-X-X");
            p.drawText(x_axis+6000, y_axis+400, "Serial No :");
            p.drawText(x_axis+7000, y_axis+400, serial_no);
            p.drawText(x_axis+6800, y_axis,":");
            p.drawText(x_axis+600, y_axis+400,":");
            p.drawText(x_axis+6000, y_axis+800, "Equip ID ");
            p.drawText(x_axis+7000, y_axis+800, equ_id);
            p.drawText(x_axis+6800, y_axis+800,":");

        }

        //-------------In Configuration----------------------------------

        p.drawText(x_axis, y_axis+400-y_var1, "Material Name");//14547 msn requirement
        p.drawText(x_axis+1750, y_axis+400-y_var1, ui->lineEdit_25->text());//11425

        p.drawText(x_axis, y_axis+800-y_var1, "B.No. / A.R.No.");
        p.drawText(x_axis+1750, y_axis+800-y_var1, ui->lineEdit_35->text());

        p.drawText(x_axis, y_axis+1200-y_var1, "Date & Time");      //             p.drawText(QPoint(x_axis+4730, y_axis+1000), "Time");
        p.drawText(x_axis+5230, y_axis+1200-y_var1, "(DD/MM/YY & HH:MM:SS)");//14547

        p.drawText(x_axis, y_axis+1600-y_var1, "Operator Name");//14547
        p.drawText(x_axis+1750, y_axis+1600-y_var1, ui->lineEdit_34->text());//operator name

        p.drawText(x_axis, y_axis+2000-y_var1, "Start Temperature");      p.drawText(x_axis+3100, y_axis+2000-y_var1, "End Temperature");    p.drawText(x_axis+6150, y_axis+2000-y_var1, "Ramp Rate");

        p.drawText(x_axis, y_axis+2400-y_var1, "OnSet");                  p.drawText(x_axis+3100, y_axis+2400-y_var1, "Clear");              p.drawText(x_axis+6150, y_axis+2400-y_var1, "Single");

        p.drawText(x_axis+1630, y_axis+400-y_var1, ":");
        p.drawText(x_axis+1630, y_axis+800-y_var1, ":");

        p.drawText(x_axis+1630, y_axis+1200-y_var1, ":");

        p.drawText(x_axis+1630, y_axis+1600-y_var1, ":");       //  p.drawText(QPoint(x_axis+6230, y_axis+1450), ":");

        p.drawText(x_axis+1630, y_axis+2000-y_var1, ":");         p.drawText(x_axis+4600, y_axis+2000-y_var1, ":");           p.drawText(x_axis+7150, y_axis+2000-y_var1, ":");
        p.drawText(x_axis+1630, y_axis+2400-y_var1, ":");         p.drawText(x_axis+4600, y_axis+2400-y_var1, ":");           p.drawText(x_axis+7150, y_axis+2400-y_var1, ":");




        p.setFont(QFont("ArialBlack",10));
        p.drawText(x_axis+2500, y_axis+2000-y_var1,trUtf8("°C") );
        p.drawText(x_axis+5500, y_axis+2000-y_var1, trUtf8("°C"));
        p.drawText(x_axis+8050, y_axis+2000-y_var1, trUtf8("°C/min"));
        p.drawText(x_axis+2500, y_axis+2400-y_var1, "%");
        p.drawText(x_axis+5500, y_axis+2400-y_var1, "%");
        p.drawText(x_axis+8050, y_axis+2400-y_var1, "%");

        //p.drawText(x_axis+8050, y_axis+2400, "%");

        //-------------In Result ----------------------------------
        p.drawText(QRect(x_axis+730,y_axis+3300-4*(y_var1),width_rect-1000,height_rect+400),Qt::AlignCenter,trUtf8 ("Sample"));
        p.drawText(QRect(x_axis+1730,y_axis+3250-4*(y_var1),width_rect,height_rect+100),Qt::AlignCenter, trUtf8("Initial Melting"));
        p.drawText(QRect(x_axis+1730,y_axis+3450-4*(y_var1),width_rect,height_rect+100),Qt::AlignCenter, trUtf8("Temperature"));
        p.drawText(QRect(x_axis+1730,y_axis+3600-3*(y_var1),width_rect,height_rect+100),Qt::AlignCenter, trUtf8("(°C)"));
        p.drawText(QRect(x_axis+3730,y_axis+3250-4*(y_var1),width_rect,height_rect+100),Qt::AlignCenter, trUtf8("Final Melting"));
        p.drawText(QRect(x_axis+3730,y_axis+3450-4*(y_var1),width_rect,height_rect+100),Qt::AlignCenter, trUtf8("Temperature"));
        p.drawText(QRect(x_axis+3730,y_axis+3600-3*(y_var1),width_rect,height_rect+100),Qt::AlignCenter, trUtf8("(°C)"));
        p.drawText(QRect(x_axis+5730,y_axis+3450-4*(y_var1),width_rect,height_rect+100),Qt::AlignCenter, trUtf8("Melting Range"));
        p.drawText(QRect(x_axis+5730,y_axis+3600-3*(y_var1),width_rect,height_rect+100),Qt::AlignCenter, trUtf8("(°C)"));

        p.drawText(QRect(x_axis+730, y_axis+4000-3*(y_var1),width_rect-1000,height_rect+100),Qt::AlignCenter, "1");
        p.drawText(QRect(x_axis+730, y_axis+4350-3*(y_var1),width_rect-1000,height_rect+100),Qt::AlignCenter, "2");
        p.drawText(QRect(x_axis+730, y_axis+4750-3*(y_var1),width_rect-1000,height_rect+100),Qt::AlignCenter, "3");

        if((((start_temp_mp1-start_temp_mp2)<5)||((start_temp_mp1-start_temp_mp2)<-5))&&(((start_temp_mp2-start_temp_mp3)<5)||((start_temp_mp2-start_temp_mp3)<-5))&&(((start_temp_mp1-start_temp_mp3)<5)||((start_temp_mp1-start_temp_mp3)<-5)))

        {
            smallest=((start_temp_mp1+start_temp_mp2+start_temp_mp3)/(3*1.0));
            largest=((end_temp_mp1+end_temp_mp2+end_temp_mp3)/(3*1.0));
            QString rng,rngstring;
            rng.clear();
            rngstring.clear();
            rngstring=res_adding(smallest);
            rng.append(rngstring);
            rng.append(" - ");
            rngstring=res_adding(largest);
            rng.append(rngstring);
            p.drawText(x_axis+1000, y_axis+8500-y_var1, rng);
        }
        else
        {
            p.drawText(x_axis+1000, y_axis+8500-y_var1, " NA - NA ");
        }
        //--------------------------In start tracebility----------------------------------
        p.setFont(QFont("Arial",10));
        QString received_res_str;//14547


        //-----------------------In Footer----------------------------------
        p.drawText(x_axis, y_axis+9300+10*(y_var1), left_tag);//14547
        p.drawText(x_axis, y_axis+9500+10*(y_var1), "Date:");
        p.drawText(x_axis+5500, y_axis+9300+10*(y_var1), right_tag);//14547
        p.drawText(x_axis+5500, y_axis+9500+10*(y_var1), "Date:");

        //---------------------In End footer----------------------------------
        p.setPen(Qt::red);
        p.drawRect(QRect(0,0,1,1));
        p.setPen(Qt::black);
        p.setFont(QFont("Arial",12,QFont::Bold));

        //------For configuration Rectangles--------------------------------
        p.drawRect(QRect(0,0,9000,12500));

        p.drawRect(QRect(x_axis+1730,y_axis+200-y_var1,width_rect+1300,height_rect));
        p.drawRect(QRect(x_axis+1730,y_axis+600-y_var1,width_rect+1300,height_rect));
        p.drawRect(QRect(x_axis+1730,y_axis+1000-y_var1,width_rect+1300,height_rect));
        p.drawRect(QRect(x_axis+1730,y_axis+1400-y_var1,width_rect+1300,height_rect));

        p.drawRect(QRect(x_axis+1730,y_axis+1800-y_var1,width_rect-1300,height_rect));      p.drawRect(QRect(x_axis+4700,y_axis+1800-y_var1,width_rect-1300,height_rect));       p.drawRect(QRect(x_axis+7300,y_axis+1800-y_var1,width_rect-1300,height_rect));

        p.drawRect(QRect(x_axis+1730,y_axis+2200-y_var1,width_rect-1300,height_rect));      p.drawRect(QRect(x_axis+4700,y_axis+2200-y_var1,width_rect-1300,height_rect));       p.drawRect(QRect(x_axis+7300,y_axis+2200-y_var1,width_rect-1300,height_rect));

        if(temp_model_no)



        p.setPen(Qt::black);
        p.setFont(QFont("Arial",12,QFont::Bold));

        //------For RESULT Rectangles--------------------------------

        p.drawRect(QRect(x_axis+730,y_axis+3300+4*(-y_var1),width_rect-1000,height_rect+400));//sample
        p.drawRect(QRect(x_axis+730,y_axis+4000+4*(-y_var1),width_rect-1000,height_rect+100));
        p.drawRect(QRect(x_axis+730,y_axis+4400+4*(-y_var1),width_rect-1000,height_rect+100));
        p.drawRect(QRect(x_axis+730,y_axis+4800+4*(-y_var1),width_rect-1000,height_rect+100));

        p.drawRect(QRect(x_axis+1730,y_axis+3300+4*(-y_var1),width_rect,height_rect+400));//initial temp
        p.drawRect(QRect(x_axis+1730,y_axis+4000+4*(-y_var1),width_rect,height_rect+100));
        p.drawRect(QRect(x_axis+1730,y_axis+4400+4*(-y_var1),width_rect,height_rect+100));
        p.drawRect(QRect(x_axis+1730,y_axis+4800+4*(-y_var1),width_rect,height_rect+100));

        p.drawRect(QRect(x_axis+3730,y_axis+3300+4*(-y_var1),width_rect,height_rect+400));//final temp
        p.drawRect(QRect(x_axis+3730,y_axis+4000+4*(-y_var1),width_rect,height_rect+100));
        p.drawRect(QRect(x_axis+3730,y_axis+4400+4*(-y_var1),width_rect,height_rect+100));
        p.drawRect(QRect(x_axis+3730,y_axis+4800+4*(-y_var1),width_rect,height_rect+100));

        p.drawRect(QRect(x_axis+5730,y_axis+3300+4*(-y_var1),width_rect,height_rect+400));// melting range
        p.drawRect(QRect(x_axis+5730,y_axis+4000+4*(-y_var1),width_rect,height_rect+100));
        p.drawRect(QRect(x_axis+5730,y_axis+4400+4*(-y_var1),width_rect,height_rect+100));
        p.drawRect(QRect(x_axis+5730,y_axis+4800+4*(-y_var1),width_rect,height_rect+100));



        //_---------------------For Lines---------------------
        p.setPen(Qt::black);
        p.setFont(QFont("Arial",10,QFont::Bold));
        p.drawLine(0,y_axis-1050-y_var1,9000,y_axis-1050-y_var1);//11425 above melting point14547
        p.drawLine(0,y_axis-600-y_var1,9000,y_axis-600-y_var1);//11425 below melting point14547
        p.drawLine(0,y_axis+2700-y_var1,9000,y_axis+2700-y_var1);//line above results
        p.drawLine(0,y_axis+5200-y_var1,9000,y_axis+5200-y_var1);//line above graph
        p.drawLine(0,y_axis+8700-y_var1,9000,y_axis+8700-y_var1);//line above remarks
        p.drawLine(0,y_axis+9500,9000,y_axis+9500);//above the checked by line14547


        QString range_string;
        range_string.clear();
        avg_ini_melt_pnt=0;//11425
        avg_final_melt_pnt=0;
        avg_var=0;//11425
        p.setFont(QFont("Arial",10));//14547

        if((start_temp_mp1 != 0) && (end_temp_mp1 != 0))
        {
            received_res_str=res_adding(start_temp_mp1);
            range_string.append(received_res_str);
            range_string.append(" - ");
            received_res_str=res_adding(end_temp_mp1);
            range_string.append(received_res_str);

            if(range_string.size() > 13)
            {
                width_var=900;
                p.drawText(QRect(x_axis+1730, y_axis+4000+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, "NA");
                p.drawText(QRect(x_axis+3730, y_axis+4000+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, "NA");
                p.drawText(QRect(x_axis+5730, y_axis+4000+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, "NA");
            }
            else//11425
            {
                avg_ini_melt_pnt+=start_temp_mp1;//11425
                avg_final_melt_pnt+=end_temp_mp1;
                avg_var++;//11425z
                width_var=900;
                //p.drawText(QPoint(x_axis+2500, y_axis+3950), QString::number(((start_temp_mp1+avg_offset)*avg_mulfactor)));
                received_res_str=res_adding(start_temp_mp1);
                // p.drawText(x_axis+2500, y_axis+3950, received_res_str);
                p.drawText(QRect(x_axis+1730, y_axis+4000+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, received_res_str);
                received_res_str=res_adding(end_temp_mp1);
                p.drawText(QRect(x_axis+3730, y_axis+4000+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, received_res_str);
                p.drawText(QRect(x_axis+5730, y_axis+4000+4*(-y_var1),width_rect,height_rect+100),Qt::AlignCenter, range_string);

            }//11425
        }
        else
        {
            width_var=900;

            p.drawText(QRect(x_axis+1730, y_axis+4000+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, "NA");
            p.drawText(QRect(x_axis+3730, y_axis+4000+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, "NA");
            p.drawText(QRect(x_axis+5730, y_axis+4000+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, "NA");
        }
        if((start_temp_mp2 != 0) && (end_temp_mp2 != 0))
        {
            range_string.clear();
            received_res_str=res_adding(start_temp_mp2);
            range_string.append(received_res_str);
            range_string.append(" - ");
            received_res_str=res_adding(end_temp_mp2);
            range_string.append(received_res_str);
            if(range_string.size() > 13)

            {
                width_var=900;
                p.drawText(QRect(x_axis+1730, y_axis+4400+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, "NA");
                p.drawText(QRect(x_axis+3730, y_axis+4400+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, "NA");
                p.drawText(QRect(x_axis+5730, y_axis+4400+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, "NA");

            }
            else//11425
            {
                avg_ini_melt_pnt += start_temp_mp2;//11425
                avg_var++;//11425
                avg_final_melt_pnt+=end_temp_mp2;
                width_var=900;
                received_res_str=res_adding(start_temp_mp2);
                p.drawText(QRect(x_axis+1730, y_axis+4400+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight,received_res_str);
                received_res_str=res_adding(end_temp_mp2);
                p.drawText(QRect(x_axis+3730, y_axis+4400+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, received_res_str);
                p.drawText(QRect(x_axis+5730, y_axis+4400+4*(-y_var1),width_rect,height_rect+100),Qt::AlignCenter, range_string);

            }//11425
        }
        else
        {
            width_var=900;
            p.drawText(QRect(x_axis+1730, y_axis+4400+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, "NA");
            p.drawText(QRect(x_axis+3730, y_axis+4400+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, "NA");
            p.drawText(QRect(x_axis+5730, y_axis+4400+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, "NA");
        }
        if((start_temp_mp3 != 0) && (end_temp_mp3 != 0))
        {
            range_string.clear();
            received_res_str=res_adding(start_temp_mp3);
            range_string.append(received_res_str);
            range_string.append(" - ");
            received_res_str=res_adding(end_temp_mp3);
            range_string.append(received_res_str);
            if(range_string.size() > 13)
            {
                width_var=900;
                p.drawText(QRect(x_axis+1730, y_axis+4800+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, "NA");
                p.drawText(QRect(x_axis+3730, y_axis+4800+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, "NA");
                p.drawText(QRect(x_axis+5730, y_axis+4800+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, "NA");
            }
            else//11425
            {
                avg_ini_melt_pnt += start_temp_mp3;//11425
                avg_final_melt_pnt+=end_temp_mp3;
                avg_var++;//11425
                width_var=900;

                received_res_str=res_adding(start_temp_mp3);
                p.drawText(QRect(x_axis+1730, y_axis+4800+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, received_res_str);

                received_res_str=res_adding(end_temp_mp3);
                p.drawText(QRect(x_axis+3730, y_axis+4800+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, received_res_str);
                p.drawText(QRect(x_axis+5730, y_axis+4800+4*(-y_var1),width_rect,height_rect+100),Qt::AlignCenter, range_string);

            }//11425
        }
        else
        {
            width_var=900;
            p.drawText(QRect(x_axis+1730, y_axis+4800+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, "NA");
            p.drawText(QRect(x_axis+3730, y_axis+4800+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, "NA");
            p.drawText(QRect(x_axis+5730, y_axis+4800+4*(-y_var1),width_rect-width_var,height_rect+100),Qt::AlignCenter|Qt::AlignRight, "NA");

        }
        p.setFont(QFont("Arial",10));


        /*************end of thermo dynamic correction************************/
        p.drawText(x_axis+1760, y_axis-y_var1, "    ");
        p.drawText(x_axis+1760, y_axis+400-y_var1, "    ");
        p.drawText(x_axis+1760, y_axis+800-y_var1, "   ");

        range_string.clear();
        range_string.append(QDate::currentDate().toString("dd/MM/yy"));//format of date

        range_string.append("  &  ");
        range_string.append(QTime::currentTime().toString());

        p.drawText(x_axis+1760, y_axis+1200-y_var1,range_string);       //p.drawText(QPoint(x_axis+6460, y_axis+1000), QTime::currentTime().toString());



        float temp=ui->lineEdit_26->text().toFloat();
        received_res_str=res_adding(temp);
        p.drawText(QRect(x_axis+1730, y_axis+1800-y_var1,width_rect-1300,height_rect),Qt::AlignCenter,received_res_str);
        temp=ui->lineEdit_27->text().toFloat();
        received_res_str=res_adding(temp);
        p.drawText(QRect(x_axis+4700, y_axis+1800-y_var1,width_rect-1300,height_rect),Qt::AlignCenter,received_res_str);
        temp=ui->lineEdit_28->text().toFloat();
        received_res_str=res_adding(temp);
        p.drawText(QRect(x_axis+7300, y_axis+1800-y_var1,width_rect-1300,height_rect),Qt::AlignCenter,received_res_str);
        p.drawText(QRect(x_axis+1730, y_axis+2200-y_var1,width_rect-1300,height_rect),Qt::AlignCenter ,ui->lineEdit_29->text());      p.drawText(QRect(x_axis+4700, y_axis+2200-y_var1,width_rect-1300,height_rect),Qt::AlignCenter,ui->lineEdit_30->text());       p.drawText(QRect(x_axis+7300, y_axis+2200-y_var1,width_rect-1300,height_rect),Qt::AlignCenter," NA");



        p.drawText(x_axis+2200, y_axis+7100-y_var1, " ");
        p.drawText(x_axis+3700, y_axis+7100-y_var1, " ");
        p.drawText(x_axis+5200, y_axis+7100-y_var1, " ");
        p.drawText(x_axis+6700, y_axis+7100, " ");

        p.drawText(x_axis+2200, y_axis+7400-y_var1, " ");
        p.drawText(x_axis+3700, y_axis+7400-y_var1, " ");
        p.drawText(x_axis+5200, y_axis+7400-y_var1, " ");
        p.drawText(x_axis+6700, y_axis+7400-y_var1, " ");

        p.drawText(x_axis+2200, y_axis+7700-y_var1, " ");
        p.drawText(x_axis+3700, y_axis+7700-y_var1, " ");
        p.drawText(x_axis+5200, y_axis+7700-y_var1, " ");
        p.drawText(x_axis+6700, y_axis+7700-y_var1, " ");

        p.drawText(x_axis+2200, y_axis+8000-y_var1, " ");
        p.drawText(x_axis+3700, y_axis+8000-y_var1, " ");
        p.drawText(x_axis+5200, y_axis+8000-y_var1, " ");
        p.drawText(x_axis+6700, y_axis+8000-y_var1, " ");



        p.drawText(x_axis+1360, y_axis+8700-y_var1, "    ");
        p.drawText(x_axis+6660, y_axis+8700-y_var1, "   ");


        p.setFont(QFont("Arial"));
        QPen pen1(Qt::darkRed);
        pen1.setWidth(5);
        p.setPen(pen1);
        p.setPen(QPen(Qt::darkRed, 3, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
        p.drawLine(x_axis+6200, y_axis+6200-y_var1, x_axis+5900+700,y_axis+6200-y_var1);


        QPen pen2(Qt::green);
        pen2.setWidth(5);
        p.setPen(pen2);
        p.drawLine(x_axis+6200, y_axis+6700-y_var1, x_axis+5900+700,y_axis+6700-y_var1);


        QPen pen3(Qt::blue);
        pen3.setWidth(5);
        p.setPen(pen3);
        p.drawLine(x_axis+6200, y_axis+7200-y_var1, x_axis+5900+700,y_axis+7200-y_var1);
        p.setFont(QFont("Arial",8));
        p.setPen(Qt::black);
        p.drawText(QRect(x_axis+6800, y_axis+6200-4*y_var1,width_rect-1300,height_rect),Qt::AlignCenter,"Sample1");
        p.drawText(QRect(x_axis+6800, y_axis+6700-4*y_var1,width_rect-1300,height_rect),Qt::AlignCenter,"Sample2");
        p.drawText(QRect(x_axis+6800, y_axis+7200-4*y_var1,width_rect-1300,height_rect),Qt::AlignCenter,"Sample3");
    }

    else  /***************************14547*****************************/
    {
        //qdebug()<<"calib report";
        int y_var,x_var;
        QString received_res_str;//14547
        p.setRenderHints(QPainter::Antialiasing,false);
        p.setRenderHints(QPainter::TextAntialiasing,false);
        p.setRenderHints(QPainter::SmoothPixmapTransform,false);

        p.setFont(QFont("Arial",12,QFont::Bold));//14547


        p.drawText(QRect(0,y_axis-1050,9000,450), Qt::AlignCenter, " Calibration Report");

        p.setFont(QFont("Arial",10));//14547
        p.drawText(x_axis, y_axis+400, "B.No. / A.R.No.");
        p.drawText(x_axis+2050, y_axis+400,ui->lineEdit_35->text());



        p.drawText(x_axis+1900, y_axis+400, ":");
        p.drawText(x_axis+1900, y_axis+1200, ":");
        p.drawText(x_axis+1900, y_axis+2000, ":");
        p.drawText(x_axis, y_axis+1200, "Calibration Date");
        if(factory==true)
        {
            p.drawText(x_axis+2050, y_axis+1200,calib_factory_date);
        }
        else
        {

        p.drawText(x_axis+2050, y_axis+1200,calib_date);
        }



        p.drawText(x_axis+3700, y_axis+2000,trUtf8("(DD/MM/YY)"));//calib date 14547
        p.drawText(x_axis+3700, y_axis+1200,trUtf8("(DD/MM/YY)"));//calib due date 14547


        p.drawText(x_axis, y_axis+2000, "Calibration Due Date");
        if(factory==true)
        {
            p.drawText(x_axis+2050, y_axis+2000,calib_factory_duedate );
        }

        else{
        p.drawText(x_axis+2050, y_axis+2000,calib_due_date );
        }

        p.drawText(x_axis-50, y_axis+6000, "Remarks:");


        if(temp_model_no)
        {
            p.drawText(x_axis+6000, y_axis, "Model No ");
            p.drawText(x_axis+7000, y_axis,"MP98-X-X");
            p.drawText(x_axis+6000, y_axis+400, "Serial No :");
            p.drawText(x_axis+7000, y_axis+400, serial_no);
            p.drawText(x_axis+6800, y_axis,":");
            p.drawText(x_axis+6800, y_axis+400,":");
            p.drawText(x_axis+6000, y_axis+800, "Equip ID ");
            p.drawText(x_axis+7000, y_axis+800, equ_id);
            p.drawText(x_axis+6800, y_axis+800,":");


        }
        //-----------------------In Footer----------------------------------

        p.drawText(x_axis, y_axis+9300, left_tag);//14547
        p.drawText(x_axis, y_axis+9500, "Date:");
        p.drawText(x_axis+5300, y_axis+9300, right_tag);//14547
        p.drawText(x_axis+5300, y_axis+9500, "Date:");

        p.setPen(Qt::red);
        p.drawRect(QRect(0,0,1,1));
        p.setPen(Qt::black);
        p.setFont(QFont("Arial",12,QFont::Bold));



        if(field_pdf==true)
        {
            p.setPen(Qt::red);
            p.drawRect(QRect(0,0,1,1));
            p.setPen(Qt::black);
            p.setFont(QFont("Arial",12,QFont::Bold));
        y_var=50;//14547
        x_var=200;
        p.setFont(QFont("Arial",10));

        p.drawText(QRect(x_axis+50+x_var, y_axis+2800,width_rect+2200,height_rect+400),Qt::AlignCenter,trUtf8("Material Name") );//14547 msn requirement
        p.drawText(QRect(x_axis+4250+x_var, y_axis+2800+y_var,width_rect-600,height_rect-100),Qt::AlignCenter,trUtf8("  Standard "));
        p.drawText(QRect(x_axis+4250+x_var, y_axis+3000+y_var,width_rect-600,height_rect-100),Qt::AlignCenter,trUtf8("Melting Range"));
        p.drawText(QRect(x_axis+4250+x_var, y_axis+3200+y_var,width_rect-600,height_rect-100),Qt::AlignCenter,trUtf8("(°C)"));
        p.drawText(QRect(x_axis+5650+x_var, y_axis+2800+y_var,width_rect-600,height_rect-100),Qt::AlignCenter,trUtf8("  Offsets "));
        p.drawText(QRect(x_axis+5650+x_var, y_axis+3000+y_var,width_rect-600,height_rect-100),Qt::AlignCenter,trUtf8("(°C)"));


        /*****************end calibration Data*******************/

        /********************chemical names************************/

        x_var=50+200;
        y_var=450;
        width_var=2200;

        p.drawText(QRect(x_axis+x_var, y_axis+3050+y_var,width_rect+width_var,height_rect+50), Qt::AlignCenter,(Noofsamples>=1)?(sample_calib_name[0]):"NA");
        p.drawText(QRect(x_axis+x_var, y_axis+3400+y_var,width_rect+width_var,height_rect+50), Qt::AlignCenter,(Noofsamples>=2)?(sample_calib_name[1]):"NA");
        p.drawText(QRect(x_axis+x_var, y_axis+3750+y_var,width_rect+width_var,height_rect+50), Qt::AlignCenter,(Noofsamples>=3)?(sample_calib_name[2]):"NA");
        p.drawText(QRect(x_axis+x_var, y_axis+4100+y_var,width_rect+width_var,height_rect+50), Qt::AlignCenter,(Noofsamples>=4)?(sample_calib_name[3]):"NA");

        /**********************standarad temp*********************/

        x_var=4250+200;
        y_var=450;
        width_var=1450;
        if(single1==false)
        p.drawText(QRect(x_axis+x_var, y_axis+3050+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=1)?(received_res_str=res_adding(sample_calib_rgl[0])):" NA "));// start temp
        if(single2==false)
        p.drawText(QRect(x_axis+x_var, y_axis+3400+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=2)?(received_res_str=res_adding(sample_calib_rgl[1])):" NA "));
        if(single3==false)
        p.drawText(QRect(x_axis+x_var, y_axis+3750+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=3)?(received_res_str=res_adding(sample_calib_rgl[2])):" NA "));
        if(single4==false)
        p.drawText(QRect(x_axis+x_var, y_axis+4100+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=4)?(received_res_str=res_adding(sample_calib_rgl[3])):" NA "));



        x_var=5000+200;

        if(single1==false)
        p.drawText(QRect(x_axis+x_var, y_axis+3050+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=1)?(received_res_str=res_adding(sample_calib_rgh[0])):" NA "));// end temp
        if(single2==false)
        p.drawText(QRect(x_axis+x_var, y_axis+3400+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=2)?(received_res_str=res_adding(sample_calib_rgh[1])):" NA "));
        if(single3==false)
        p.drawText(QRect(x_axis+x_var, y_axis+3750+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=3)?(received_res_str=res_adding(sample_calib_rgh[2])):" NA "));
        if(single4==false)
        p.drawText(QRect(x_axis+x_var, y_axis+4100+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=4)?(received_res_str=res_adding(sample_calib_rgh[3])):" NA "));


 /**************************measured temp**************************/
        x_var=5650+200;
        /**********uncommented code for mim max range*************************/

        width_var=600;

        x_var=6400;



      /*  p.drawText(QRect(x_axis+x_var, y_axis+3050+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=1)?(received_res_str=res_adding(largest_final[0])):" NA "));// end temp
        p.drawText(QRect(x_axis+x_var, y_axis+3400+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=2)?(received_res_str=res_adding(largest_final[1])):" NA "));
        p.drawText(QRect(x_axis+x_var, y_axis+3750+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=3)?(received_res_str=res_adding(largest_final[2])):" NA "));
        p.drawText(QRect(x_axis+x_var, y_axis+4100+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=4)?(received_res_str=res_adding(largest_final[3])):" NA "));*/





        /*************************calibrated to************************************/

      /*  x_var=7050;


        p.drawText(QRect(x_axis+x_var, y_axis+3050+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=1)?(received_res_str=res_adding(sample_calib_rgl[0])):" NA "));// start temp
        p.drawText(QRect(x_axis+x_var, y_axis+3400+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=2)?(received_res_str=res_adding(sample_calib_rgl[1])):" NA "));
        p.drawText(QRect(x_axis+x_var, y_axis+3750+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=3)?(received_res_str=res_adding(sample_calib_rgl[2])):" NA "));
        p.drawText(QRect(x_axis+x_var, y_axis+4100+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=4)?(received_res_str=res_adding(sample_calib_rgl[3])):" NA "));


        x_var=7800;

        p.drawText(QRect(x_axis+x_var, y_axis+3050+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=1)?(received_res_str=res_adding(sample_calib_rgh[0])):" NA "));// end temp
        p.drawText(QRect(x_axis+x_var, y_axis+3400+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=2)?(received_res_str=res_adding(sample_calib_rgh[1])):" NA "));
        p.drawText(QRect(x_axis+x_var, y_axis+3750+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=3)?(received_res_str=res_adding(sample_calib_rgh[2])):" NA "));
        p.drawText(QRect(x_axis+x_var, y_axis+4100+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignVCenter,((Noofsamples>=4)?(received_res_str=res_adding(sample_calib_rgh[3])):" NA "));*/





        x_var=50+200;
        y_var=450;
        width_var=2200;
        p.drawRect(QRect(x_axis+x_var,y_axis+2800,width_rect+width_var,height_rect+400));//1st coloumn
        p.drawRect(QRect(x_axis+x_var,y_axis+3050+y_var,width_rect+width_var,height_rect+50));
        p.drawRect(QRect(x_axis+x_var,y_axis+3400+y_var,width_rect+width_var,height_rect+50));
        p.drawRect(QRect(x_axis+x_var,y_axis+3750+y_var,width_rect+width_var,height_rect+50));
        p.drawRect(QRect(x_axis+x_var,y_axis+4100+y_var,width_rect+width_var,height_rect+50));

        x_var=4250+200;
        width_var=600;
        p.drawRect(QRect(x_axis+x_var,y_axis+2800,width_rect-width_var,height_rect+400));//2nd coloumn
        p.drawRect(QRect(x_axis+x_var,y_axis+3050+y_var,width_rect-width_var,height_rect+50));
        p.drawRect(QRect(x_axis+x_var,y_axis+3400+y_var,width_rect-width_var,height_rect+50));
        p.drawRect(QRect(x_axis+x_var,y_axis+3750+y_var,width_rect-width_var,height_rect+50));
        p.drawRect(QRect(x_axis+x_var,y_axis+4100+y_var,width_rect-width_var,height_rect+50));


        if(single1==false)
        p.drawText(QRect(x_axis+x_var,y_axis+3050+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,"-");//2nd coloumn
        else
        p.drawText(QRect(x_axis+x_var,y_axis+3050+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignCenter,((Noofsamples>=1)?(received_res_str=res_adding(cmp_fac[0])):" NA "));
        if(single2==false)
        p.drawText(QRect(x_axis+x_var,y_axis+3400+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,"-");
        else
        p.drawText(QRect(x_axis+x_var,y_axis+3400+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignCenter,((Noofsamples>=1)?(received_res_str=res_adding(cmp_fac[1])):" NA "));
        if(single3==false)
        p.drawText(QRect(x_axis+x_var,y_axis+3750+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,"-");
        else
        p.drawText(QRect(x_axis+x_var,y_axis+3750+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignCenter,((Noofsamples>=1)?(received_res_str=res_adding(cmp_fac[2])):" NA "));
        if(single4==false)
        p.drawText(QRect(x_axis+x_var,y_axis+4100+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,"-");
        else
        p.drawText(QRect(x_axis+x_var,y_axis+4100+y_var,width_rect-width_var,height_rect+50),Qt::AlignTrailing|Qt::AlignCenter,((Noofsamples>=1)?(received_res_str=res_adding(cmp_fac[3])):" NA "));

        x_var=5650+200;

        p.drawRect(QRect(x_axis+x_var,y_axis+2800,width_rect-width_var,height_rect+400));// 3rd coloumn
        p.drawRect(QRect(x_axis+x_var,y_axis+3050+y_var,width_rect-width_var,height_rect+50));
        p.drawRect(QRect(x_axis+x_var,y_axis+3400+y_var,width_rect-width_var,height_rect+50));
        p.drawRect(QRect(x_axis+x_var,y_axis+3750+y_var,width_rect-width_var,height_rect+50));
        p.drawRect(QRect(x_axis+x_var,y_axis+4100+y_var,width_rect-width_var,height_rect+50));


        p.drawText(QRect(x_axis+x_var, y_axis+3050+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,((Noofsamples>=1)?(received_res_str=res_adding(cfield[0])):" NA "));// start temp
        p.drawText(QRect(x_axis+x_var, y_axis+3400+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,((Noofsamples>=2)?(received_res_str=res_adding(cfield[1])):" NA "));
        p.drawText(QRect(x_axis+x_var, y_axis+3750+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,((Noofsamples>=3)?(received_res_str=res_adding(cfield[2])):" NA "));
        p.drawText(QRect(x_axis+x_var, y_axis+4100+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,((Noofsamples>=4)?(received_res_str=res_adding(cfield[3])):" NA "));

       /* p.drawText(QRect(x_axis+x_var,y_axis+3050+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,"-");//3rd coloumn
        p.drawText(QRect(x_axis+x_var,y_axis+3400+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,"-");
        p.drawText(QRect(x_axis+x_var,y_axis+3750+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,"-");
        p.drawText(QRect(x_axis+x_var,y_axis+4100+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,"-");*/

        x_var=7050+200;
       /* p.drawRect(QRect(x_axis+x_var,y_axis+2800,width_rect-width_var,height_rect+400));//4th coloumn
        p.drawRect(QRect(x_axis+x_var,y_axis+3050+y_var,width_rect-width_var,height_rect+50));
        p.drawRect(QRect(x_axis+x_var,y_axis+3400+y_var,width_rect-width_var,height_rect+50));
        p.drawRect(QRect(x_axis+x_var,y_axis+3750+y_var,width_rect-width_var,height_rect+50));
        p.drawRect(QRect(x_axis+x_var,y_axis+4100+y_var,width_rect-width_var,height_rect+50));

        p.drawText(QRect(x_axis+x_var,y_axis+3050+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,"-");//4th coloumn
        p.drawText(QRect(x_axis+x_var,y_axis+3400+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,"-");
        p.drawText(QRect(x_axis+x_var,y_axis+3750+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,"-");
        p.drawText(QRect(x_axis+x_var,y_axis+4100+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,"-");*/

}
        if(factory_pdf==true)
        {

            p.setPen(Qt::red);
            p.drawRect(QRect(0,0,1,1));
            p.setPen(Qt::black);
            p.setFont(QFont("Arial",12,QFont::Bold));
        y_var=50;//14547
        x_var=200;
        p.setFont(QFont("Arial",10));

        p.drawText(QRect(x_axis+200+x_var, y_axis+2700,width_rect+2200,height_rect+400),Qt::AlignCenter,trUtf8("Temperature Range") );//14547 msn requirement
        p.drawText(QRect(x_axis+200+x_var, y_axis+2900,width_rect+2200,height_rect+400),Qt::AlignCenter,trUtf8(" (°C)") );
        p.drawText(QRect(x_axis+4250+x_var, y_axis+2900+y_var,width_rect-600,height_rect-100),Qt::AlignCenter,trUtf8("  Slopes "));
        p.drawText(QRect(x_axis+5650+x_var, y_axis+2900+y_var,width_rect-600,height_rect-100),Qt::AlignCenter,trUtf8("  Offsets "));
        p.drawText(QRect(x_axis+5650+x_var, y_axis+3100+y_var,width_rect-600,height_rect-100),Qt::AlignCenter,trUtf8("(°C)"));


        /*****************end calibration Data*******************/





        x_var=50+200;
        y_var=450;
        width_var=2200;
        p.drawRect(QRect(x_axis+x_var,y_axis+2800,width_rect+width_var,height_rect+400));//1st coloumn
        p.drawRect(QRect(x_axis+x_var,y_axis+3050+y_var,width_rect+width_var,height_rect+50));
        p.drawRect(QRect(x_axis+x_var,y_axis+3400+y_var,width_rect+width_var,height_rect+50));
        p.drawRect(QRect(x_axis+x_var,y_axis+3750+y_var,width_rect+width_var,height_rect+50));

        received_res_str="< ";
        received_res_str.append(res_adding(Mmp_fac[1]));
        p.drawText(QRect(x_axis+x_var, y_axis+3050+y_var,width_rect+width_var,height_rect+50),Qt::AlignCenter,((Noofsamples>=2)?(received_res_str):" NA "));
        received_res_str=res_adding(Mmp_fac[1]);
        received_res_str.append(" to ");
        received_res_str.append(res_adding(Mmp_fac[2]));
        p.drawText(QRect(x_axis+x_var, y_axis+3400+y_var,width_rect+width_var,height_rect+50),Qt::AlignCenter,((Noofsamples>=3)?(received_res_str):" NA "));
        received_res_str="> ";
        received_res_str.append(res_adding(Mmp_fac[2]));
        p.drawText(QRect(x_axis+x_var, y_axis+3750+y_var,width_rect+width_var,height_rect+50),Qt::AlignCenter,((Noofsamples>=3)?(received_res_str):" NA "));

        x_var=4250+200;
        width_var=600;
        p.drawRect(QRect(x_axis+x_var,y_axis+2800,width_rect-width_var,height_rect+400));//2nd coloumn
        p.drawRect(QRect(x_axis+x_var,y_axis+3050+y_var,width_rect-width_var,height_rect+50));
        p.drawRect(QRect(x_axis+x_var,y_axis+3400+y_var,width_rect-width_var,height_rect+50));
        p.drawRect(QRect(x_axis+x_var,y_axis+3750+y_var,width_rect-width_var,height_rect+50));

        p.drawText(QRect(x_axis+x_var,y_axis+3050+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,((Noofsamples>=2)?(received_res_str=res_adding(mfactory[0])):" NA "));
        p.drawText(QRect(x_axis+x_var,y_axis+3400+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,((Noofsamples>=3)?(received_res_str=res_adding(mfactory[1])):" NA "));
        p.drawText(QRect(x_axis+x_var,y_axis+3750+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,((Noofsamples>=4)?(received_res_str=res_adding(mfactory[2])):" NA "));
        x_var=5650+200;

        p.drawRect(QRect(x_axis+x_var,y_axis+2800,width_rect-width_var,height_rect+400));// 3rd coloumn
        p.drawRect(QRect(x_axis+x_var,y_axis+3050+y_var,width_rect-width_var,height_rect+50));
        p.drawRect(QRect(x_axis+x_var,y_axis+3400+y_var,width_rect-width_var,height_rect+50));
        p.drawRect(QRect(x_axis+x_var,y_axis+3750+y_var,width_rect-width_var,height_rect+50));

        p.drawText(QRect(x_axis+x_var,y_axis+3050+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,((Noofsamples>=2)?(received_res_str=res_adding(cfactory[0])):" NA "));
        p.drawText(QRect(x_axis+x_var,y_axis+3400+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,((Noofsamples>=3)?(received_res_str=res_adding(cfactory[1])):" NA "));
        p.drawText(QRect(x_axis+x_var,y_axis+3750+y_var,width_rect-width_var,height_rect+50),Qt::AlignCenter,((Noofsamples>=4)?(received_res_str=res_adding(cfactory[2])):" NA "));


        }


        //---------------------In End footer----------------------------------
        p.setPen(Qt::red);
        p.drawRect(QRect(0,0,1,1));
        p.setPen(Qt::black);
        p.setFont(QFont("Arial",12,QFont::Bold));
        //------For configuration Rectangles--------------------------------

        p.drawRect(QRect(0,0,9000,12000));//main rect
        p.drawRect(QRect(x_axis+2000,y_axis+200,width_rect-700,height_rect));//calib date
        p.drawRect(QRect(x_axis+2000,y_axis+1000,width_rect-700,height_rect));//calib due date
        p.drawRect(QRect(x_axis+2000,y_axis+1800,width_rect-700,height_rect));//A.r.no / B. no
        p.drawImage(QRect(x_axis+7550,0,1250,1150),maskk);//14547


        p.drawLine(0,y_axis-1050,9000,y_axis-1050);//11425 above calibration report 14547
        p.drawLine(0,y_axis-600,9000,y_axis-600);//11425 below calibration report 14547
        p.drawLine(0,y_axis+5400,9000,y_axis+5400);//line above remarks 14547
        p.drawLine(0,y_axis+8700,9000,y_axis+8700);//line below remarks 14547

        /*********************************end 14547**************************************/


    }

    //qdebug() << "start3";
}



void MainWindow::avg_save_file()
{


    min_range=0;
    max_range=0;

    min_slope=0;
    max_slope=0;
    min11=max11=min22=max22=min33=max33=0;
    clear1=false;
    clear2=false;
    clear3=false;
    index1=index2=index3=0;
    index_min1=index_min2=index_min3=0;
    start_temp_mp1=start_temp_mp2=start_temp_mp3=0;
    end_temp_mp1=end_temp_mp2=end_temp_mp3=0;
    MP_therm1=MP_therm2=MP_therm3=0;
    max1=max2=max3=min1=min2=min3=0;
    temp_clear1=temp_clear2=temp_clear3=0;
    temp_onset1=temp_onset2=temp_onset3=0;
    end_temp_mp1=end_temp_mp2=end_temp_mp1=mean_avg=0;
    bool raw_end1,raw_end2,raw_end3,raw_strt1,raw_strt2,raw_strt3;
    bool start_found1,start_found2,start_found3;
    start_found1=start_found2=start_found3=false;
    int  start_temp_index1, start_temp_index2, start_temp_index3;
    start_temp_index1= start_temp_index2= start_temp_index3=0;
    float onset_percnt,clear_percnt,temp_start1,temp_start2,temp_start3,temp_end1,temp_end2,temp_end3;
    temp_start1=temp_start2=temp_start3=temp_end1=temp_end2=temp_end3=0;
    onset_percnt=ui->lineEdit_29->text().toFloat();
    clear_percnt=ui->lineEdit_30->text().toFloat();

    raw_end1=true;
    raw_end2=true;
    raw_end3=true;
    raw_strt1=true;
    raw_strt2=true;
    raw_strt3=true;
graph_enable_flag1=false;
graph_enable_flag2=false;
graph_enable_flag3=false;



    double  min_range_norm1,max_range_norm1,min_range_norm2,max_range_norm2,min_range_norm3,max_range_norm3;
    min_range_norm1 = max_range_norm1 = sample1_norm.at(0);
    min_range_norm2 = max_range_norm2 = sample2_norm.at(0);
    min_range_norm3 = max_range_norm3 = sample3_norm.at(0);

    double min_range_slope1,min_range_slope2,min_range_slope3,max_range_slope1,max_range_slope2,max_range_slope3;

    new_sample1.clear();
    new_sample2.clear();
    new_sample3.clear();

    new_temp.clear();

    double temp;
    double sum=0;
    double i1sum=0;
    double i2sum=0;
    double i3sum=0;
    int x=0;
    int z=0;
    int i=0;
    temp=temp_array_final[i];
    for (int i = 0;i < temp_array_final.size()-1;i++)
    {
        if(((temp==temp_array_final[i])&&(temp==temp_array_final[i+1])&&(temp==temp_array_final[i+2])&&(temp==temp_array_final[i+3])&&(temp==temp_array_final[i+4])))// comparing 3 consecutive values
        {
            x++;
            sum=sum+temp_array_final[i];// summing same temperatures
            i1sum=i1sum+sample1_norm[i];// summing intensities parallely with temperatures
            i2sum=i2sum+sample2_norm[i];
            i3sum=i3sum+sample3_norm[i];
        }
        else if((temp==temp_array_final[i])&&(temp==temp_array_final[i+1])&&(temp==temp_array_final[i+2])&&(temp==temp_array_final[i+3]))// comparing 3consecutive values
        {
            x++;
            sum=sum+temp_array_final[i];// summing same temperatures
            i1sum=i1sum+sample1_norm[i];// summing intensities parallely with temperatures
            i2sum=i2sum+sample2_norm[i];
            i3sum=i3sum+sample3_norm[i];

        }
        else if((temp==temp_array_final[i])&&(temp==temp_array_final[i+1])&&(temp==temp_array_final[i+2]))
        {
            x++;
            sum=sum+temp_array_final[i];// summing same temperatures
            i1sum=i1sum+sample1_norm[i];// summing intensities parallely with temperatures
            i2sum=i2sum+sample2_norm[i];
            i3sum=i3sum+sample3_norm[i];
        }
        else if((temp==temp_array_final[i])&&(temp==temp_array_final[i+1]))
        {
            x++;
            sum=sum+temp_array_final[i];// summing same temperatures
            i1sum=i1sum+sample1_norm[i];// summing intensities parallely with temperatures
            i2sum=i2sum+sample2_norm[i];
            i3sum=i3sum+sample3_norm[i];
        }
        else if((temp==temp_array_final[i]))
        {
            x++;
            sum=sum+temp_array_final[i];// summing same temperatures
            i1sum=i1sum+sample1_norm[i];// summing intensities parallely with temperatures
            i2sum=i2sum+sample2_norm[i];
            i3sum=i3sum+sample3_norm[i];
        }
        else if((temp==temp_array_final[i+1]))
        {
            ;
        }
        else
        {
            z=1;
        }

        if(z==1)
        {
            if(x>0)
            {


                if(temp<temp_array_final[i])
                {
                    new_temp.append(sum/x);// averaging same temperatures
                    new_sample1.append(i1sum/x);// averaging intensities parallely with temperatures
                    new_sample2.append(i2sum/x);
                    new_sample3.append(i3sum/x);
                   x=0;
                   z=0;
                   sum=0;
                   i1sum=0;
                   i2sum=0;
                   i3sum=0;
                   temp=temp_array_final[i];
                   i--;
                }
                else
                {
                    i++;
                }

             }

        }

    }
    qDebug() <<"raw data";
    qDebug() <<"temp_array_final"<<temp_array_final;
    qDebug() <<"sample1_norm"<<sample1_norm;
    qDebug() <<"sample2_norm"<<sample2_norm;
    qDebug() <<"sample3_norm"<<sample3_norm;

    qDebug() <<"Data before smoothing";
    qDebug() <<"new_sample1"<<new_sample1;
    qDebug() <<"new_sample2"<<new_sample2;
    qDebug() <<"new_sample3"<<new_sample3;
    qDebug() <<"avgd temp"<<new_temp;

  /*  for(unsigned short m = 1; m < new_sample1.size();m++)  // THIS LOOP ONE OF THE SMOOTHING TECHINIQUE TO CORRECT THE GRAPH
    {   // 0.2 + 0.8 = 1..
        qDebug() << "1234567890";
        new_sample1.replace(m,((0.2 * new_sample1.at(m))+(0.8 * new_sample1.at(m-1))));
        new_sample2.replace(m,((0.2 * new_sample2.at(m))+(0.8 * new_sample2.at(m-1))));
        new_sample3.replace(m,((0.2 * new_sample3.at(m))+(0.8 * new_sample3.at(m-1))));
    }*/
    max1=min1=new_sample1[0];
    max2=min2=new_sample2[0];
    max3=min3=new_sample3[0];


    for(i=0;i<new_sample1.size()-1;i++)// finding maxi & min intensity of all the 3 samples
          {
             if(new_sample1[i]>max1)
              {
                 max1=new_sample1[i];

              }
             if(new_sample1[i]<min1)
              {
                min1=new_sample1[i];
              }
             if(new_sample2[i]>max2)
              {
                 max2=new_sample2[i];

              }
             if(new_sample2[i]<min2)
              {
                 min2=new_sample2[i];
              }

             if(new_sample3[i]>max3)
              {
                 max3=new_sample1[i];

              }
             if(new_sample3[i]<min3)
              {
                 min3=new_sample3[i];
              }

          }


    temp_onset1 = ((max1 - min1) *(30/100.0)) + min1; // change the variable names. this is not temp. this is sample value
    temp_onset2 = ((max2 - min2) *(30/100.0)) + min2;
    temp_onset3 = ((max3 - min3) *(30/100.0)) + min3;

         stop_sample1 =  ((max1 - min1) *0.6) + min1;
         stop_sample2 =  ((max2 - min2) *0.6) + min2;
         stop_sample3 =  ((max3 - min3) *0.6) + min3;

         start_sample1 =  ((max1 - min1) *0.1) + min1;
         start_sample2 =  ((max2 - min2) *0.1) + min2;
         start_sample3 =  ((max3 - min3) *0.1) + min3;

          found1 = found2 = found3 = false;
          for(int i=0; i<new_sample1.size();i++)
          {
             if((new_sample1[i] > stop_sample1) && !found1)// finding index at 60% change
              {
                  stop_temp_index1 = i; found1 = true;

              }
             if((new_sample1[i] > start_sample1) && !start_found1)// finding index at 10% change
              {
                   start_temp_index1 = i; start_found1 = true;

              }
             if((new_sample2[i] > stop_sample2) && !found2)
              {
                  stop_temp_index2 = i; found2 = true;

              }
             if((new_sample2[i] > start_sample2) && !start_found2)// finding index at 60% change
              {
                   start_temp_index2 = i; start_found2 = true;

              }

             if((new_sample3[i] > stop_sample3)&&  !found3)
              {
               stop_temp_index3 = i; found3 = true;

              }
             if((new_sample3[i] > start_sample3)&&  !start_found3)
              {
               start_temp_index3 = i; start_found3 = true;

              }

          }



          new_sample1_slope.clear();
          new_sample2_slope.clear();
          new_sample3_slope.clear();


          for(i=0;i<new_sample1.size()-2;i++)
          {
             new_sample1_slope.append(new_sample1[i+1] - new_sample1[i]);
             new_sample2_slope.append(new_sample2[i+1] - new_sample2[i]);
             new_sample3_slope.append(new_sample3[i+1] - new_sample3[i]);

          }
         new_sample1_slope.append(new_sample1_slope[i-1]); // this ensure the # of samples match the temp array
         new_sample2_slope.append(new_sample2_slope[i-1]);
         new_sample3_slope.append(new_sample3_slope[i-1]);

         qDebug() <<" Slope Data";
         qDebug() <<" new_sample1_slope"<< new_sample1_slope;
         qDebug() <<" new_sample2_slope"<< new_sample2_slope;
         qDebug() <<" new_sample3_slope"<< new_sample3_slope;

             max11= new_sample1_slope[start_temp_index1];
             max22= new_sample2_slope[start_temp_index2];
             max33= new_sample3_slope[start_temp_index3];
             temp_start1=new_temp[start_temp_index3];
             temp_start2=new_temp[start_temp_index3];
             temp_start3=new_temp[start_temp_index3];
             qDebug()<<"max11"<<max11;

             for(int i=start_temp_index1; i<stop_temp_index1; i++) // is this index and and slope index right?
              {


                     if((new_sample1_slope[i+1]>=max11))
                      {
                        max11=new_sample1_slope[i];
                          index1=i;
                         temp_start1=new_temp[i];

                      }
              }

            start_temp_mp1=(temp_start1+(onset_percnt/100));
             for(int i=start_temp_index2; i<stop_temp_index2; i++)
              {

                    if(new_sample2_slope[i+1]>=max22)
                      {
                         max22=new_sample2_slope[i];
                         index2=i;
                         temp_start2=new_temp[i];


                    }

              }
              start_temp_mp2=(temp_start2+(onset_percnt/100));

             for(int i=start_temp_index3; i<stop_temp_index3; i++)
              {

                  if((new_sample3_slope[i+1]>=max33))
                      {
                         max33=new_sample3_slope[i];
                          index3=i;
                         temp_start3=new_temp[i];

                      }
                  }
              start_temp_mp3=(temp_start3+(onset_percnt/100));


                  for(int i=new_sample1_slope.size()-1;i>=0;i--)
                  {
                   if(raw_end1==true)
                   {
                   if((new_sample1_slope.at(i-1)>=5000)||(new_sample1_slope.at(i-1)<=-5000))
                   {
                       temp_end1=new_temp[i];
                       raw_end1=false;

                       end_temp_mp1=temp_end1-(1-(clear_percnt/100));

                   }
                   }

                   if(raw_end2==true)
                   {
                        if((new_sample2_slope.at(i-1)>=5000)||(new_sample2_slope.at(i-1)<=-5000))
                       {
                           temp_end2=new_temp[i];
                           raw_end2=false;

                           end_temp_mp2=temp_end2-(1-(clear_percnt/100));


                       }
                   }
                   if(raw_end3==true)
                   {
                       if((new_sample3_slope.at(i-1)>=5000)||(new_sample3_slope.at(i-1)<=-5000))
                       {
                           temp_end3=new_temp[i];
                           raw_end3=false;

                           end_temp_mp3=temp_end3-(1-(clear_percnt/100));


                        }
                   }
               }





         min_range_slope1=max_range_slope1=new_sample1_slope.at(0);
         min_range_slope2=max_range_slope2=new_sample2_slope.at(0);
         min_range_slope3=max_range_slope3=new_sample3_slope.at(0);


        for(int i=0; i<new_sample1.size(); i++)// finding min and max from averaged data
        {
            if(new_sample1.at(i) < min_range_norm1)
                min_range_norm1 = new_sample1.at(i);
            if(new_sample1.at(i) > max_range_norm1)
                max_range_norm1 = new_sample1.at(i);

            if(new_sample2.at(i) < min_range_norm2)
                min_range_norm2 = new_sample2.at(i);
            if(new_sample2.at(i) > max_range_norm2)
                max_range_norm2 = new_sample2.at(i);

            if(new_sample3.at(i) < min_range_norm3)
                min_range_norm3 = new_sample3.at(i);
            if(new_sample3.at(i) > max_range_norm3)
                max_range_norm3 = new_sample3.at(i);

        }



        min_range=min_range_norm1;// finding min/max of all the 3 minium/maximum intensity values
        max_range=max_range_norm1;
        if (min_range_norm2< min_range)
            min_range =min_range_norm2;
        if (min_range_norm3 < min_range)
            min_range =min_range_norm3;

        if (max_range_norm2 > max_range)
            max_range = max_range_norm2;
        if (max_range_norm3 >max_range)
            max_range = max_range_norm3;







     min_range_slope1=max_range_slope1=new_sample1_slope.at(0);
     min_range_slope2=max_range_slope2=new_sample2_slope.at(0);
     min_range_slope3=max_range_slope3=new_sample3_slope.at(0);


    for(int i=0; i<new_sample1_slope.size(); i++)
    {
        if(new_sample1_slope.at(i) < min_range_slope1)
            min_range_slope1 = new_sample1_slope.at(i);
        if(new_sample1_slope.at(i) > max_range_slope1)
            max_range_slope1 = new_sample1_slope.at(i);

        if(new_sample2_slope.at(i) < min_range_slope2)
            min_range_slope2 = new_sample2_slope.at(i);
        if(new_sample2_slope.at(i) > max_range_slope2)
            max_range_slope2 = new_sample2_slope.at(i);

        if(new_sample3_slope.at(i) < min_range_slope3)
            min_range_slope3 = new_sample3_slope.at(i);
        if(new_sample3_slope.at(i) > max_range_slope3)
            max_range_slope3 = new_sample3_slope.at(i);

    }


    min_slope=min_range_slope1;
    max_slope=max_range_slope1;

    if (min_range_slope2<  min_slope)
        min_range =min_range_slope2;
    if (min_range_slope3 < min_slope)
         min_slope =min_range_slope3;

    if (max_range_slope2 >max_slope)
        max_slope = max_range_slope2;
    if (max_range_slope3 >max_slope)
        max_slope = max_range_slope3;

    avg_den=0;
    mean_avg=0;
    if(manual_operation==1)
    {
        start_temp_mp1=manual_strt1;
        start_temp_mp2=manual_strt2;
        start_temp_mp3=manual_strt3;

        end_temp_mp1=manual_end1;
        end_temp_mp2=manual_end2;
        end_temp_mp3=manual_end3;

    }

     if(max1-min1<10000)
     {
         start_temp_mp1=0;
         end_temp_mp1=0;
         qDebug()<<"inside sample not inserted1";
         ui->sample1_label1->hide();
         ui->sample1_label2->hide();
         ui->sample1_label3->hide();
         ui->sample1_label4->hide();
         ui->sample1_label5->hide();
         ui->sample1_label6->hide();

         ui->label1_err_message->show();
         graph_enable_flag1=false;
     }
     else
     {


         ui->sample1_label1->show();
         ui->sample1_label2->show();
         ui->sample1_label3->show();
         ui->sample1_label4->show();
         ui->sample1_label5->show();
         ui->sample1_label6->show();
         ui->label1_err_message->hide();
         ui->sample1_label2->setText(res_adding(start_temp_mp1));
         ui->sample1_label5->setText(res_adding(end_temp_mp1));
         mean_avg=((start_temp_mp1+end_temp_mp1)/2.0);
         avg_den++;       
         MP_therm1 =(end_temp_mp1-((thermo_correction_factor)*sqrt(ramp_rate)));

         graph_enable_flag1=true;
     }

     if(max2-min2<10000)
     {
         start_temp_mp1=0;
         end_temp_mp1=0;
         qDebug()<<"inside sample not inserted2";
         ui->sample2_label1->hide();
         ui->sample2_label2->hide();
         ui->sample2_label3->hide();
         ui->sample2_label4->hide();
         ui->sample2_label5->hide();
         ui->sample2_label6->hide();

         ui->label2_err_message->show();
         graph_enable_flag2=false;



     }
     else
     {
         ui->sample2_label1->show();
         ui->sample2_label2->show();
         ui->sample2_label3->show();
         ui->sample2_label4->show();
         ui->sample2_label5->show();
         ui->sample2_label6->show();
         ui->label2_err_message->hide();
         ui->sample2_label2->setText(res_adding(start_temp_mp2));
         ui->sample2_label5->setText(res_adding(end_temp_mp2));
         mean_avg+=((start_temp_mp2+end_temp_mp2)/2.0);
         avg_den++;
         MP_therm2 =(end_temp_mp2-((thermo_correction_factor)*sqrt(ramp_rate)));
         graph_enable_flag2=true;
     }

     if(max3-min3<10000)
     {
         start_temp_mp1=0;
         end_temp_mp1=0;
         qDebug()<<"inside sample not inserted3";
         ui->sample3_label1->hide();
         ui->sample3_label2->hide();
         ui->sample3_label3->hide();
         ui->sample3_label4->hide();
         ui->sample3_label5->hide();
         ui->sample3_label6->hide();
         ui->label3_err_message->show();
         graph_enable_flag3=false;
     }
     else
     {
         ui->sample3_label1->show();
         ui->sample3_label2->show();
         ui->sample3_label3->show();
         ui->sample3_label4->show();
         ui->sample3_label5->show();
         ui->sample3_label6->show();
         ui->label3_err_message->hide();
         ui->sample3_label2->setText(res_adding(start_temp_mp3));
         ui->sample3_label5->setText(res_adding(end_temp_mp3));
         mean_avg+=((start_temp_mp3+end_temp_mp3)/2.0);
         avg_den++;
         MP_therm3 =(end_temp_mp3-((thermo_correction_factor)*sqrt(ramp_rate)));
         graph_enable_flag3=true;
     }

     qDebug()<<"mean_avg"<<mean_avg/avg_den*1.0;
}
void MainWindow::save_Results() // we need to check for disk space before saving, actually before even startiong the process.
{




    ui->tabWidget->setCurrentIndex(2); //vinay
    if(calib_flag==true)
    {
        ui->pushButton_75->show();
        ui->pushButton_74->show();
        hiding_calib(!calib_flag);
        ui->pushButton_18->setEnabled(true);
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_16->setEnabled(false);
        ui->lineEdit_35->setEnabled(true);//11425
    }
    else
    {
        ui->pushButton_75->hide();
        ui->pushButton_74->hide();
    }


    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString(Qt::ISODate);
    dateTimeString.remove(" ");
    dateTimeString.remove("-");
    dateTimeString.remove(":");
    dateTimeString.replace("T","_");
    filename.clear();
    filename.append(filename11);
    filename.append("/");
    filename.append(ui->lineEdit_35->text());
    filename.append(ui->lineEdit_25->text());
    filename.append(dateTimeString);


    filename_pdf.clear();
    filename_pdf.append(filename);
    filename_pdf.append(".png");
    fname.clear();
    fname.append(filename_pdf);


   /* QCPItemText *textLabel = new QCPItemText(customPlot);
    customPlot->addItem(textLabel);
    textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
    textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    textLabel->position->setCoords(0.5, 0); // place position at center/top of axis rect
    textLabel->setText("Melting Curve");
    textLabel->setFont(QFont(font().family(), 16)); // make font a bit larger
    textLabel->setPen(QPen(Qt::black)); // show black border around text*/ // vinay
    customPlot->xAxis->setLabel("Temperature");
    //customPlot->yAxis->setLabel("Intensity");


     avg_save_file();

     if(graph_enable_flag1==true)
     {
    //slope of raw data
         customPlot->graph(0)->setVisible(true);
         customPlot->graph(0)->setData(new_temp, new_sample1);//res1
         customPlot->graph(0)->setPen(QPen(Qt::darkRed)); // line color red for first graph//res1
         customPlot->graph(3)->setVisible(false);
         customPlot->graph(3)->setData(new_temp, new_sample1_slope);
         customPlot->graph(3)->setPen(QPen(Qt::darkYellow));

     }
     else
     {
         customPlot->graph(0)->setVisible(false);
         //customPlot->graph(3)->setVisible(false);
     }

     if(graph_enable_flag2==true)
     {
    //slope of raw data
         customPlot->graph(1)->setVisible(true);
         customPlot->graph(1)->setData(new_temp, new_sample2);//res1
         customPlot->graph(1)->setPen(QPen(Qt::darkGreen)); // line color blue for first graph//res1
         customPlot->graph(4)->setVisible(false);
         customPlot->graph(4)->setData(new_temp, new_sample2_slope);
         customPlot->graph(4)->setPen(QPen(Qt::darkCyan));

     }
     else
     {
         customPlot->graph(1)->setVisible(false);
        // customPlot->graph(4)->setVisible(false);
     }


     if(graph_enable_flag3==true)
     {
    //slope of raw data
        customPlot->graph(2)->setVisible(true);
        customPlot->graph(2)->setData(new_temp, new_sample3);//res1
        customPlot->graph(2)->setPen(QPen(Qt::darkBlue)); // line color blue for first graph//res1
        customPlot->graph(5)->setVisible(false);
        customPlot->graph(5)->setData(new_temp, new_sample3_slope);
        customPlot->graph(5)->setPen(QPen(Qt::darkMagenta));
     }
     else
     {
        customPlot->graph(2)->setVisible(false);
      //  customPlot->graph(5)->setVisible(false);
     }


     customPlot->yAxis->setRange(min_range-100, max_range+100);
     customPlot->xAxis->setRange(new_temp[0], new_temp[new_temp.size() - 1]);
     customPlot->yAxis2->setRange(min_slope-100, max_slope+100);
     customPlot->xAxis2->setRange(new_temp[0], new_temp[new_temp.size() - 1]);


    customPlot->replot();
    qDebug()<<"finished replotting";


    customPlot->savePng(filename_pdf);


    temp_array_final.clear();
    sample1_norm.clear();
    sample2_norm.clear();
    sample3_norm.clear();
    new_sample1.clear();
    new_sample2.clear();
    new_sample3.clear();
    new_sample1_slope.clear();
    new_sample2_slope.clear();
    new_sample3_slope.clear();
    new_temp.clear();



    /****************************/

    model->refresh(model->index(storedevice));
    pdf_report();

}

void MainWindow::on_pushButton_21_clicked() //up
{
    int i = ui->comboBox->currentIndex();
    if(i == 0)
        qr1.adjust(0,-5,0,-5);
    if(i == 1)
        qr2.adjust(0,-5,0,-5);
    if(i == 2)
        qr3.adjust(0,-5,0,-5);

}

void MainWindow::on_pushButton_22_clicked() //down
{

    int i = ui->comboBox->currentIndex();
    if(i == 0)
        qr1.adjust(0,5,0,5);
    if(i == 1)
        qr2.adjust(0,5,0,5);
    if(i == 2)
        qr3.adjust(0,5,0,5);
}

void MainWindow::on_pushButton_24_clicked() // right
{
    int i = ui->comboBox->currentIndex();
    if((i == 0)&&(qr1.x() < (qr2.x()-qr2.width())))
        qr1.adjust(5,0,5,0);
    if((i == 1)&&(qr2.x()<(qr3.x()-qr3.width())))
        qr2.adjust(5,0,5,0);
    if(i == 2)
        qr3.adjust(5,0,5,0);
}

void MainWindow::on_pushButton_23_clicked() //left
{
    int i = ui->comboBox->currentIndex();
    if(i == 0)
        qr1.adjust(-5,0,-5,0);
    if((i == 1)&&(qr2.x()>(qr1.x()+qr1.width())))
        qr2.adjust(-5,0,-5,0);
    if((i == 2)&&(qr3.x() >(qr2.x()+qr3.width())))
        qr3.adjust(-5,0,-5,0);
}

void MainWindow::on_pushButton_20_clicked() // up  button in optical calibration.
{
    //   CvRect cvRect1;
    cvRect1.x = qr1.x();
    cvRect1.y = qr1.y();
    cvRect1.width = qr1.width();
    cvRect1.height = qr1.height();

    //    CvRect cvRect2;
    cvRect2.x = qr2.x();
    cvRect2.y = qr2.y();
    cvRect2.width = qr2.width();
    cvRect2.height = qr2.height();

    //   CvRect cvRect3;
    cvRect3.x = qr3.x();
    cvRect3.y = qr3.y();
    cvRect3.width = qr3.width();
    cvRect3.height = qr3.height();

    area_x=area_x+temp_area_x;//11425
    area_y=area_y+temp_area_y;//11425

    qDebug()<<"image area x-axis"<<area_x;
    qDebug()<<"image area y-axis"<<area_y;
    qDebug()<<"cvRect1.x"<<cvRect1.x<<"cvRect1.y"<<cvRect1.y<<"cvRect1.width"<<cvRect1.width<<"cvRect1.height"<<cvRect1.height;
    qDebug()<<"cvRect1.x"<<cvRect2.x<<"cvRect2.y"<<cvRect2.y<<"cvRect2.width"<<cvRect2.width<<"cvRect1.height"<<cvRect2.height;
    qDebug()<<"cvRect1.x"<<cvRect3.x<<"cvRect3.y"<<cvRect3.y<<"cvRect3.width"<<cvRect3.width<<"cvRect1.height"<<cvRect3.height;
    //qdebug() << "Inside update button";

    update_Profile();
    emit emit_update_roi(cvRect1,cvRect2,cvRect3);
    emit send_image_area_para(area_x,area_y);
    temp_area_x=0;
    temp_area_y=0;
    QMessageBox::information(this,"User Alert","Image Updated");
}


///////////////////////////////////////////////////////////////////////////////
//  RobotNavigator::recordingSlot
//!
//! @brief Slots called when a clicked signal is emitted by the recording button
//!
//! Here we don't want a file dialog popping up on a record click. We generate
//! a temporary file name,  hpare the recording and start the blinking timer.
//! On a second click we close the media file, stop the blinking timer and
//! finally pop the file dialog.
////////////////////////////////////////////////////////////////////////////////
void MainWindow::recording_Start()
{
    //qdebug() << "inside recording start";
    // QTime dateTime = QTime::currentTime();
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString(Qt::ISODate);
    dateTimeString.remove(" ");
    dateTimeString.remove("-");
    dateTimeString.remove(":");
    dateTimeString.replace("T","_");
    ////qdebug() << "filename : " << filename;

    filename = "./install/bin/ffmpeg -f rawvideo -pix_fmt rgb32 -s 400x300 -i - ";




    filename11.clear();
    filename11.append("/results/"); // Pen drive
    filename11.append(ui->lineEdit_35->text());//batch number
    filename11.append(ui->lineEdit_25->text()); // chemical name
    filename11.append(dateTimeString); // Date & Time

    if(QDir(filename11).exists())
    {
        filename11.append("_1");
        QDir().mkdir(filename11);

    }
    else
    {
        QDir().mkdir(filename11);

    }


    filename.append("/results/");
    filename.append(ui->lineEdit_35->text());
    filename.append(ui->lineEdit_25->text());
    filename.append(dateTimeString);
    filename.append("/");
    filename.append(ui->lineEdit_35->text());
    filename.append(ui->lineEdit_25->text());
    filename.append(dateTimeString);
    filename.append(".avi");
    ffmpeg_filename = new char[filename.length() + 1];
    strcpy(ffmpeg_filename,filename.toLatin1().constData());

    ffmpeg = popen(ffmpeg_filename,"w");
    if(ffmpeg == NULL)
    {

    }
    else
    {

        recording = true;
    }

    filename.clear();
    filename.append(filename11);
    filename.append("/");
    filename.append(ui->lineEdit_35->text());
    filename.append(ui->lineEdit_25->text());
    filename.append(dateTimeString);
    filename.append(".avi");
}

void MainWindow::recording_Stop()
{

    recording = false;
    int status = pclose(ffmpeg);

    if(status == -1)
    {

    }
    else
        ffmpeg_filename = NULL; // possible mem leakage
}

void MainWindow::save_movie(QString x)
{

    x=" ";
    if(save_mov)
    {

        save_mov = false;

    }


}




void MainWindow::setup_player()
{

    pp = new char[sPath.length() + 1];
    strcpy(pp, sPath.toLatin1().constData());

    av_register_all();

    if(avformat_open_input(&pFormatCtx, pp, NULL, NULL)!=0)
    {

        return;
    }


    if(avformat_find_stream_info(pFormatCtx, NULL)<0)
    {

        return;
    }


    av_dump_format(pFormatCtx, 0, pp, 0);


    videoStream=-1;
    for(unsigned int i=0; i<pFormatCtx->nb_streams; i++)
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoStream=i;
            break;
        }

    if(videoStream==-1)
    {

        return;
    }


    pCodecCtx=pFormatCtx->streams[videoStream]->codec;
    video_st = pFormatCtx->streams[videoStream];

    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL)
    {

        return;
    }


    if(avcodec_open2(pCodecCtx, pCodec, NULL)<0)
    {

        return;

    }

    // Allocate video frame
    pFrame=avcodec_alloc_frame();

    // Allocate an AVFrame structure
    pFrameRGB=avcodec_alloc_frame();
    if(pFrameRGB==NULL)
    {
        //qdebug() << "pFrameRGB is Null!!";
        return;
    }
    // Determine required buffer size and allocate buffer
    numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
                                pCodecCtx->height);
    buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture
    avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,
                   pCodecCtx->width, pCodecCtx->height);
}

void MainWindow::loop_player()
{
    // int j=0;//char filename[32];
    //    Ti=0;
    if(av_read_frame(pFormatCtx, &packet) >= 0)
    {
        //j++;
        // Is this a packet from the video stream?
        if(packet.stream_index==videoStream)
        {
            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frame_finished,
                                  &packet);
            // Did we get a video frame?
            if(frame_finished)
            {
                struct SwsContext *img_convert_ctx;
                int w = video_st->codec->width;
                int h = video_st->codec->height;
                //int scaled_w = w / 3;
                //int scaled_h = h / 3;
                int scaled_w = w;
                int scaled_h = h;

                img_convert_ctx = sws_getContext(w, h,
                                                 video_st->codec->pix_fmt,
                                                 scaled_w, scaled_h, PIX_FMT_RGB24, SWS_BICUBIC,
                                                 NULL, NULL, NULL);
                if(img_convert_ctx == NULL)
                {
                    //qdebug() << "Cannot initialize the conversion context!\n";
                    return;
                }
                sws_scale(img_convert_ctx, (const uint8_t * const*)pFrame->data,
                          pFrame->linesize, 0,
                          video_st->codec->height,
                          pFrameRGB->data, pFrameRGB->linesize);



                QImage img = QImage(scaled_w,scaled_h, QImage::Format_RGB888);

                for (int y=0; y<scaled_h; y++)
                    memcpy(img.scanLine(y),pFrameRGB->data[0]+y*pFrameRGB->linesize[0],scaled_w*3);


                emit frameready(img,1);

                sws_freeContext(img_convert_ctx);
            }
        }
        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);
    }
    else
    {
        emit movie_completed();
    }
}

void MainWindow::stop_player()
{
    av_dump_format(pFormatCtx, 0, pp, 0);

    // Free the RGB image
    av_free(buffer);
    av_free(pFrameRGB);

    // Free the YUV frame
    av_free(pFrame);

    // Close the codec
    avcodec_close(pCodecCtx);

    // Close the video file
    //av_close_input_file(pFormatCtx);
    avformat_close_input(&pFormatCtx);

}

void MainWindow::start_player()
{
    ui->treeView->setEnabled(false);
    setup_player();
    mov_timer->start(42);
}

void MainWindow::receive_stop_movie_display(bool image_flag) // this needs to be handled carefully.
{

    ui->treeView->setEnabled(true);
    ui->tabWidget->setTabEnabled(0,true);
    ui->tabWidget->setTabEnabled(1,true);
    ui->tabWidget->setTabEnabled(2,true);
    ui->tabWidget->setTabEnabled(3,true);
    ui->tabWidget->setTabEnabled(4,true);
    ui->tabWidget->setTabEnabled(5,true);
    ui->tabWidget->setCurrentIndex(3);
    ui->open_button->setEnabled(true);
    if(image_flag == false)
    {
        movie_display->hide(); // this needs to be done earlier. other wise double clicking stop causes issues.
    }
    else
    {
        stop_clicked = true;
        pause = false;
        //ui->pushButton_99->setText("Play");
        mov_timer->stop();
        stop_player();
        movie_display->hide();

    }
}


void MainWindow::receive_play_pause_movie_display(char var)
{
    //qdebug() << "inside pause";
    if(!stop_clicked)
    {
        pause ^= 1;
        if(var == 2)
        {
            stop_player();

            setup_player();
            pause = 0;
            var = 0;
        }
        if(pause)
        {

            mov_timer->stop();
        }
        else
        {

            mov_timer->start(42);
        }
    }
    else
    {
        stop_clicked = false;
        start_player();
    }
}

/*void MainWindow::on_pushButton_26_clicked() // wide
{
    //int i = ui->comboBox->currentIndex();
   // if(i == 0)
        qr1.adjust(-2,0,2,0);
    //if(i == 1)
        qr2.adjust(-2,0,2,0);
    //if(i == 2)
        qr3.adjust(-2,0,2,0);
}

void MainWindow::on_pushButton_28_clicked() //thin
{
    //int i = ui->comboBox->currentIndex();
    //if(i == 0)
        qr1.adjust(2,0,-2,0);
    //if(i == 1)
        qr2.adjust(2,0,-2,0);
    //if(i == 2)
        qr3.adjust(2,0,-2,0);
}


void MainWindow::on_pushButton_27_clicked() //tall
{
   // int i = ui->comboBox->currentIndex();
   // if(i == 0)
        qr1.adjust(0,-2,0,2);
   // if(i == 1)
        qr2.adjust(0,-2,0,2);
   // if(i == 2)
        qr3.adjust(0,-2,0,2);
}

void MainWindow::on_pushButton_29_clicked() //short
{
   // int i = ui->comboBox->currentIndex();
    //if(i == 0)
        qr1.adjust(0,2,0,-2);
    //if(i == 1)
        qr2.adjust(0,2,0,-2);
   // if(i == 2)
        qr3.adjust(0,2,0,-2);
}*/

void MainWindow::update_status(wchar_t x)
{
    //qdebug() << "in update status";
    if(x == 0)
    {
        toogle_status=0;
        ui->label_25->setText("Please load the samples ...");
        ui->label_25->show();
        connect(blink_timer,SIGNAL(timeout()),this,SLOT(staus_message()));
        blink_timer->start(500);//11425
    }
    else if(x == 1)
    {

        ui->label_25->setText("<font color='black'>Melting process started</font>");
        ui->label_25->show();
        disconnect(blink_timer,SIGNAL(timeout()),this,SLOT(staus_message()));
        blink_timer->stop();//11425
    }
    else if(x == 2)
    {
        ui->label_25->setText("<font color='black'>wait ...</font>");
        ui->label_25->show();
        disconnect(blink_timer,SIGNAL(timeout()),this,SLOT(staus_message()));
        blink_timer->stop();//11425
    }
    else
    {
        ui->label_25->setText("<font color='black'>Cooling process started</font>");
        ui->label_25->show();
        disconnect(blink_timer,SIGNAL(timeout()),this,SLOT(staus_message()));
        blink_timer->stop();//11425
    }
    //qdebug() << "in update status1";
}


bool removeDir(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName))
    {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
        {
            if (info.isDir())
            {
                result = removeDir(info.absoluteFilePath());
            }
            else
            {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result)
            {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

void MainWindow::on_delete_button_clicked()
{

  bool result;
    QModelIndex Index = ui->treeView->currentIndex();
    QString sPath1 = model->fileInfo(Index).absoluteFilePath();
    QString extn = sPath1.right(4);

   QMessageBox msgBox(this);
   msgBox.setWindowTitle("User Alert");
    msgBox.setText("Do You Want To Delete The File?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

    msgBox.setButtonText( QMessageBox::Ok, "Delete" );

    int ret = msgBox.exec();
    switch (ret)
{
    case QMessageBox::Ok:

{
        if(!Index.isValid())
            return;
        if(QDir(sPath1).exists())
        {
            result = removeDir(sPath1);
            if(result == true)
            {

                QMessageBox::information(this,"User Alert","File Deleted");

            }

        }
        else if((extn == ".pdf")||(extn == ".txt")||(extn == ".avi")/*||(extn == ".png")*/||(extn == ".jpg"))
        {
            model->remove(Index);

            QMessageBox::information(this,"User Alert","File Has Been Deleted");
        }
        else
        {

            QMessageBox::information(this,"User Alert","Driver files can't be deleted");
        }
        model->refresh(model->index(storedevice));

        }

        break;
    case QMessageBox::Cancel:

        break;
    default:

        break;
}

}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if(index == 4)
        {
        //ui->tabWidget_3->setCurrentIndex(0);
        if(ui->tabWidget_3->currentIndex()==2)
        {
            read_rtc();                          //vinay------ on selection of tab, date updates.
                ui->lineEdit_day->setText(QString::number(day));
                ui->lineEdit_month->setText(QString::number(month));               //vinay
                ui->lineEdit_year->setText(QString::number(year));
                ui->lineEdit_hour->setText(QString::number(hour));
                ui->lineEdit_minutes->setText(QString::number(minute));
                ui->lineEdit_seconds->setText(QString::number(sec));
        }
            if(ui->tabWidget_3->currentIndex() == 0)
            {
                temp_area_x=temp_area_y=0;//11425
                camera_calib_image_reading();//11425

            }
            else
            {
                if(camera_once_flag == true)
                {
                    camera_once_flag = false;
                    camera_timer->stop();
                    emit call_stop_camera();
                }
            }
        }
        else
        {
            if(camera_once_flag == true)
            {
                camera_once_flag = false;
                camera_timer->stop();
                emit call_stop_camera();
            }
        }

        //11425
        if(index==3)        //vinay -----results
        {
                    model->refresh(model->index(storedevice));
                    qDebug() << "refresh index - model";

        }
}

void MainWindow::on_tabWidget_3_currentChanged(int index)
{
    if(index == 0)
    {
        temp_area_x=temp_area_y=0;//11425
        if(!camera_timer->isActive())
        {
            camera_once_flag = true;
            //qdebug() << "timer is not active";
            emit call_setup_camera();

            //qdebug() << "camera setup";
            camera_timer->start(500); // why do we need to start the camera on index 0? shudnt we do it only upon start melt?
            //qdebug() << "camera started";
        }
    }
    else
    {
        if(camera_once_flag == true)
        {
            camera_once_flag = false;
            camera_timer->stop();
            emit call_stop_camera();
        }
       /* if(index==1)
        {
            if(!calib_flag)
            ui->tabWidget_4->setCurrentIndex(0);
        }*/
        if(index==3)
        {
           user_push_buttons_hide();
           user_hide();
           Authorised_show();
        }
        if(index == 2)
        {

            read_rtc();
            ui->lineEdit_day->setText(QString::number(day));
            ui->lineEdit_month->setText(QString::number(month));
            ui->lineEdit_year->setText(QString::number(year));
            ui->lineEdit_hour->setText(QString::number(hour));
            ui->lineEdit_minutes->setText(QString::number(minute));
            ui->lineEdit_seconds->setText(QString::number(sec));
        }
        if(index == 5)
        {
            ui->lineEdit_GATEWAY1->setText(gw1);
            ui->lineEdit_GATEWAY2->setText(gw2);
            ui->lineEdit_GATEWAY3->setText(gw3);
            ui->lineEdit_GATEWAY4->setText(gw4);

            ui->lineEdit_MASK1->setText(mk1);
            ui->lineEdit_MASK2->setText(mk2);
            ui->lineEdit_MASK3->setText(mk3);
            ui->lineEdit_MASK4->setText(mk4);

            ui->lineEdit_IP1->setText(ip1);
            ui->lineEdit_IP2->setText(ip2);
            ui->lineEdit_IP3->setText(ip3);
            ui->lineEdit_IP4->setText(ip4);
        }
    }


}

void MainWindow::receive_display_pv()
{
    bool decimal_flag = false;
    //samples offset and mulfactor

   calculate_offset_mulfactor();

    int xxx = block_temp * 10;
    if((xxx%10) == 0)
    {
        decimal_flag = true;

    }

    QString x = QString::number(block_temp);
    // ovrsht_timer->start(1000);
    if(decimal_flag == true)
    {
        x.append(".0");

    }



    if(block_temp > -2 && block_temp < 402)
    {
        ui->label_36->setText(x);
        ui->label_17->setText(x);
    }
    else
    {
        if(block_temp <= -2)
        {

            ui->label_36->setText("Temp Low");
        }
        else
        {

            ui->label_36->setText("Temp High");
        }
    }
    if((fanon==false))
    {
        if((block_temp>=40))
        {

            end_temp = curr_profile->Stop_temp;
            ramp_rate = curr_profile->Ramp_rate;

            emit fan_setpoint(40);

        }

    }


}

void MainWindow::on_pushButton_19_clicked()
{
    char buf[40];
    day = ui->lineEdit_day->text().toInt();

    month = ui->lineEdit_month->text().toInt();

    year = ui->lineEdit_year->text().toInt();

    hour = ui->lineEdit_hour->text().toInt();

    minute = ui->lineEdit_minutes->text().toInt();

    sec = ui->lineEdit_seconds->text().toInt();

    sprintf(buf, "/bin/date -s %04d.%02d.%02d-%02d:%02d:%02d", year, month, day, hour, minute,sec);

    system(buf);

    system("/sbin/hwclock -w");


        serial_no =ui->lineEdit_36->text();
        equ_id  =ui->lineEdit_70->text();
        // equ_id =ui->lineEdit_70->text();

    update_Profile();
    QMessageBox::information(this,"User Alert","Data Updated");

}

void MainWindow::on_open_button_clicked()
{

    sPath = model->filePath(ui->treeView->currentIndex());

    qDebug() <<"Spath in open button: " << sPath;
    ui->open_button->setEnabled(false);


    if(treeview_open_flag == true)
    {


        QString extn = sPath.right(3);
        QMessageBox msgBox;
        QString errMsg;



        if ((extn == "png")||(extn == "jpg"))
        {
            QImage img;
            img.load(sPath);
            movie_display->show();
            ui->treeView->setEnabled(false);
            emit frameready(img,0);

            ui->tabWidget->setTabEnabled(0,false);
            ui->tabWidget->setTabEnabled(1,false);
            ui->tabWidget->setTabEnabled(2,false);
            ui->tabWidget->setTabEnabled(3,false);
            ui->tabWidget->setTabEnabled(4,false);
            ui->tabWidget->setTabEnabled(5,false);
            ui->tabWidget->setCurrentIndex(3);

        }
        else if (extn == "avi" || extn == "mpg")
        {
            pause = false;
            //ui->pushButton_99->setText("Pause");
            stop_clicked = false;
            movie_display->show();
            //ui->frame->show();
            start_player();
            ui->tabWidget->setTabEnabled(0,false);
            ui->tabWidget->setTabEnabled(1,false);
            ui->tabWidget->setTabEnabled(2,false);
            ui->tabWidget->setTabEnabled(3,false);
            ui->tabWidget->setTabEnabled(4,false);
            ui->tabWidget->setTabEnabled(5,false);
            ui->tabWidget->setCurrentIndex(3);
        }
        else if (extn == "pdf")
        {
            errMsg.append("This version of software does not support pdf files.");
            msgBox.critical(0,"Error",errMsg);
            msgBox.setFixedSize(500,200);
            ui->tabWidget->setTabEnabled(0,false);
            ui->tabWidget->setTabEnabled(1,false);
            ui->tabWidget->setTabEnabled(2,false);
            ui->tabWidget->setTabEnabled(3,false);
            ui->tabWidget->setTabEnabled(4,false);
            ui->tabWidget->setTabEnabled(5,false);
            ui->tabWidget->setCurrentIndex(3);
        }
    }
    ui->open_button->setEnabled(true);
}




//11425
void MainWindow::receive_from_login(char vvv,int receive_usr)
{
    ui->tabWidget->setEnabled(true);
    if(vvv == 0)
        password_ok_flag = true;
    else
        password_ok_flag = false;
    if(password_ok_flag == true)
    {
        login_timer->stop();//11425
       // disconnect(login_timer,SIGNAL(timeout()),this,SLOT(raise_login_form()));
        login_form->close();
        calib_receive_usr=receive_usr;
        loged_user=receive_usr;
        update_Profile();
        qDebug() << "instrument login user name:" <<calib_receive_usr;
        //qdebug() << "Password Matched" << index_tabwidget << update_index;
        if(receive_usr !=12)
        {
        ui->tabWidget_4->removeTab(0);
        ui->tabWidget_5->removeTab(0);
        ui->tabWidget_4->setStyleSheet("QTabBar::tab { height: 40px; width: 393px;font: 11pt }");
        ui->tabWidget_5->setStyleSheet("QTabBar::tab { height: 40px; width: 393px;font: 11pt }");

        }
        if((receive_usr==12)||(receive_usr==10))
        {
            screen = true;

            ui->label_17->show();
            ui->label_5->show();
            ui->label_6->show();
            ui->label_7->show();
            ui->label_8->show();
            ui->lineEdit->show();
            ui->lineEdit_2->show();
            ui->label_2->show();
            ui->label_4->show();
            ui->pushButton->show();
            ui->tabWidget_2->setTabEnabled(1,true);
            ui->single_sel_button->setEnabled(true);//14547 07-10-14
            ui->lineEdit_70->setEnabled(true);//14547 eqiup id

            report_tags_show();//14547

            if(receive_usr==12)
            {
                ui->label_3->show();
                ui->label_18->show();
                ui->lineEdit_34->setText("pol");//11425
                ui->lineEdit_36->setEnabled(true);
                factory_signin=true;
                authorized_signin=false;
            }
            else
            {
                ui->label_3->hide();
                ui->label_18->hide();
                ui->lineEdit_34->setText(user_name[10]);
                ui->lineEdit_36->setEnabled(false);
                factory_signin=false;
                authorized_signin=true;


            }

           // calib_receive_usr=0;
        }
        else
        {
            screen = false;
            ui->single_sel_button->setEnabled(false);//14547 07-10-14
            ui->lineEdit_34->setText(user_name[receive_usr]);//11425
            ui->label_17->hide();
            ui->label_5->hide();
            ui->label_6->hide();
            ui->label_7->hide();
            ui->label_8->hide();
            ui->lineEdit->hide();
            ui->lineEdit_2->hide();
            ui->label_2->hide();
            ui->label_4->hide();
            ui->pushButton->hide();
            ui->tabWidget_2->setTabEnabled(1,false);
            ui->lineEdit_70->setEnabled(false);// equip id disable for normal usr
            //ui->tabWidget_4->setStyleSheet("QTabBar::tab { height: 40px; width: 365px;font: 11pt }");
            //ui->tabWidget_5->setStyleSheet("QTabBar::tab { height: 40px; width: 375px;font: 11pt }");


            report_tags_hide();
        }
        ui->lineEdit_34->setEnabled(false);
        ui->tabWidget->setCurrentIndex(0);//11425
        ui->tabWidget->setTabEnabled(1,true);
        ui->tabWidget->setTabEnabled(2,true);
        ui->tabWidget->setTabEnabled(3,true);
        ui->tabWidget->setTabEnabled(4,true);
        ui->tabWidget->setTabEnabled(5,true);

        //index_tabwidget = update_index;
    }
    ui->tabWidget->show(); //vinay
    if (full_percentage >= 90)// check1 sdcard checkup--vinay
       {
           QMessageBox::information(this,"User Alert","Memory Full:Please Backup The Memory");
       }

    QString str(calib_alert_date);
    QDate converted_date(QDate::fromString(str, "dd/MM/yyyy"));

    qDebug() << "calib_alert_date"<<calib_alert_date;
    if((current_date1 >= converted_date))//vinay---calibration due date alert

    {
        QMessageBox::information(this,"User Alert"," Calibration due date completed,please recalibrate");
    }

}//11425

void MainWindow::on_pushButton_54_clicked()
{
    QString Ip1,Ip2,Ip3,Ip4,Mk1,Mk2,Mk3,Mk4,Gw1,Gw2,Gw3,Gw4;
    Ip1.append(ui->lineEdit_IP1->text());
    Ip2.append(ui->lineEdit_IP2->text());
    Ip3.append(ui->lineEdit_IP3->text());
    Ip4.append(ui->lineEdit_IP4->text());

    Mk1.append(ui->lineEdit_MASK1->text());
    Mk2.append(ui->lineEdit_MASK2->text());
    Mk3.append(ui->lineEdit_MASK3->text());
    Mk4.append(ui->lineEdit_MASK4->text());

    Gw1.append(ui->lineEdit_GATEWAY1->text());
    Gw2.append(ui->lineEdit_GATEWAY2->text());
    Gw3.append(ui->lineEdit_GATEWAY3->text());
    Gw4.append(ui->lineEdit_GATEWAY4->text());
   // if((Ip1=="+")||(Ip2=="+")||(Ip3=="+")||(Ip4=="+")||(Mk1=="+")||(Mk2=="+")||(Mk3=="+")||(Mk4=="+")||(Gw1=="+")||(Gw2=="+")||(Gw3=="+")||(Gw4=="+"))
  //  {
   //    QMessageBox::critical(this,"Error","Special characters are not allowed");
//    }
   // else

        ip1.clear();
        ip2.clear();
        ip3.clear();
        ip4.clear();


        mk1.clear();
        mk2.clear();
        mk3.clear();
        mk4.clear();

        gw1.clear();
        gw2.clear();
        gw3.clear();
        gw4.clear();

        ip.clear();
        ip.append("IP=");
        ip.append(ip1.append(ui->lineEdit_IP1->text()));
        ip.append(".");
        ip.append(ip2.append(ui->lineEdit_IP2->text()));
        ip.append(".");
        ip.append(ip3.append(ui->lineEdit_IP3->text()));
        ip.append(".");
        ip.append(ip4.append(ui->lineEdit_IP4->text()));

        mask.clear();
        mask.append("MASK=");
        mask.append(mk1.append(ui->lineEdit_MASK1->text()));
        mask.append(".");
        mask.append(mk2.append(ui->lineEdit_MASK2->text()));
        mask.append(".");
        mask.append(mk3.append(ui->lineEdit_MASK3->text()));
        mask.append(".");
        mask.append(mk4.append(ui->lineEdit_MASK4->text()));

        gateway.clear();
        gateway.append("GATEWAY=");
        gateway.append(gw1.append(ui->lineEdit_GATEWAY1->text()));
        gateway.append(".");
        gateway.append(gw2.append(ui->lineEdit_GATEWAY2->text()));
        gateway.append(".");
        gateway.append(gw3.append(ui->lineEdit_GATEWAY3->text()));
        gateway.append(".");
        gateway.append(gw4.append(ui->lineEdit_GATEWAY4->text()));

        QFile file("/etc/eth0-setting");
        if(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            QTextStream out(&file);

            out << ip << "\n";
            out << mask << "\n";
            out << gateway << "\n";
            out << "DNS=192.168.1.1" << "\n";
            out << "MAC=08:90:90:90:90:90" << "\n";

        }
        file.close();
        update_Profile();//11425
        QMessageBox::information(this,"User Alert","Configuration Updated");
   // }
    return;
}
//11425


//11425
void MainWindow::on_single_sel_button_clicked()
{
    onset_clear^=1;
    ui->lineEdit_29->setEnabled(!onset_clear);
    ui->lineEdit_30->setEnabled(!onset_clear);
    ui->lineEdit_31->setEnabled(onset_clear);
}

void MainWindow::raise_login_form()
{
    login_form->show();//11425
    login_form->raise();//11425
}


//sample calibration buttons
void MainWindow::on_pushButton_99_clicked()//start button
{
    sample_profile_sel=0;
    calib_flag=true;
    update_sample_params(0);
}

void MainWindow::on_pushButton_102_clicked()
{
    sample_profile_sel=1;

    calib_flag=true;
    update_sample_params(1);
}

void MainWindow::on_pushButton_101_clicked()
{
    sample_profile_sel=2;

    calib_flag=true;
    update_sample_params(2);
}

void MainWindow::on_pushButton_98_clicked()
{
    sample_profile_sel=3;

    calib_flag=true;
    update_sample_params(3);
}

void MainWindow::receive_from_sample_form(bool ok_cancel,int days)
{
    if(ok_cancel)
    {      

        if(factory==true)
        {
            calib_factory_months=days;
        }
        else
        {
           calib_days=days;
        }
        calc_mulfactor_offset();
        calib_usr=calib_receive_usr;
       // calib_usr=loged_user;
        final_Noofsamples=Noofsamples;
        ui->tabWidget_8->setTabEnabled(0,true);
        ui->tabWidget_8->setCurrentIndex(0);
        calib_cmplt=1;
        update_Profile();
        if(factory_pdf==true)
        {
          calibration_paasword_hide();
          ui->lineEdit_89->hide();
          ui->lineEdit_88->hide();
          ui->pushButton_91->hide();
          ui->pushButton_92->hide();
          ui->label_150->hide();
          ui->label_149->hide();
          ui->factory_calib_button->show();
          ui->factory_calib_button->setEnabled(false);
          calib_pdf_report=true;//14547
          pdf_report();//14547
          factory_pdf=false;
          calib_pdf_report=false;
          reset=false;


        }
         if(field_pdf==true)
        {

             calibration_paasword_show();
             ui->factory_calib_button->hide();
             ui->field_calib_button->hide();
             ui->reset->hide();
             ui->pushButton_18->setEnabled(true);
             ui->pushButton_2->setEnabled(false);
             ui->pushButton_16->setEnabled(false);
             ui->lineEdit_35->setEnabled(true);//11425
            calib_pdf_report=true;//14547
            pdf_report();//14547
            field_pdf=false;
            calib_pdf_report=false;
            ui->tabWidget_8->setTabEnabled(3,false);
            field_reset=false;

            for(int i=0;i<Noofsamples;i++)
            {

                cfield_past[i]=cfield_past[i]+cfield[i];

            }


        }
       //14547


        hide_sample_pushbuttons();
        calibration_History();
        clear_fields();
        ui->tabWidget_8->setTabEnabled(3,false);

    }
    else
    {
        ui->pushButton_99->setEnabled(true);
        ui->pushButton_102->setEnabled(true);
        ui->pushButton_101->setEnabled(true);
        ui->pushButton_98->setEnabled(true);
        ui->pushButton_100->setEnabled(false);
        ui->tabWidget_8->setTabEnabled(0,true);
        ui->tabWidget_8->setCurrentIndex(0);
        clear_fields();
    }
    ui->tabWidget->setTabEnabled(0,true);
    ui->tabWidget->setTabEnabled(1,true);
    ui->tabWidget->setTabEnabled(2,true);
    ui->tabWidget->setTabEnabled(3,true);
    ui->tabWidget->setTabEnabled(4,true);
    ui->tabWidget->setTabEnabled(5,true);
    ui->tabWidget_2->setTabEnabled(0,true);
    ui->tabWidget_2->setTabEnabled(1,true);
    ui->tabWidget_2->setTabEnabled(2,true);
    ui->tabWidget_3->setTabEnabled(0,true);
    ui->tabWidget_3->setTabEnabled(1,true);
    ui->tabWidget_3->setTabEnabled(2,true);
    ui->tabWidget_3->setTabEnabled(3,true);
    ui->tabWidget_3->setTabEnabled(4,true);
    ui->tabWidget_3->setTabEnabled(5,true);
    ui->tabWidget_4->setTabEnabled(0,true);
    ui->tabWidget_4->setTabEnabled(1,true);
    ui->tabWidget_4->setTabEnabled(2,true);
    ui->tabWidget_5->setTabEnabled(0,true);
    ui->tabWidget_5->setTabEnabled(1,true);
    ui->tabWidget_5->setTabEnabled(2,true);

    if(loged_user==12)               //vinay
    {
        ui->lineEdit_34->setText("pol");
        qDebug()<<"login as user name"<<"pol";

    }
    else
    {
        qDebug()<<"login as user name"<<user_name[loged_user];

    ui->lineEdit_34->setText(user_name[loged_user]);
    }


}
void MainWindow::update_sample_params(int i)
{
    ui->tabWidget->setCurrentIndex(0);
    QString str;
    ui->lineEdit_25->setText(sample_calib_name[i]);
    ui->lineEdit_34->text().clear();

    ui->lineEdit_34->setText(operator_name);

    str = QString::number((cmp_fac[i]-5.0));  //make it 5 -- for testing purpose changed
    ui->lineEdit_26->setText(str);

    str = QString::number((cmp_fac[i]+5.0)); //make it 5 -- for testing purpose changed
    ui->lineEdit_27->setText(str);

    str = QString::number(1.0); //make it 1.0 --- for testing changed to 5.0
    ui->lineEdit_28->setText(str);

    //str = QString::number(25);//10 to 25
    str = QString::number(20);//14547
    ui->lineEdit_29->setText(str);

    str = QString::number(50);
    ui->lineEdit_31->setText(str);

    //str = QString::number(85);
    str = QString::number(80);//14547
    ui->lineEdit_30->setText(str);

    ui->lineEdit_29->setEnabled(1);//11425
    ui->lineEdit_30->setEnabled(1);//11425
    ui->lineEdit_31->setEnabled(0);//11425
    ui->single_sel_button->setChecked(0);//11425

    ui->automatic->setChecked(true);
    manual_operation=0;
    onset_clear=false;

    mainscreen();
    if((factory_signin==true)||(authorized_signin==true))
    {
         ui->lineEdit_30->setEnabled(true);
         ui->lineEdit_29->setEnabled(true);
    }

}
void MainWindow::mainscreen()
{
    ui->tabWidget->setTabEnabled(1,false);
    ui->tabWidget->setTabEnabled(2,false);
    ui->tabWidget->setTabEnabled(3,false);
    ui->tabWidget->setTabEnabled(4,false);
    ui->tabWidget->setTabEnabled(5,false);
    ui->lineEdit_25->setEnabled(false);
    ui->lineEdit_26->setEnabled(false);
    ui->lineEdit_27->setEnabled(false);
    ui->lineEdit_28->setEnabled(false);
    ui->lineEdit_29->setEnabled(false);
    ui->lineEdit_31->setEnabled(false);
    ui->lineEdit_30->setEnabled(false);
    ui->manual->setEnabled(false);
    ui->automatic->setEnabled(false);
    ui->single_sel_button->setEnabled(false);

}
void MainWindow::calc_mulfactor_offset()
{
    QString calib_date_str,calib_date_str1,calib_date_str2 ;
    if (factory==true)                     //vinay
    {
        calib_date_str.clear();
        calib_date_str.append(QDate::currentDate().toString("dd/MM/yyyy"));//format of date
        calib_factory_date=calib_date_str;

        calib_date_str1 .clear();
        calib_date_str1.append(QDate::currentDate().addMonths(calib_factory_months).toString("dd/MM/yyyy")); //vinay
        calib_factory_duedate=calib_date_str1;
    }

    else                                       //vinay
    {
    calib_date_str.clear();
    calib_date_str.append(QDate::currentDate().toString("dd/MM/yyyy"));//format of date
    calib_date=calib_date_str;

    calib_date_str1 .clear();
    calib_date_str1.append(QDate::currentDate().addDays(calib_days).toString("dd/MM/yyyy")); //vinay
    calib_due_date=calib_date_str1;

    calib_date_str2.clear();
    calib_date_str2.append(QDate::currentDate().addDays(calib_days-1).toString("dd/MM/yyyy")); //vinay
    calib_alert_date=calib_date_str2;
    }
    hiding_calib(true);
   //update_Profile();
    hide_sample_pushbuttons();
}

//11425

void MainWindow::on_pushButton_32_clicked()
{

    //alert message
    QMessageBox msgBox(this);


    msgBox.setWindowTitle("User Alert");
    msgBox.setText("Do you want to shutdown the system?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setButtonText( QMessageBox::Ok, "ShutDown" );
    msgBox.setButtonText( QMessageBox::No, "Logoff" );
    int ret = msgBox.exec();
    switch (ret)
    {
    case QMessageBox::Ok:

        shutdown();
        break;


    case QMessageBox::No:


        logoff_form->show();
        logoff_form->raise();
        log_off();


        break;

    case QMessageBox::Cancel:

        break;
    default:

        break;
    }


}


//***************thermo Dynamic Screen validating Function************************//
bool MainWindow::thermo_dynamic_screen_check()
{
    QMessageBox msgBox;
    QString errMsg;
    bool error_bit = false;
    float thermo_ramp_rate1[6];
    thermo_ramp_rate1[0] = ui->lineEdit_3->text().toFloat();
    if (thermo_ramp_rate1[0] > 20.0 || thermo_ramp_rate1[0] < 0)
    {
        error_bit = true;
        errMsg.append("Ramp rate1 must be with in the range 0.0 to 20.0 degC/min.\n");
    }
    thermo_ramp_rate1[1] = ui->lineEdit_9->text().toFloat();
    if (thermo_ramp_rate1[1] > 20.0 || thermo_ramp_rate1[1] < 0)
    {
        error_bit = true;
        errMsg.append("Ramp rate2 must be with in the range 0.0 to 20.0 degC/min.\n");
    }
    thermo_ramp_rate1[2] = ui->lineEdit_11->text().toFloat();
    if (thermo_ramp_rate1[2] > 20.0 || thermo_ramp_rate1[2] < 0)
    {
        error_bit = true;
        errMsg.append("Ramp rate3 must be with in the range 0.0 to 20.0 degC/min.\n");
    }
    thermo_ramp_rate1[3] = ui->lineEdit_14->text().toFloat();
    if (thermo_ramp_rate1[3] > 20.0 || thermo_ramp_rate1[3] < 0)
    {
        error_bit = true;
        errMsg.append("Ramp rate4 must be with in the range 0.0 to 20.0 degC/min.\n");
    }
    thermo_ramp_rate1[4] = ui->lineEdit_17->text().toFloat();
    if (thermo_ramp_rate1[4] > 20.0 || thermo_ramp_rate1[4] < 0)
    {
        error_bit = true;
        errMsg.append("Ramp rate5 must be with in the range 0.0 to 20.0 degC/min.\n");
    }
    thermo_ramp_rate1[5] = ui->lineEdit_18->text().toFloat();
    if (thermo_ramp_rate1[5] > 20.0 || thermo_ramp_rate1[5] < 0)
    {
        error_bit = true;
        errMsg.append("Ramp rate6 must be with in the range 0.0 to 20.0 degC/min.\n");
    }
    for(int i=0;i<6;i++)
    {
        for(int j=i+1;j<6;j++)
        {
            if(thermo_ramp_rate1[i]==thermo_ramp_rate1[j])
            {
                error_bit = true;
                errMsg.append("Ramp rates must be different from each other\n");
                goto end_compare;
            }
        }
    }
end_compare:
    int length = ui->lineEdit_10->text().toInt();
    if (length > 6 || length <2)
    {
        error_bit = true;
        errMsg.append("No.of cycles must be in between 1 to 6\n");
    }

    length = ui->lineEdit_20->text().size();
    if (length > 20 || length <=0)
    {
        error_bit = true;
        errMsg.append("Chemical name cannot be empty and must be less than 20 characters in size.\n");
    }

    float start_temp = ui->lineEdit_22->text().toFloat();
    if (start_temp > 399.0 || start_temp < 25)
    {
        error_bit = true;
        errMsg.append("Start temperature must be with in the range 25 to 399 degC.\n");
    }

    float stop_temp = ui->lineEdit_21->text().toFloat();
    if (stop_temp > 400.0 || stop_temp < start_temp)
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
    }
}

void MainWindow::on_pushButton_33_clicked()
{
    bool valid_flag;
    valid_flag=thermo_dynamic_screen_check();
    if(valid_flag)
    {
    thermo_sample_params();
    thermo_ramp_rate[0]=ui->lineEdit_3->text().toFloat();
    thermo_ramp_rate[1]=ui->lineEdit_9->text().toFloat();
    thermo_ramp_rate[2]=ui->lineEdit_11->text().toFloat();
    thermo_ramp_rate[3]=ui->lineEdit_14->text().toFloat();
    thermo_ramp_rate[4]=ui->lineEdit_17->text().toFloat();
    thermo_ramp_rate[5]=ui->lineEdit_18->text().toFloat();
    No_ramp_cycles = ui->lineEdit_10->text().toInt();
    thermo_chemical_name=ui->lineEdit_20->text();
    thermo_start_temp=ui->lineEdit_22->text().toFloat();
    thermo_stop_temp=ui->lineEdit_21->text().toFloat();
    thermo_correction_factor=ui->lineEdit_37->text().toFloat();

    update_Profile();
     thermo_calib_flag=true;
    }
    else
    {
    ;
    }
}

void MainWindow::on_pushButton_34_clicked()
{
    ui->lineEdit_3->clear();
    ui->lineEdit_9->clear();
    ui->lineEdit_11->clear();
    ui->lineEdit_14->clear();
    ui->lineEdit_17->clear();
    ui->lineEdit_18->clear();
    ui->lineEdit_10->clear();
    ui->lineEdit_20->clear();
    ui->lineEdit_22->clear();
    ui->lineEdit_21->clear();
}
void MainWindow::thermo_sample_params()
{
    ui->tabWidget->setCurrentIndex(0);
    QString str;
    ui->lineEdit_25->setText(ui->lineEdit_20->text());

    str = QString::number(ui->lineEdit_22->text().toFloat());
    ui->lineEdit_26->setText(str);

    str = QString::number(ui->lineEdit_21->text().toFloat());
    ui->lineEdit_27->setText(str);
    if(ramp_cnt==0)
        str = QString::number(ui->lineEdit_3->text().toFloat());
    else if(ramp_cnt==1)
        str = QString::number(ui->lineEdit_9->text().toFloat());
    else if(ramp_cnt==2)
        str = QString::number(ui->lineEdit_11->text().toFloat());
    else if(ramp_cnt==3)
        str = QString::number(ui->lineEdit_14->text().toFloat());
    else if(ramp_cnt==4)
        str = QString::number(ui->lineEdit_17->text().toFloat());
    else if(ramp_cnt==5)
        str = QString::number(ui->lineEdit_18->text().toFloat());
    ui->lineEdit_28->setText(str);

    str = QString::number(15);
    ui->lineEdit_29->setText(str);

    str = QString::number(50);
    ui->lineEdit_31->setText(str);

    str = QString::number(90);
    ui->lineEdit_30->setText(str);

    ui->lineEdit_29->setEnabled(1);//11425
    ui->lineEdit_30->setEnabled(1);//11425
    ui->lineEdit_31->setEnabled(0);//11425
    onset_clear=false;
    mainscreen();

}
void MainWindow::thermo_sample_display()
{
    ui->lineEdit_3->setText(QString::number(thermo_ramp_rate[0]));
    ui->lineEdit_9->setText(QString::number(thermo_ramp_rate[1]));
    ui->lineEdit_11->setText(QString::number(thermo_ramp_rate[2]));
    ui->lineEdit_14->setText(QString::number(thermo_ramp_rate[3]));
    ui->lineEdit_17->setText(QString::number(thermo_ramp_rate[4]));
    ui->lineEdit_18->setText(QString::number(thermo_ramp_rate[5]));
    ui->lineEdit_10->setText(QString::number(No_ramp_cycles));
    ui->lineEdit_20->setText(thermo_chemical_name);
    ui->lineEdit_22->setText(QString::number(thermo_start_temp));
    ui->lineEdit_21->setText(QString::number(thermo_stop_temp));
}

void MainWindow::on_pushButton_96_clicked()
{
    int length = ui->lineEdit_90->text().toInt();
    QString errMsg;
    QMessageBox msgBox;
    if (length > 4 || length <2)
    {
        errMsg.append("No.of Standards must be in between 2 to 4\n");
        msgBox.critical(0,"Error",errMsg);
        msgBox.setFixedSize(500,200);
    }
    else
    {
        Noofsamples=ui->lineEdit_90->text().toInt();
        Noof_standards_hide();

        if(Noofsamples==1)
        {
            show_standard1();
        }
        else if(Noofsamples==2)
        {
            show_standard1();
            show_standard2();
        }
        else if(Noofsamples==3)
        {
            show_standard1();
            show_standard2();
            show_standard3();
        }
        else if(Noofsamples==4)
        {
            show_standard1();
            show_standard2();
            show_standard3();
            show_standard4();
        }


    }
    ui->label_142->hide(); //vinay

}
void MainWindow::hide_sample_pushbuttons()
{
    ui->pushButton_99->hide();
    ui->pushButton_102->hide();
    ui->pushButton_101->hide();
    ui->pushButton_98->hide();
    ui->pushButton_100->hide();
    ui->pushButton_97->hide();
    ui->lineEdit_90->show();
    ui->label_151->show();
    ui->pushButton_96->show();
}

void MainWindow::on_pushButton_100_clicked()
{
    QString str;
    ui->pushButton_100->setEnabled(false);
   // sample_form->show();
   // sample_form->raise();
    if((factory==true)&&(field==false))
    {
        ui->label_88->setText("NOTE: Calibration interval should be in between 6 to 12 Months");
        ui->label_86->setText("Months");
    }
    else if((factory==false)&&(field==true))
    {
        ui->label_88->setText("NOTE: Calibration interval should be in between 15 to 365 Days");
        ui->label_86->setText("Days");
    }

    if(reset==true)
    {
      ui->column1->setText("Temp Range");
      ui->column2->setText("Slopes");
      ui->column3->setText("Offsets");
       for(int i=0;i<Noofsamples-1;i++)
       {
           mfactory[i]=((cmp_fac[i+1]-cmp_fac[i])/(Mmp_fac[i+1]-Mmp_fac[i]));
           cfactory[i]=(cmp_fac[i+1]-(mfactory[i]*Mmp_fac[i+1]));
           qDebug()<<"mfactory[i]"<<mfactory[i];
           qDebug()<<"mfactory[i]"<<mfactory[i];

          //emit send_sample_profile(Mmp_fac[i+1],mfactory[i],cfactory[i],i,true,single_range[i]);
       }
       if(Noofsamples==2)
       {
          str="< ";
          str.append(res_adding(Mmp_fac[1]));
          ui->range1->setText(str);
          ui->range1->show();
          str=res_adding(mfactory[0]);
          ui->slope1->setText(str);
          ui->slope1->setEnabled(false);
          ui->slope1->show();
          str=res_adding(cfactory[0]);
          ui->offset1->setText(str);
          ui->offset1->setEnabled(false);
          ui->offset1->show();
          ui->range2->hide();
          ui->range3->hide();
          ui->range4->hide();
          ui->slope2->hide();
          ui->slope3->hide();
          ui->slope4->hide();
          ui->offset2->hide();
          ui->offset3->hide();
          ui->offset4->hide();
       }
       if(Noofsamples==3)
      {
        str="< ";
        str.append(res_adding(Mmp_fac[1]));
        ui->range1->setText(str);
        ui->range1->show();
        str="> ";
        str.append(res_adding(Mmp_fac[1]));
        ui->range2->setText(str);
        ui->range2->show();
        str=res_adding(mfactory[0]);
        ui->slope1->setText(str);
        ui->slope1->setEnabled(false);
        ui->slope1->show();
        str=res_adding(mfactory[1]);
        ui->slope2->setText(str);
        ui->slope2->setEnabled(false);
        ui->slope2->show();
        str=res_adding(cfactory[0]);
        ui->offset1->setText(str);
        ui->offset1->setEnabled(false);
        ui->offset1->show();
        str=res_adding(cfactory[1]);
        ui->offset2->setText(str);
        ui->offset2->setEnabled(false);
        ui->offset2->show();
        ui->range3->hide();
        ui->range4->hide();
        ui->slope3->hide();
        ui->slope4->hide();
        ui->offset3->hide();
        ui->offset4->hide();
    }
     if(Noofsamples==4)
     {

     str="< ";
     str.append(res_adding(Mmp_fac[1]));
     ui->range1->setText(str);
     ui->range1->show();
     str=res_adding(Mmp_fac[1]);
     str.append(" to ");
     str.append(res_adding(Mmp_fac[2]));
     ui->range2->setText(str);
     ui->range2->show();
     str="> ";
     str.append(res_adding(Mmp_fac[2]));
     ui->range3->setText(str);
     ui->range3->show();
     str=res_adding(mfactory[0]);
     ui->slope1->setText(str);
     ui->slope1->setEnabled(false);
     ui->slope1->show();
     str=res_adding(mfactory[1]);
     ui->slope2->setText(str);
     ui->slope2->setEnabled(false);
     ui->slope2->show();
     str=res_adding(mfactory[2]);
     ui->slope3->setText(str);
     ui->slope3->setEnabled(false);
     str=res_adding(cfactory[0]);
     ui->offset1->setText(str);
     ui->offset1->setEnabled(false);
     ui->offset1->show();
     str=res_adding(cfactory[1]);
     ui->offset2->setText(str);
     ui->offset2->setEnabled(false);
     str=res_adding(cfactory[2]);
     ui->offset3->setText(str);
     ui->offset3->setEnabled(false);
     ui->offset3->show();
     ui->range4->hide();
     ui->slope4->hide();
     ui->offset4->hide();
     }
    }
    if(field_reset==true)
    {
        ui->column1->setText("Chemical Names");
        ui->column2->setText("Temp Range");
        ui->column3->setText("Offsets");
        for(int i=0;i<Noofsamples;i++)
        {
            cfield[i]=cmp_fac[i]-Mmp_fac[i];
            qDebug()<<"cfield[i]"<<cfield[i];

        }
        if(Noofsamples==2)
        {
        ui->range1->setText(sample_calib_name[0]);
        ui->range1->show();
        ui->range2->setText(sample_calib_name[1]);
        ui->range2->show();
        if(single1==false)
        {
        str=res_adding(sample_calib_rgl[0]);
        str.append(" - ");
        str.append(res_adding(sample_calib_rgh[0]));
        }
        else
        {
            str=res_adding(cmp_fac[0]);
        }
        ui->slope1->setText(str);
        ui->slope1->setEnabled(false);
        ui->slope1->show();
        if(single2==false)
        {
        str=res_adding(sample_calib_rgl[1]);
        str.append(" - ");
        str.append(res_adding(sample_calib_rgh[1]));
        }
        else
        {
            str=res_adding(cmp_fac[1]);
        }
        ui->slope2->setText(str);
        ui->slope2->setEnabled(false);
        ui->slope2->show();
        str=res_adding(cfield[0]);
        ui->offset1->setText(str);
        ui->offset1->setEnabled(false);
        ui->offset1->show();
        str=res_adding(cfield[1]);
        ui->offset2->setText(str);
        ui->offset2->setEnabled(false);
        ui->offset2->show();
        ui->range3->hide();
        ui->range4->hide();
        ui->slope3->hide();
        ui->slope4->hide();
        ui->offset3->hide();
        ui->offset4->hide();
        }
        if(Noofsamples==3)
        {
            ui->range1->setText(sample_calib_name[0]);
            ui->range1->show();
            ui->range2->setText(sample_calib_name[1]);
            ui->range2->show();
            ui->range3->setText(sample_calib_name[2]);
            ui->range3->show();
            if(single1==false)
            {
            str=res_adding(sample_calib_rgl[0]);
            str.append(" - ");
            str.append(res_adding(sample_calib_rgh[0]));
            }
            else
            {
                str=res_adding(cmp_fac[0]);
            }
            ui->slope1->setText(str);
            ui->slope1->setEnabled(false);
            ui->slope1->show();
            if(single2==false)
            {
            str=res_adding(sample_calib_rgl[1]);
            str.append(" - ");
            str.append(res_adding(sample_calib_rgh[1]));
            }
            else
            {
                str=res_adding(cmp_fac[1]);
            }
            ui->slope2->setText(str);
            ui->slope2->setEnabled(false);
            ui->slope2->show();
            if(single3==false)
            {
            str=res_adding(sample_calib_rgl[2]);
            str.append(" - ");
            str.append(res_adding(sample_calib_rgh[2]));
            }
            else
            {
                str=res_adding(cmp_fac[2]);
            }
            ui->slope3->setText(str);
            ui->slope3->setEnabled(false);
            ui->slope3->show();
            str=res_adding(cfield[0]);
            ui->offset1->setText(str);
            ui->offset1->setEnabled(false);
            ui->offset1->show();
            str=res_adding(cfield[1]);
            ui->offset2->setText(str);
            ui->offset2->setEnabled(false);
            ui->offset2->show();
            str=res_adding(cfield[2]);
            ui->offset3->setText(str);
            ui->offset3->setEnabled(false);
            ui->offset3->show();
            ui->range4->hide();
            ui->slope4->hide();
            ui->offset4->hide();
        }
         if(Noofsamples==4)
         {

             ui->range1->setText(sample_calib_name[0]);
             ui->range1->show();
             ui->range2->setText(sample_calib_name[1]);
             ui->range2->show();
             ui->range3->setText(sample_calib_name[2]);
             ui->range3->show();
             ui->range4->setText(sample_calib_name[3]);
             ui->range4->show();
             if(single1==false)
             {
             str=res_adding(sample_calib_rgl[0]);
             str.append(" - ");
             str.append(res_adding(sample_calib_rgh[0]));
             }
             else
             {
                 str=res_adding(cmp_fac[0]);
             }
             ui->slope1->setText(str);
             ui->slope1->setEnabled(false);
             ui->slope1->show();
             if(single2==false)
             {
             str=res_adding(sample_calib_rgl[1]);
             str.append(" - ");
             str.append(res_adding(sample_calib_rgh[1]));
             }
             else
             {
                 str=res_adding(cmp_fac[1]);
             }
             ui->slope2->setText(str);
             ui->slope2->setEnabled(false);
             ui->slope2->show();
             if(single3==false)
             {
             str=res_adding(sample_calib_rgl[2]);
             str.append(" - ");
             str.append(res_adding(sample_calib_rgh[2]));
             }
             else
             {
                 str=res_adding(cmp_fac[2]);
             }
             ui->slope3->setText(str);
             ui->slope3->setEnabled(false);
             ui->slope3->show();
             if(single3==false)
             {
             str=res_adding(sample_calib_rgl[3]);
             str.append(" - ");
             str.append(res_adding(sample_calib_rgh[3]));
             }
             else
             {
                 str=res_adding(cmp_fac[3]);
             }
             ui->slope4->setText(str);
             ui->slope4->setEnabled(false);
             ui->slope4->show();
             str=res_adding(cfield[0]);
             ui->offset1->setText(str);
             ui->offset1->setEnabled(false);
             ui->offset1->show();
             str=res_adding(cfield[1]);
             ui->offset2->setText(str);
             ui->offset2->setEnabled(false);
             ui->offset2->show();
             str=res_adding(cfield[2]);
             ui->offset3->setText(str);
             ui->offset3->setEnabled(false);
             ui->offset3->show();
             str=res_adding(cfield[3]);
             ui->offset4->setText(str);
             ui->offset4->setEnabled(false);
             ui->offset4->show();
}
    }


    calibration_paasword_show();
    ui->tabWidget_8->setTabEnabled(3,true);
    ui->tabWidget_8->setCurrentIndex(3);
    ui->tabWidget_8->setTabEnabled(2,false);

}
QString MainWindow::res_adding(float receive_temp)
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

void MainWindow::on_pushButton_97_clicked()
{
    hide_sample_pushbuttons();
    hide_standard1();
    hide_standard2();
    hide_standard3();
    hide_standard4();
    Noof_standards_show();
    calib_flag=false;
    hiding_calib(true);
    ui->tabWidget_8->setCurrentIndex(1);
    ui->tabWidget_8->setTabEnabled(2,false);


}

void MainWindow::on_pushButton_38_clicked()
{
    if((ui->lineEdit_23->text() == user_name[10] && ui->lineEdit_24->text() == user_password[10])||(ui->lineEdit_23->text() == "pol" && ui->lineEdit_24->text() == "121"))
    {
        ui->lineEdit_23->clear();
        ui->lineEdit_24->clear();
        user_push_buttons_show();
        Authorised_hide();

    }
     else if(ui->lineEdit_23->text()=="\0"||ui->lineEdit_24->text() =="\0")
    {


        QMessageBox::critical(this,"Error","User id & Password cannot be Empty");


    }//remember
    else
    {
        QMessageBox::critical(this,"Error","Wrong User id & Password");

    }

}

void MainWindow::on_pushButton_51_clicked()
{
    ui->lineEdit_23->clear();
    ui->lineEdit_24->clear();
}
void MainWindow::user_push_buttons_show()
{
    ui->pushButton_52->show();
    ui->pushButton_53->show();
    ui->pushButton_55->show();
    ui->pushButton_56->show();
    ui->pushButton_58->show();
    ui->pushButton_59->show();
    ui->pushButton_60->show();
    ui->pushButton_61->show();
    ui->pushButton_57->show();
    ui->pushButton_62->show();
    ui->pushButton_63->show();
    ui->pushButton_66->show();
    //ui->pushButton_30->show();            //vinay

}
void MainWindow::user_push_buttons_hide()
{
    ui->pushButton_52->hide();
    ui->pushButton_53->hide();
    ui->pushButton_55->hide();
    ui->pushButton_56->hide();
    ui->pushButton_58->hide();
    ui->pushButton_59->hide();
    ui->pushButton_60->hide();
    ui->pushButton_61->hide();
    ui->pushButton_57->hide();
    ui->pushButton_62->hide();
    ui->pushButton_63->hide();
    ui->pushButton_66->hide();
}
void MainWindow::Authorised_hide()
{
    ui->pushButton_38->hide();
    ui->pushButton_51->hide();
    ui->lineEdit_23->hide();
    ui->lineEdit_24->hide();
    ui->label_81->hide();
    ui->label_82->hide();
}

void MainWindow::Authorised_show()
{
    ui->pushButton_38->show();
    ui->pushButton_51->show();
    ui->lineEdit_23->show();
    ui->lineEdit_24->show();
    ui->label_81->show();
    ui->label_82->show();
}
void MainWindow::user_hide()
{
    ui->pushButton_64->hide();
    ui->pushButton_65->hide();
    ui->lineEdit_32->hide();
    ui->lineEdit_33->hide();
    ui->label_83->hide();
    ui->label_84->hide();
}

void MainWindow::user_show()
{
    ui->pushButton_64->show();
    ui->pushButton_65->show();
    ui->lineEdit_32->show();
    ui->lineEdit_33->show();
    ui->label_83->show();
    ui->label_84->show();
}


void MainWindow::on_pushButton_66_clicked()
{
    user_push_buttons_hide();
    Authorised_show();
}

void MainWindow::on_pushButton_52_clicked()
{
    users=0;
    user_password_lineEdit_show(user_name[users],user_password[users]);
    user_show();
    user_push_buttons_hide();
}

void MainWindow::on_pushButton_65_clicked()
{
    user_hide();
    user_push_buttons_show();
}

void MainWindow::on_pushButton_53_clicked()
{
    users=1;
    user_password_lineEdit_show(user_name[users],user_password[users]);
    user_show();
    user_push_buttons_hide();
}

void MainWindow::on_pushButton_55_clicked()
{
    users=2;
    user_password_lineEdit_show(user_name[users],user_password[users]);
    user_show();
    user_push_buttons_hide();
}

void MainWindow::on_pushButton_56_clicked()
{
    users=3;
    user_password_lineEdit_show(user_name[users],user_password[users]);
    user_show();
    user_push_buttons_hide();
}

void MainWindow::on_pushButton_58_clicked()
{
    users=4;
    user_password_lineEdit_show(user_name[users],user_password[users]);
    user_show();
    user_push_buttons_hide();
}

void MainWindow::on_pushButton_59_clicked()
{
    users=5;
    user_password_lineEdit_show(user_name[users],user_password[users]);
    user_show();
    user_push_buttons_hide();
}

void MainWindow::on_pushButton_60_clicked()
{
    users=6;
    user_password_lineEdit_show(user_name[users],user_password[users]);
    user_show();
    user_push_buttons_hide();
}

void MainWindow::on_pushButton_61_clicked()
{
    users=7;
    user_password_lineEdit_show(user_name[users],user_password[users]);
    user_show();
    user_push_buttons_hide();
}

void MainWindow::on_pushButton_57_clicked()
{
    users=8;
    user_password_lineEdit_show(user_name[users],user_password[users]);
    user_show();
    user_push_buttons_hide();
}

void MainWindow::on_pushButton_62_clicked()
{
    users=9;
    user_password_lineEdit_show(user_name[users],user_password[users]);
    user_show();
    user_push_buttons_hide();
}

void MainWindow::on_pushButton_63_clicked()
{
    users=10;
    user_password_lineEdit_show(user_name[users],user_password[users]);
    user_show();
    user_push_buttons_hide();
}

void MainWindow::on_pushButton_64_clicked()
{
    updated_username_password(users);
}
void MainWindow::updated_username_password(int receive_user)// avoiding same user id for different users
{
    bool id;
    id=true;
    //qdebug()<<"receive user"<<receive_user;
    QString temp_username,temp_password,status_msg;
    int length;
    temp_username=ui->lineEdit_32->text();
    temp_password=ui->lineEdit_33->text();

    switch( receive_user)
    {

    case 0:
        for(int i=0;i<11;i++)
        {

            if(temp_username==user_name[i])
            {
                status_msg="User ID already exists ";
                goto diag_msg;
                id=false;
            }
            else
            {
                id=true;
            }
        }
        break;
    case 1:
        for(int i=0;i<11;i++)
        {

            if(temp_username==user_name[i])
            {
                status_msg="User ID already exists ";
                goto diag_msg;
                id=false;
            }
            else
            {
                id=true;
            }
        }
        break;
    case 2:
        for(int i=0;i<11;i++)
        {

            if(temp_username==user_name[i])
            {
                status_msg="User ID already exists ";
                goto diag_msg;
                id=false;
            }
            else
            {
                id=true;
            }
        }
        break;
    case 3:
        for(int i=0;i<11;i++)
        {

            if(temp_username==user_name[i])
            {
                status_msg="User ID already exists ";
                goto diag_msg;
                id=false;
            }
            else
            {
                id=true;
            }
        }
        break;
    case 5:
        for(int i=0;i<11;i++)
        {

            if(temp_username==user_name[i])
            {
                status_msg="User ID already exists ";
                goto diag_msg;
                id=false;

            }
        }
        break;

    case 6:
        for(int i=0;i<11;i++)
        {

            if(temp_username==user_name[i])
            {
                status_msg="User ID already exists ";
                goto diag_msg;
                id=false;
            }
            else
            {
                id=true;
            }
        }
        break;
    case 7:
        for(int i=0;i<11;i++)
        {

            if(temp_username==user_name[i])
            {
                status_msg="User ID already exists ";
                goto diag_msg;
                id=false;
            }
            else
            {
                id=true;
            }
        }
        break;
    case 8:
        for(int i=0;i<11;i++)
        {
            if(temp_username==user_name[i])
            {
                status_msg="User ID already exists ";
                goto diag_msg;
                id=false;
            }
            else
            {
                id=true;
            }
        }
        break;
    case 9:
        for(int i=0;i<11;i++)
        {

            if(temp_username==user_name[i])
            {
                status_msg="User ID already exists ";
                goto diag_msg;
                id=false;
            }
            else
            {
                id=true;
            }
        }
        break;
    case 10:
        for(int i=0;i<11;i++)
        {

            if(temp_username==user_name[i])
            {
                status_msg="User ID already exists ";
                goto diag_msg;
                id=false;
            }
            else
            {
                id=true;
            }
        }
        break;
    }
     if((temp_username[0]=='\0')||(temp_password[0]=='\0'))
    {
        status_msg="spaces are not allowed";
        goto diag_msg;
    }//remember


      for(int i=0;((temp_username[i]!='\0')||(temp_password[i]!='\0'));i++)
    {
        if((temp_username[i]==' ')||(temp_password[i]==' '))
        {
            status_msg="spaces are not allowed";
            goto diag_msg;
        }
    }//remember
    length = temp_username.size();
    if(length >= 20)
    {
        status_msg="User Name length should not be more then 20 characters";
        goto diag_msg;
    }

    length =temp_password.size();
    if(length >= 20)
    {
        status_msg="Password length should not be more then 20 characters";
        goto diag_msg;
    }
    if(id==true)
    {
        status_msg="User Name & Password updated";
        user_name[receive_user]=temp_username;
        user_password[receive_user]=temp_password;

diag_msg:
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("User Alert");
        msgBox.setText(status_msg);
        msgBox.setStandardButtons(QMessageBox::Ok );
        int ret = msgBox.exec();
        switch (ret)
        {
        case QMessageBox::Ok:
            if(status_msg=="User Name & Password updated")
            {
                update_password_screen();
                update_Profile();
                user_push_buttons_show();
                user_hide();
                users=0;
            }
            ui->lineEdit_32->clear();
            ui->lineEdit_33->clear();
            break;
        default:
            break;
        }
    }
}
void MainWindow::update_password_screen()
{
    ui->pushButton_52->setText(user_name[0]);
    ui->pushButton_53->setText(user_name[1]);
    ui->pushButton_55->setText(user_name[2]);
    ui->pushButton_56->setText(user_name[3]);
    ui->pushButton_58->setText(user_name[4]);
    ui->pushButton_59->setText(user_name[5]);
    ui->pushButton_60->setText(user_name[6]);
    ui->pushButton_61->setText(user_name[7]);
    ui->pushButton_57->setText(user_name[8]);
    ui->pushButton_62->setText(user_name[9]);
}
void MainWindow::user_password_lineEdit_show(QString user_name1,QString password1)
{
    ui->lineEdit_32->setText(user_name1);
    ui->lineEdit_33->setText(password1);
}
void MainWindow::ramprate_configuration_screen_hide()
{
    //labels
    ui->label_15->hide();
    ui->label_22->hide();
    ui->label_19->hide();
    ui->label_26->hide();
    ui->label_29->hide();
    ui->label_33->hide();
    ui->label_30->hide();
    ui->label_34->hide();
    ui->label_21->hide();
    ui->label_42->hide();
    ui->label_32->hide();
    ui->label_45->hide();
    ui->label_46->hide();
    ui->label_71->hide();
    ui->label_47->hide();
    ui->label_20->hide();
    ui->label_76->hide();

    //lineEdits
    ui->lineEdit_3->hide();
    ui->lineEdit_9->hide();
    ui->lineEdit_11->hide();
    ui->lineEdit_14->hide();
    ui->lineEdit_17->hide();
    ui->lineEdit_18->hide();
    ui->lineEdit_20->hide();
    ui->lineEdit_22->hide();
    ui->lineEdit_21->hide();
    ui->lineEdit_10->hide();
     ui->lineEdit_37->hide();
    //pushbuttons
    ui->pushButton_33->hide();
    ui->pushButton_34->hide();

}
void MainWindow::camera_calib_image_reading()
{
    if(!camera_timer->isActive())
    {
        camera_once_flag = true;
        //qdebug() << "timer is not active";
        emit call_setup_camera();

        //qdebug() << "camera setup";
        camera_timer->start(500);
        //qdebug() << "camera started";
    }
}

void MainWindow::on_pushButton_67_clicked()//x-axis going forward
{

    if((area_x+temp_area_x)<180)
    {
        temp_area_x++;
        emit send_image_area_para((area_x+temp_area_x),(area_y+temp_area_y));
    }
}

void MainWindow::on_pushButton_68_clicked()//x-axis going backward
{
    if((area_x+temp_area_x)>60)
    {
        temp_area_x--;
        emit send_image_area_para((area_x+temp_area_x),(area_y+temp_area_y));
    }
}

void MainWindow::on_pushButton_69_clicked()//y-axis up going
{
    if((area_y+temp_area_y)<120)
    {
        temp_area_y++;
        emit send_image_area_para((area_x+temp_area_x),(area_y+temp_area_y));
    }
}

void MainWindow::on_pushButton_70_clicked()//y-axis down going
{
    if((area_y+temp_area_y)>30)
    {
        temp_area_y--;
        emit send_image_area_para((area_x+temp_area_x),(area_y+temp_area_y));
    }
}
void MainWindow::staus_message()
{
    toogle_status^=1;
    if(toogle_status)
        ui->label_25->show();
    else
        ui->label_25->hide();
}
void MainWindow::hide_main_tab(char first,char second,char third,char fourth,char fifth)
{
    ui->tabWidget->setTabEnabled(1,first);
    ui->tabWidget->setTabEnabled(2,second);
    ui->tabWidget->setTabEnabled(3,third);
    ui->tabWidget->setTabEnabled(4,fourth);
    ui->tabWidget->setTabEnabled(5,fifth);
}
void MainWindow::hide_main_tab3(char first,char second,char third,char fourth,char fifth,char sixth)
{
    ui->tabWidget_3->setTabEnabled(1,first);
    ui->tabWidget_3->setTabEnabled(2,second);
    ui->tabWidget_3->setTabEnabled(3,third);
    ui->tabWidget_3->setTabEnabled(4,fourth);
    ui->tabWidget_3->setTabEnabled(5,fifth);
    ui->tabWidget_3->setTabEnabled(6,sixth);
}

void MainWindow::conformation_delete_data()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("User Alert");
    msgBox.setText("Do you want to cancel?\nThe data will be not stored.\n");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    {
    case QMessageBox::Ok:

        QFile().remove(filename);
        QDir().rmdir(filename11);
        break;
    case QMessageBox::Cancel:

        save_Results();

        break;
    default:
        // should never be reached
        break;
    }
}
void MainWindow::show_standard1()
{
    ui->label_153->show();
    ui->label_142->show();
    ui->pushButton_89->show();
    ui->pushButton_90->show();
    ui->label_141->show();
    ui->lineEdit_76->show();
   // ui->lineEdit_77->show();
   // ui->lineEdit_78->show();
   // ui->label_147->show();
    ui->radioButton->show();
    ui->radioButton_5->show();
    //ui->radioButton_5->setChecked(true);

}
void MainWindow::show_standard2()
{
    ui->label_143->show();
   // ui->lineEdit_79->show();
    ui->lineEdit_80->show();
    //ui->lineEdit_81->show();
    //ui->label_148->show();
    ui->radioButton_2->show();
    ui->radioButton_6->show();
    //ui->radioButton_6->setChecked(true);



}
void MainWindow::show_standard3()
{
    ui->label_144->show();
    //ui->lineEdit_82->show();
    ui->lineEdit_83->show();
    //ui->lineEdit_84->show();
   // ui->label_145->show();
    ui->radioButton_3->show();
    ui->radioButton_7->show();
    //ui->radioButton_7->setChecked(true);



}
void MainWindow::show_standard4()
{
    ui->label_152->show();
   // ui->lineEdit_85->show();
    ui->lineEdit_86->show();
    //ui->lineEdit_87->show();
    //ui->label_146->show();
    ui->radioButton_4->show();
    ui->radioButton_8->show();
    //ui->radioButton_8->setChecked(true);
}
void MainWindow::hide_standard1()
{
    ui->label_142->hide();
    ui->pushButton_89->hide();
    ui->pushButton_90->hide();
    ui->label_141->hide();
    ui->label_153->hide();
    ui->lineEdit_76->hide();
    ui->lineEdit_77->hide();
    ui->lineEdit_78->hide();
    ui->label_147->hide();
    ui->radioButton->hide();
    ui->radioButton_5->hide();


}
void MainWindow::hide_standard2()
{
    ui->label_143->hide();
    ui->lineEdit_79->hide();
    ui->lineEdit_80->hide();
    ui->lineEdit_81->hide();
    ui->label_148->hide();
    ui->radioButton_2->hide();
    ui->radioButton_6->hide();


}
void MainWindow::hide_standard3()
{
    ui->label_144->hide();
    ui->lineEdit_82->hide();
    ui->lineEdit_83->hide();
    ui->lineEdit_84->hide();
    ui->label_145->hide();
    ui->radioButton_3->hide();
    ui->radioButton_7->hide();


}
void MainWindow::hide_standard4()
{
    ui->label_152->hide();
    ui->lineEdit_85->hide();
    ui->lineEdit_86->hide();
    ui->lineEdit_87->hide();
    ui->label_146->hide();
    ui->radioButton_4->hide();
    ui->radioButton_8->hide();


}

void MainWindow::on_pushButton_89_clicked()
{

    ui->radioButton->setChecked(false);
    ui->radioButton_2->setChecked(false);
    ui->radioButton_3->setChecked(false);                //vinay
    ui->radioButton_4->setChecked(false);
    ui->radioButton_5->setChecked(false);
    ui->radioButton_6->setChecked(false);
    ui->radioButton_7->setChecked(false);
    ui->radioButton_8->setChecked(false);
    QMessageBox msgBox;
    QString errMsg;
    bool error_bit = false;
    int length;
    float start_temp,stop_temp;
    length = ui->lineEdit_76->text().size();
    if (length > 20 || length <=0)
    {
        error_bit = true;
        errMsg.append("Standard1 name cannot be empty and must be less than 20 characters in size.\n");
    }
    if(single1==false)
    {
    start_temp = ui->lineEdit_77->text().toFloat();
    if (start_temp > 399.0 || start_temp < 25)
    {
        error_bit = true;
        errMsg.append("Standard1 Start temperature must be with in the range 25 to 399 degC.\n");
    }
    stop_temp = ui->lineEdit_78->text().toFloat();
    if (stop_temp > 400.0 || stop_temp < start_temp)
    {
        error_bit = true;
        errMsg.append("Standard1 Stop temperature must be greater than start temperature and be less than 400degC.\n");
    }
    }
    else
    {
        start_temp = ui->lineEdit_77->text().toFloat();
        if (start_temp > 399.0 || start_temp < 25)
        {
            error_bit = true;
            errMsg.append("Standard1  temperature must be with in the range 25 to 399 degC.\n");
        }
    }
    if(Noofsamples>1)
        ;
    else
        goto error_label;
    length = ui->lineEdit_80->text().size();

    if (length > 20 || length <=0)
    {
        error_bit = true;
        errMsg.append("Standard2 Chemical name cannot be empty and must be less than 20 characters in size.\n");
    }
    if(single2==false)
    {
    start_temp = ui->lineEdit_79->text().toFloat();
    if (start_temp > 399.0 || start_temp < 25)
    {
        error_bit = true;
        errMsg.append("Standard2 Start temperature must be with in the range 25 to 399 degC.\n");
    }

    stop_temp = ui->lineEdit_81->text().toFloat();
    if (stop_temp > 400.0 || stop_temp < start_temp)
    {
        error_bit = true;
        errMsg.append("Standard2 Stop temperature must be greater than start temperature and be less than 400degC.\n");
    }
    }
    else
    {
        start_temp = ui->lineEdit_79->text().toFloat();
        if (start_temp > 399.0 || start_temp < 25)
        {
            error_bit = true;
            errMsg.append("Standard2  temperature must be with in the range 25 to 399 degC.\n");
        }
    }
    if(Noofsamples>2)
        ;
    else
        goto error_label;
    length = ui->lineEdit_83->text().size();

    if (length > 20 || length <=0)
    {
        error_bit = true;
        errMsg.append("Standard3 Chemical name cannot be empty and must be less than 20 characters in size.\n");
    }
    if(single3==false)
    {
    start_temp = ui->lineEdit_82->text().toFloat();
    if (start_temp > 399.0 || start_temp < 25)
    {
        error_bit = true;
        errMsg.append("Standard3 Start temperature must be with in the range 25 to 399 degC.\n");
    }

    stop_temp = ui->lineEdit_84->text().toFloat();
    if (stop_temp > 400.0 || stop_temp < start_temp)
    {
        error_bit = true;
        errMsg.append("Standard3 Stop temperature must be greater than start temperature and be less than 400degC.\n");
    }
    }
    else
    {
        start_temp = ui->lineEdit_82->text().toFloat();
        if (start_temp > 399.0 || start_temp < 25)
        {
            error_bit = true;
            errMsg.append("Standard3  temperature must be with in the range 25 to 399 degC.\n");

    }
    }
    if(Noofsamples>3)
        ;
    else
        goto error_label;
    length = ui->lineEdit_86->text().size();

    if (length > 20 || length <=0)
    {
        error_bit = true;
        errMsg.append("Standard4 Chemical name cannot be empty and must be less than 20 characters in size.\n");
    }
    if(single4==false)
    {
    start_temp = ui->lineEdit_85->text().toFloat();
    if (start_temp > 399.0 || start_temp < 25)
    {
        error_bit = true;
        errMsg.append("Standard4 Start temperature must be with in the range 25 to 399 degC.\n");
    }

    stop_temp = ui->lineEdit_87->text().toFloat();
    if (stop_temp > 400.0 || stop_temp < start_temp)
    {
        error_bit = true;
        errMsg.append("Standard4 Stop temperature must be greater than start temperature and be less than 400degC.\n");
    }
    }
    else
    {
        start_temp = ui->lineEdit_85->text().toFloat();
        if (start_temp > 399.0 || start_temp < 25)
        {
            error_bit = true;
            errMsg.append("Standard4  temperature must be with in the range 25 to 399 degC.\n");
        }
    }
error_label:
    if(error_bit)
    {
        msgBox.critical(0,"Error",errMsg);
        msgBox.setFixedSize(500,200);
    }
    else
    {
       // if(Noofsamples==1)
          //  receive_standard(ui->lineEdit_76->text(),ui->lineEdit_77->text().toFloat(),ui->lineEdit_78->text().toFloat(),0);
        if(Noofsamples==2)
        {
            if(single1==false)
            receive_standard(ui->lineEdit_76->text(),ui->lineEdit_77->text().toFloat(),ui->lineEdit_78->text().toFloat(),0,0);
            else if(single1==true)
            receive_standard(ui->lineEdit_76->text(),ui->lineEdit_77->text().toFloat(),ui->lineEdit_78->text().toFloat(),0,1);
            if(single2==false)
            receive_standard(ui->lineEdit_80->text(),ui->lineEdit_79->text().toFloat(),ui->lineEdit_81->text().toFloat(),1,0);
            else if(single2==true)
            receive_standard(ui->lineEdit_80->text(),ui->lineEdit_79->text().toFloat(),ui->lineEdit_81->text().toFloat(),1,1);

        }
        else if(Noofsamples==3)
        {
            if(single1==false)
            receive_standard(ui->lineEdit_76->text(),ui->lineEdit_77->text().toFloat(),ui->lineEdit_78->text().toFloat(),0,0);
            else if(single1==true)
            receive_standard(ui->lineEdit_76->text(),ui->lineEdit_77->text().toFloat(),ui->lineEdit_78->text().toFloat(),0,1);
            if(single2==false)
            receive_standard(ui->lineEdit_80->text(),ui->lineEdit_79->text().toFloat(),ui->lineEdit_81->text().toFloat(),1,0);
            else if(single2==true)
            receive_standard(ui->lineEdit_80->text(),ui->lineEdit_79->text().toFloat(),ui->lineEdit_81->text().toFloat(),1,1);
            if(single3==false)
            receive_standard(ui->lineEdit_83->text(),ui->lineEdit_82->text().toFloat(),ui->lineEdit_84->text().toFloat(),2,0);
            else if(single3==true)
            receive_standard(ui->lineEdit_83->text(),ui->lineEdit_82->text().toFloat(),ui->lineEdit_84->text().toFloat(),2,1);
        }
        else
        {
            if(single1==false)
            receive_standard(ui->lineEdit_76->text(),ui->lineEdit_77->text().toFloat(),ui->lineEdit_78->text().toFloat(),0,0);
            else if(single1==true)
            receive_standard(ui->lineEdit_76->text(),ui->lineEdit_77->text().toFloat(),ui->lineEdit_78->text().toFloat(),0,1);
            if(single2==false)
            receive_standard(ui->lineEdit_80->text(),ui->lineEdit_79->text().toFloat(),ui->lineEdit_81->text().toFloat(),1,0);
            else if(single2==true)
            receive_standard(ui->lineEdit_80->text(),ui->lineEdit_79->text().toFloat(),ui->lineEdit_81->text().toFloat(),1,1);
            if(single3==false)
            receive_standard(ui->lineEdit_83->text(),ui->lineEdit_82->text().toFloat(),ui->lineEdit_84->text().toFloat(),2,0);
            else if(single3==true)
            receive_standard(ui->lineEdit_83->text(),ui->lineEdit_82->text().toFloat(),ui->lineEdit_84->text().toFloat(),2,1);
            if(single4==false)
            receive_standard(ui->lineEdit_86->text(),ui->lineEdit_85->text().toFloat(),ui->lineEdit_87->text().toFloat(),3,0);
            else if(single4==true)
            receive_standard(ui->lineEdit_86->text(),ui->lineEdit_85->text().toFloat(),ui->lineEdit_87->text().toFloat(),3,1);

        }

         if(Noofsamples==2)
        {


            Show_button_text1();
            Show_button_text2();
        }
        else if(Noofsamples==3)
        {

            Show_button_text1();
            Show_button_text2();
            Show_button_text3();
        }
        else if(Noofsamples==4)
        {

            Show_button_text1();
            Show_button_text2();
            Show_button_text3();
            Show_button_text4();
        }
        hide_standard1();
        hide_standard2();
        hide_standard3();
        hide_standard4();
        ui->pushButton_97->show();
        ui->pushButton_100->show();
        ui->pushButton_100->setEnabled(false);
        ui->pushButton_99->setEnabled(true);
        ui->pushButton_102->setEnabled(true);
        ui->pushButton_101->setEnabled(true);
        ui->pushButton_98->setEnabled(true);
        calib_cnt=0;
        ui->tabWidget_8->setCurrentIndex(2);
        ui->tabWidget_8->setTabEnabled(2,true);
        ui->tabWidget_8->setTabEnabled(1,false);

    }
}

void MainWindow::on_pushButton_90_clicked()
{
    clear_fields();
    hide_standard1();
    hide_standard2();
    hide_standard3();
    hide_standard4();
    hide_sample_pushbuttons();
    Noof_standards_show();
}
void MainWindow::receive_standard(QString standard,float start_temp,float end_temp,int standard_no,int x)
{
    QString range;
    sample_calib_name[standard_no]= standard;
    sample_calib_rgl[standard_no] = start_temp;
    sample_calib_rgh[standard_no] = end_temp;
    range.clear();
    if(x==1)
    {

    cmp_fac[standard_no]=start_temp;
    calib_measure_values[standard_no]=QString::number(start_temp);

    }
    else
    {

        range.append(QString::number(start_temp));
        range.append("-");
        range.append(QString::number(end_temp));
        cmp_fac[standard_no]=(start_temp+end_temp)/2*1.0;
        calib_measure_values[standard_no]=range;
    }
        single_range[standard_no]=x;

    \
}

QString MainWindow::button_text(QString chemical_name,float start,float end, bool x )
{
    QString button_text;
    button_text.clear();
    button_text.append(chemical_name);
    button_text.append("\n");
    if(x)
    button_text.append(QString::number(start));
    else
    {
    button_text.append(QString::number(start));
    button_text.append(" - ");
    button_text.append(QString::number(end));
    }
   //button_text.append(QString::number(receive_mean_temp));
    return button_text;
}
void MainWindow::Show_button_text1()
{
    receive_button_text.clear();
    receive_button_text=button_text(sample_calib_name[0], sample_calib_rgl[0],sample_calib_rgh[0],single_range[0]);
    ui->pushButton_99->setText(receive_button_text);
    ui->pushButton_99->show();

}
void MainWindow::Show_button_text2()
{
    receive_button_text.clear();
    receive_button_text=button_text(sample_calib_name[1], sample_calib_rgl[1],sample_calib_rgh[1],single_range[1]);
    ui->pushButton_102->setText(receive_button_text);
    ui->pushButton_102->show();
}
void MainWindow::Show_button_text3()
{
    receive_button_text.clear();
    receive_button_text=button_text(sample_calib_name[2], sample_calib_rgl[2],sample_calib_rgh[2],single_range[2]);
    ui->pushButton_101->setText(receive_button_text);
    ui->pushButton_101->show();
}
void MainWindow::Show_button_text4()
{
    receive_button_text.clear();
    receive_button_text=button_text(sample_calib_name[3], sample_calib_rgl[3],sample_calib_rgh[3],single_range[3]);
    ui->pushButton_98->setText(receive_button_text);
    ui->pushButton_98->show();
}

void MainWindow::on_pushButton_91_clicked()
{
    //qdebug()<<"id"<<user_name[10];
    //qdebug()<<"psw"<<user_password[10];



   calibration_paasword_hide();

    if((ui->lineEdit_89->text() == user_name[10] && ui->lineEdit_88->text() == user_password[10])||(ui->lineEdit_89->text() == "pol" && ui->lineEdit_88->text() == "121"))
    {
        if((ui->lineEdit_89->text() == user_name[10] && ui->lineEdit_88->text() == user_password[10]))
        {
        ui->factory_calib_button->hide();
        ui->field_calib_button->show();
        ui->reset->show();
        operator_name=user_name[10];
        }
        else if((ui->lineEdit_89->text() == "pol" && ui->lineEdit_88->text() == "121"))
        {
        ui->field_calib_button->show();
        ui->factory_calib_button->show();
        ui->reset->show();
        operator_name="pol";
        } 
    }
    else if(ui->lineEdit_89->text()=="\0"||ui->lineEdit_88->text() =="\0")
        {
            QMessageBox::critical(this,"Error","User id & Password cannot be Empty");

        }//remember
    else
    {
        QMessageBox::critical(this,"Error"," Wrong User Name & Password ");
        calibration_paasword_show();
    }
}

void MainWindow::on_pushButton_95_clicked() //vinay
{
   ui->tabWidget_8->setCurrentIndex(0);
    clear_fields();
    calibration_paasword_show();
    Noof_standards_hide();
    ui->field_calib_button->hide();
    ui->reset->hide();
    ui->factory_calib_button->hide();
    ui->tabWidget_8->setTabEnabled(0,true);
    ui->tabWidget_8->setTabText(1,"Calibration Mode");
    ui->tabWidget_8->setTabEnabled(1,false);
    ui->tabWidget->setEnabled(true);
    ui->tabWidget_3->setEnabled(true);
    ui->tabWidget_4->setEnabled(true);
    ui->tabWidget_5->setEnabled(true);



}
void MainWindow::Noof_standards_show() //vinay
{
    ui->lineEdit_90->show();
    ui->label_151->show();
    ui->pushButton_96->show();
    ui->pushButton_95->show();
}
void MainWindow::Noof_standards_hide() //vinay
{
    ui->lineEdit_90->hide();
    ui->label_151->hide();
    ui->pushButton_96->hide();
    ui->pushButton_95->hide();
}
void MainWindow::calibration_paasword_show() //vinay
{
    ui->lineEdit_89->show();
    ui->lineEdit_88->show();
    ui->label_149->show();
    ui->label_150->show();
    ui->pushButton_91->show();
    ui->pushButton_92->show();
}
void MainWindow::calibration_paasword_hide() //vinay
{
    ui->lineEdit_89->hide();
    ui->lineEdit_88->hide();
    ui->label_149->hide();
    ui->label_150->hide();
    ui->pushButton_91->hide();
    ui->pushButton_92->hide();

}
void MainWindow::clear_fields()
{
   ui->lineEdit_90->clear() ;
   ui->lineEdit_89->clear() ;
   ui->lineEdit_88->clear() ;
   ui->lineEdit_76->clear();
   ui->lineEdit_80->clear();
   ui->lineEdit_83->clear();
   ui->lineEdit_86->clear();
   ui->lineEdit_77->clear();
   ui->lineEdit_78->clear();
   ui->lineEdit_79->clear();
   ui->lineEdit_81->clear();
   ui->lineEdit_82->clear();
   ui->lineEdit_84->clear();
   ui->lineEdit_85->clear();
   ui->lineEdit_87->clear();
   ui->radioButton->setChecked(false);
   ui->radioButton_2->setChecked(false);
   ui->radioButton_3->setChecked(false);
   ui->radioButton_4->setChecked(false);
   ui->radioButton_5->setChecked(false);
   ui->radioButton_6->setChecked(false);
   ui->radioButton_7->setChecked(false);
   ui->radioButton_8->setChecked(false);

}
void MainWindow::calibration_History()
{
    if(field_calib==1)
    {
    QString str,str1;
    ui->lineEdit_62->setText((Noofsamples>=1)?(operator_name):"  NA  ");
    ui->lineEdit_62->setEnabled(false);
    ui->lineEdit_40->setText((Noofsamples>=1)?(calib_date):"  NA  ");
    ui->lineEdit_40->setEnabled(false);
    ui->lineEdit_67->setText((Noofsamples>=1)?(calib_due_date):"  NA  ");
    ui->lineEdit_67->setEnabled(false);
    //sample1
    ui->stnd1->setText((Noofsamples>=1)?(sample_calib_name[0]):"  NA  ");
    ui->stnd1->setEnabled(false);
     ui->calib_range1->setText((Noofsamples>=1)?(calib_measure_values[0]):"NA");
     ui->calib_range1->setEnabled(false);
     str = res_adding(cfield[0]);
     ui->calib_offset1->setText((Noofsamples>=1)?str:"NA");
     ui->calib_offset1->setEnabled(false);

    //sample2
    ui->stnd2->setText((Noofsamples>=2)?(sample_calib_name[1]):"  NA  ");
    ui->stnd2->setEnabled(false);
    ui->calib_range2->setText((Noofsamples>=1)?(calib_measure_values[1]):"NA");
    ui->calib_range2->setEnabled(false);
    str = res_adding(cfield[1]);
    ui->calib_offset2->setText((Noofsamples>=2)?str:"NA");
    ui->calib_offset2->setEnabled(false);

    //sample3
    ui->stnd3->setText((Noofsamples>=3)?(sample_calib_name[2]):"  NA  ");
    ui->stnd3->setEnabled(false); 
    ui->calib_range3->setText((Noofsamples>=3)?(calib_measure_values[2]):"NA");
    ui->calib_range3->setEnabled(false);
    str = res_adding(cfield[2]);
    ui->calib_offset3->setText((Noofsamples>=3)?str:"NA");
    ui->calib_offset3->setEnabled(false);

    //sample4
    ui->stnd4->setText((Noofsamples>=4)?(sample_calib_name[3]):"  NA  ");
    ui->stnd4->setEnabled(false);
    ui->calib_range4->setText((Noofsamples>=4)?(calib_measure_values[3]):"NA");
    ui->calib_range4->setEnabled(false);
    str = res_adding(cfield[3]);
    ui->calib_offset4->setText((Noofsamples>=4)?str:"NA");
    ui->calib_offset4->setEnabled(false);
    }
    else
    {
        ui->lineEdit_62->setEnabled(false);
        ui->lineEdit_40->setEnabled(false);
        ui->lineEdit_67->setEnabled(false);
        ui->stnd1->setEnabled(false);
        ui->stnd2->setEnabled(false);
        ui->stnd3->setEnabled(false);
        ui->stnd4->setEnabled(false);
        ui->calib_range1->setEnabled(false);
        ui->calib_range2->setEnabled(false);
        ui->calib_range3->setEnabled(false);
        ui->calib_range4->setEnabled(false);
        ui->calib_offset1->setEnabled(false);
        ui->calib_offset2->setEnabled(false);
        ui->calib_offset3->setEnabled(false);
        ui->calib_offset4->setEnabled(false);
    }


}

void MainWindow::on_pushButton_74_clicked()
{
   //if(avg_var != 0)
    {
        //alert message

        QMessageBox msgBox(this);
        msgBox.setWindowTitle("User Alert");
        if(sample_profile_sel==0)
        {
            msgBox.setText("Standard1 calibration completed");

        }
        else if(sample_profile_sel==1)
        {
            msgBox.setText("Standard2 calibration completed");

        }
        else if(sample_profile_sel==2)
        {
            msgBox.setText("Standard3 calibration completed");

        }
        else if(sample_profile_sel==3)
        {
            msgBox.setText("Standard4 calibration completed");

        }
        else
        {
            ;
        }
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        switch (ret)
        {
        case QMessageBox::Ok:
            emit pid_strt(false);
            emit update_status(3);
            emit send_setpoint(40,start_temp,end_temp,ramp_rate,p,Ti,sbt,time_cycle,false);
           if(single_range[sample_profile_sel]==0)
           {
            sample_avg_ini_calib[sample_profile_sel] = avg_ini_melt_pnt/avg_var*1.0;
            sample_avg_final_calib[sample_profile_sel] = avg_final_melt_pnt/avg_var*1.0;
            Mmp_fac[sample_profile_sel]=mean_avg/(avg_den*1.0);
            }
            else
            {
            Mmp_fac[sample_profile_sel]=avg_final_melt_pnt/(avg_var*1.0);
            }
            //qdebug() << "average value:  " << sample_avg_ini_calib[sample_profile_sel];
            //qdebug() << "average value:  " << sample_avg_final_calib[sample_profile_sel];
            smallest_init[sample_profile_sel]=smallest;
            largest_final[sample_profile_sel]=largest;

            if(++calib_cnt==Noofsamples)
            {
                calib_cnt=0;
                ui->pushButton_100->setEnabled(true);
            }
            if(sample_profile_sel==0)
            {
                ui->pushButton_99->setEnabled(false);
            }
            else if(sample_profile_sel==1)
            {
                ui->pushButton_102->setEnabled(false);
            }
            else if(sample_profile_sel==2)
            {
                ui->pushButton_101->setEnabled(false);
            }
            else if(sample_profile_sel==3)
            {
                ui->pushButton_98->setEnabled(false);
            }
            /*ui->tabWidget->setTabEnabled(1,true);
                ui->tabWidget->setTabEnabled(2,true);
                ui->tabWidget->setTabEnabled(3,true);
                ui->tabWidget->setTabEnabled(4,true);
                ui->tabWidget->setTabEnabled(5,true);*/
            ui->tabWidget->setCurrentIndex(4);
            calib_flag=false;
            ui->pushButton_75->hide();
            ui->pushButton_74->hide();
            break;
        case QMessageBox::Cancel:
            //goto end_cal;
            break;
        default:
            // should never be reached
            break;
        }
    }


}
void MainWindow::on_pushButton_75_clicked()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("User Alert");
    if(sample_profile_sel==0)
        msgBox.setText("Do you want Discard the Standard1 calibration?.\n");
    else if(sample_profile_sel==1)
        msgBox.setText("Do you want Discard the Standard2 calibration?.\n");
    else if(sample_profile_sel==2)
        msgBox.setText("Do you want Discard the Standard3 calibration?.\n");
    else if(sample_profile_sel==3)
        msgBox.setText("Do you want Discard the Standard4 calibration?.\n");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    {
    case QMessageBox::Ok:
        /*ui->tabWidget->setTabEnabled(1,true);
                ui->tabWidget->setTabEnabled(2,true);
                ui->tabWidget->setTabEnabled(3,true);
                ui->tabWidget->setTabEnabled(4,true);
                ui->tabWidget->setTabEnabled(5,true);*/
        ui->tabWidget->setCurrentIndex(4);
        ui->pushButton_75->hide();
        ui->pushButton_74->hide();
        calib_flag=false;
        break;
    case QMessageBox::Cancel:
        break;
    default:
        // should never be reached
        break;
    }

}
void MainWindow::calculate_offset_mulfactor()
{

    if( (calib_cmplt==1)&&(reset==false))
    {
        if((factory_calib==1)&&(reset==false))
    {
       // qDebug()<<"inside factory calibration";

        if(Noofsamples==2)
        {
        if(block_temp<Mmp_fac[1])
        emit send_sample_calib_factors(mfactory[0],cfactory[0],false) ;
        }
        if(Noofsamples==3)
        {
        if(block_temp<Mmp_fac[1])
        emit send_sample_calib_factors(mfactory[0],cfactory[0],false) ;
        else if((block_temp>=Mmp_fac[1])&&(block_temp<Mmp_fac[2]))
        emit send_sample_calib_factors(mfactory[1],cfactory[1],false) ;
        }
        if(Noofsamples==4)
        {
            if(block_temp<Mmp_fac[1])
            emit send_sample_calib_factors(mfactory[0],cfactory[0],false) ;
            else if((block_temp>=Mmp_fac[1])&&(block_temp<Mmp_fac[2]))
            emit send_sample_calib_factors(mfactory[1],cfactory[1],false) ;
            else
            emit send_sample_calib_factors(mfactory[2],cfactory[2],false) ;

        }

    }
     if(field_calib==1)
    {
         //qDebug()<<"inside field calibration";
        if(Noofsamples==2)
        {
        if(block_temp<Mmp_fac[1])
            emit send_field_calib_factors(cfield_past[0],true) ;
          else if((block_temp>=Mmp_fac[1])&&(block_temp<Mmp_fac[2]))
          emit send_field_calib_factors(cfield_past[1],true) ;
        }
        if(Noofsamples==3)
        {
          if(block_temp<Mmp_fac[1])
          emit send_field_calib_factors(cfield_past[0],true) ;
          else if((block_temp>=Mmp_fac[1])&&(block_temp<Mmp_fac[2]))
          emit send_field_calib_factors(cfield_past[1],true) ;
          else if((block_temp>=Mmp_fac[2])&&(block_temp<Mmp_fac[3]))
          emit send_field_calib_factors(cfield_past[2],true) ;
        }
        if(Noofsamples==4)
        {
            if(block_temp<Mmp_fac[1])
            emit send_field_calib_factors(cfield_past[0],true) ;
            else if((block_temp>=Mmp_fac[1])&&(block_temp<Mmp_fac[2]))
            emit send_field_calib_factors(cfield_past[1],true) ;
            else if((block_temp>=Mmp_fac[2])&&(block_temp<Mmp_fac[3]))
            emit send_field_calib_factors(cfield_past[2],true) ;
            else
            emit send_field_calib_factors(cfield_past[3],true) ;
        }

    }
  }

      if((reset==true)||(calib_cmplt==0))
    {
        //qDebug()<<"inside factory reset";
        emit send_sample_calib_factors(1.0,0.0,false);
        emit send_field_calib_factors(0.0,true) ;
    }

}

 void MainWindow::hiding_calib(bool status_flag)
 {
    ui->tabWidget->setTabEnabled(0,status_flag);
    ui->tabWidget->setTabEnabled(1,status_flag);
    ui->tabWidget->setTabEnabled(3,status_flag);

    ui->tabWidget_3->setTabEnabled(0,status_flag);
    ui->tabWidget_3->setTabEnabled(2,status_flag);
    ui->tabWidget_3->setTabEnabled(3,status_flag);
    ui->tabWidget_3->setTabEnabled(4,status_flag);
    ui->tabWidget_3->setTabEnabled(5,status_flag);
    if(factory_signin)
    {
        ui->tabWidget_4->setTabEnabled(0,status_flag);
        ui->tabWidget_4->setTabEnabled(2,status_flag);
        ui->tabWidget_5->setTabEnabled(0,status_flag);
        ui->tabWidget_5->setTabEnabled(2,status_flag);
    }
    else
    {
        ui->tabWidget_4->setTabEnabled(1,status_flag);
        ui->tabWidget_5->setTabEnabled(1,status_flag);

    }


    if(!status_flag)
    {
        if(factory_signin)
        {
        ui->tabWidget->setTabEnabled(4,true);
        ui->tabWidget_3->setTabEnabled(1,true);
        ui->tabWidget_4->setTabEnabled(1,true);
        ui->tabWidget_5->setTabEnabled(1,true);
        ui->tabWidget_8->setTabEnabled(2,true);
        }
        else
        {
            ui->tabWidget_4->setTabEnabled(1,true);
            ui->tabWidget_5->setTabEnabled(1,true);
        }
    }
    else
    {
        if(factory_signin)
        {
        ui->tabWidget->setTabEnabled(4,status_flag);
        ui->tabWidget_3->setTabEnabled(1,status_flag);
        ui->tabWidget_4->setTabEnabled(1,status_flag);
        ui->tabWidget_5->setTabEnabled(1,status_flag);
        ui->tabWidget_8->setTabEnabled(2,status_flag);
        }
        else
        {
            ui->tabWidget_4->setTabEnabled(1,status_flag);
            ui->tabWidget_5->setTabEnabled(1,status_flag);
        }
    }
}

void MainWindow::on_pushButton_76_clicked()
{

    Company_name=ui->lineEdit_56->text();
    company_add1=ui->lineEdit_58->text();
    company_add2=ui->lineEdit_60->text();
    company_add3=ui->lineEdit_66->text();
    left_tag=ui->lineEdit_68->text();
    right_tag=ui->lineEdit_69->text();
    model_no=temp_model_no;
    update_Profile();
    QMessageBox::information(this,"User Alert","Report Tags Updated");
    //}
}
void MainWindow::report_tags_show()
{
    ui->label_120->show();
    ui->lineEdit_56->show();
    ui->label_122->show();
    ui->lineEdit_58->show();
    ui->label_123->show();
    ui->lineEdit_60->show();
    ui->label_124->show();
    ui->lineEdit_66->show();
    ui->label_125->show();
    ui->lineEdit_68->show();
    ui->label_126->show();
    ui->lineEdit_69->show();
    ui->pushButton_76->show();
  //  ui->pushButton_77->show();
    ui->lineEdit_day->setEnabled(true);
    ui->lineEdit_month->setEnabled(true);
    ui->lineEdit_year->setEnabled(true);
    ui->lineEdit_hour->setEnabled(true);
    ui->lineEdit_minutes->setEnabled(true);
    ui->lineEdit_seconds->setEnabled(true);
    ui->lineEdit_36->setEnabled(true);
    //ui->lineEdit_37->setEnabled(true);
    ui->delete_button->show();          //vinay
    ui->Model_no->show();
    ui->Model_no->setChecked(model_no);
    /*************14547**************/

    ui->lineEdit_25->setEnabled(true);
    ui->lineEdit_26->setEnabled(true);
    ui->lineEdit_27->setEnabled(true);
    ui->lineEdit_28->setEnabled(true);
    ui->lineEdit_29->setEnabled(true);
    ui->lineEdit_30->setEnabled(true);
    ui->lineEdit_31->setEnabled(true);
    ui->manual->setEnabled(true);
    ui->automatic->setEnabled(true);
    /***********14547************/

}
void MainWindow::report_tags_hide()
{

    ui->lineEdit_56->setEnabled(false);//14547 disabled to nrml usrs
    ui->lineEdit_58->setEnabled(false);
    ui->lineEdit_60->setEnabled(false);
    ui->lineEdit_66->setEnabled(false);
    ui->lineEdit_68->setEnabled(false);
    ui->lineEdit_69->setEnabled(false);
    ui->label_120->show();
    ui->label_122->show();
    ui->label_123->show();
    ui->label_124->show();
    ui->label_125->show();
    ui->label_126->show();
    ui->pushButton_76->hide();
  //  ui->pushButton_77->hide();
    ui->lineEdit_day->setEnabled(false);
    ui->lineEdit_month->setEnabled(false);
    ui->lineEdit_year->setEnabled(false);
    ui->lineEdit_hour->setEnabled(false);
    ui->lineEdit_minutes->setEnabled(false);
    ui->lineEdit_seconds->setEnabled(false);
    ui->label_3->hide();
    ui->label_18->hide();
    ui->delete_button->hide();
    ui->Model_no->hide();
    ui->lineEdit_25->setEnabled(false);
    ui->lineEdit_26->setEnabled(false);
    ui->lineEdit_27->setEnabled(false);
    ui->lineEdit_28->setEnabled(false);
    ui->lineEdit_29->setEnabled(false);
    ui->lineEdit_30->setEnabled(false);
    ui->lineEdit_31->setEnabled(false);
    ui->manual->setEnabled(false);
    ui->automatic->setEnabled(false);
}

void MainWindow::on_Model_no_clicked()
{
    temp_model_no ^=1;
}
void MainWindow::cool_mode()
{
    end =true;
    emit send_pid_signal(false);
    emit send_pwm_signal(false);
    emit fan_setpoint(40);
    emit update_status(2);
    ui->pushButton_18->setEnabled(true);


}
void MainWindow::log_off()
{
#ifdef Q_WS_QWS
    QWSServer::setCursorVisible( false );// disabling cursor during logoff
#endif
    QMessageBox::information(this,"User Alert","System is Logging off");
    system("/sbin/reboot");

}

void MainWindow::buzzer_on() // turn on buzzer
{

    QString command;
    qDebug()<<"in loop_buzzer_on";
    command = "echo \"high\" > /sys/class/gpio/gpio33/direction"; //sets gpio as output

    system(qPrintable(command));

    command = "echo \"1\" > /sys/class/gpio/gpio33/value";  // buzzer on

    system(qPrintable(command));

}

void MainWindow::buzzer_off()  //turn off buzzer
{
    QString command;
    qDebug()<<"in loop_buzzer_off";
    command = "echo \"high\" > /sys/class/gpio/gpio33/direction";   //sets gpio as output

    system(qPrintable(command));

    command = "echo \"0\" > /sys/class/gpio/gpio33/value";  // buzzer off

    system(qPrintable(command));

}



void MainWindow::on_pushButton_92_clicked()  //vinay- sample calibration- cancel button
{
    ui->lineEdit_89->clear();
    ui->lineEdit_88->clear();
    calibration_paasword_show();
    factory_pdf=false;
    field_pdf=false;
    reset=false;
    field_reset=false;

}

void MainWindow::on_pushButton_78_clicked()         //refresh button
{
     model->refresh(model->index(storedevice));
     qDebug() << "refresh" ;
}

void MainWindow::on_pushButton_81_clicked()   //sync with usb button----vinay
{
    QString backup_date;
    backup_date.clear();
    backup_date.append("MP_98_backup_");
    backup_date.append(QDate::currentDate().toString("dd/MM/yyyy"));
    backup_date.replace("/","-");
    ui->pushButton_81->setEnabled(false);
    QString command = "cp -rL /results/* /udisk/"+backup_date+"/";
    QString command1 = "mkdir /udisk/"+backup_date;
    QDir disk(usbdrive);
    //if(!disk.exists(usbdrive))
    QDir disk1("/dev/udisk");
    if(!disk1.exists("/dev/udisk"))
    {
        QMessageBox::information(this,"User Alert","USB not Inserted/Detected");
    }
    //else if(disk.exists(usbdrive))
    else if(disk1.exists("/dev/udisk"))
    {

        if(disk.exists("/udisk/results"))
         {
         system(qPrintable(command1));
         qDebug() << "Synchronization with USB,Started"<<QTime::currentTime();
         QMessageBox::information(this,"User Alert","Click OK to Synchronize with USB ");
         system(qPrintable(command));
         QMessageBox::information(this,"User Alert","Synchronization with USB Completed sucessfully.");
         qDebug() << "Synchronization with USB,Completed"<<QTime::currentTime();
         }
         else
         {
         system(qPrintable(command1));
         qDebug() << "Synchronization with USB,Started"<<QTime::currentTime();
         QMessageBox::information(this,"User Alert","Click OK to Synchronize with USB ");
         system(qPrintable(command));
         QMessageBox::information(this,"User Alert","Synchronization with USB Completed sucessfully.");
         qDebug() << "Synchronization with USB,Completed"<<QTime::currentTime();
         }
    }
    ui->pushButton_81->setEnabled(true);

}

void MainWindow::on_pushButton_82_clicked()    //copy button----vinay
{
    ui->pushButton_82->setEnabled(false);

    sPath = model->filePath(ui->treeView->currentIndex());
    QString command = "cp -rL " + sPath + " /udisk/results";
    QString command1 = "mkdir /udisk/results";
    QDir disk(usbdrive);
    //if(!disk.exists(usbdrive))
    QDir disk1("/dev/udisk");
    if(!disk1.exists("/dev/udisk"))
     {
        QMessageBox::information(this,"User Alert","USB not Inserted/Detected");
     }
    //else if(disk.exists(usbdrive))
    else if(disk1.exists("/dev/udisk"))
    {
             if(disk.exists("/udisk/results"))
               {
               qDebug() << "copying particular folder to udisk/results"<<QTime::currentTime();


               QMessageBox msgBox(this);
               msgBox.setWindowTitle("User Alert");
               msgBox.setText("Do you want to Copy the File" + sPath +" ?");
               msgBox.setStandardButtons(QMessageBox::Ok  | QMessageBox::Cancel);

               msgBox.setButtonText( QMessageBox::Ok, "Copy" );
               int ret = msgBox.exec();
               switch (ret)
               {
               case QMessageBox::Ok:
                   system(qPrintable(command));
                   QMessageBox::information(this,"User Alert","Copying Completed sucessfully.");

                   break;
               case QMessageBox::Cancel:
                   break;
               default:
                   break;
               }

               //QMessageBox::information(this,"User Alert","Please Wait.. copying in progress");

               qDebug() << "coped particular folder to udisk/results"<<QTime::currentTime();
               }
             else
               {
               system(qPrintable(command1));
               qDebug() << "results folder created,copying particular folder to udisk/results"<<QTime::currentTime();

               QMessageBox msgBox(this);
               msgBox.setWindowTitle("User Alert");
               msgBox.setText("Do you want to Copy the File" +sPath+" ?");
               msgBox.setStandardButtons(QMessageBox::Ok  | QMessageBox::Cancel);

               msgBox.setButtonText( QMessageBox::Ok, "Copy" );
               int ret = msgBox.exec();
               switch (ret)
               {
               case QMessageBox::Ok:
                   system(qPrintable(command));
                   QMessageBox::information(this,"User Alert","Copying Completed sucessfully.");

                   break;
               case QMessageBox::Cancel:
                   break;
               default:
                   break;
               }
               qDebug() << "coped particular folder to udisk/results"<<QTime::currentTime();
               }
    }
    ui->pushButton_82->setEnabled(true);

}
void MainWindow::read_rtc()        //-----vinay
{

int fd_rtc = open("/dev/rtc", O_RDONLY,0);

if (fd_rtc < 0)
{
    printf("can't open device");

}
else
{

            int ret = ioctl(fd_rtc, RTC_RD_TIME, &rtc_tm); //vinay
             if (ret == -1)
             {
                 printf("rtc ioctl RTC_RD_TIME error\r\n");
             }


             day = rtc_tm.tm_mday;
             month = rtc_tm.tm_mon + 1;
             year = rtc_tm.tm_year + 1900;
             hour = rtc_tm.tm_hour;
             minute = rtc_tm.tm_min;
             sec = rtc_tm.tm_sec;
              ::close(fd_rtc);
}
}

void MainWindow::on_pushButton_83_clicked()
{
    toggle_graph1 ^= 1;
    if(graph_enable_flag1==true)
    {
    if(toggle_graph1)
    {
        customPlot->graph(0)->setVisible(false);
        customPlot->replot();
        ui->pushButton_83->setStyleSheet("QPushButton {color: red;}");
    }
    else
    {
        customPlot->graph(0)->setVisible(true);
        customPlot->replot();
        ui->pushButton_83->setStyleSheet("QPushButton {color: black;}");

    }
    }
}

void MainWindow::on_pushButton_84_clicked()
{
    toggle_graph2 ^= 1;
    if(graph_enable_flag2==true)
    {
    if(toggle_graph2)
    {
        ui->pushButton_84->setStyleSheet("QPushButton {color: green;}");

    customPlot->graph(1)->setVisible(false);
    customPlot->replot();
    }
    else
    {
        ui->pushButton_84->setStyleSheet("QPushButton {color: black;}");
      customPlot->graph(1)->setVisible(true);
      customPlot->replot();
    }
    }

}

void MainWindow::on_pushButton_85_clicked()
{
    toggle_graph3 ^= 1;
    if(graph_enable_flag3==true)
    {
    if(toggle_graph3)
    {
        ui->pushButton_85->setStyleSheet("QPushButton {color: blue;}");

    customPlot->graph(2)->setVisible(false);
    customPlot->replot();
    }
    else
    {
        ui->pushButton_85->setStyleSheet("QPushButton {color: black;}");
     customPlot->graph(2)->setVisible(true);
     customPlot->replot();
    }
    }

}


void MainWindow::on_factory_calib_button_clicked()
{
   reset=true;
   ui->tabWidget_8->setCurrentIndex(1);
   emit send_sample_calib_factors(1.0,0.0,false) ;
   Noof_standards_show();
   ui->tabWidget_8->setTabEnabled(0,false);
   factory_calib=1;
   factory_pdf=true;
   factory=true;
   field=false;
   ui->tabWidget_8->setTabText(1,"Factory Calibration");
}

void MainWindow::on_radioButton_clicked()
{
    single1=true;
    ui->label_147->hide();
    ui->lineEdit_78->hide();
    ui->lineEdit_78->clear();
    ui->lineEdit_77->show();
    ui->label_142->show();

}

void MainWindow::on_radioButton_5_clicked()
{
    single1=false;
    ui->label_147->show();
    ui->lineEdit_78->show();
    ui->lineEdit_77->show();
    ui->label_142->show();

}

void MainWindow::on_radioButton_2_clicked()
{
    single2=true;
    ui->label_148->hide();
    ui->lineEdit_81->hide();
    ui->lineEdit_81->clear();
    ui->lineEdit_79->show();
    ui->label_142->show();


}

void MainWindow::on_radioButton_6_clicked()
{
    single2=false;
    ui->label_148->show();
    ui->lineEdit_81->show();
    ui->lineEdit_79->show();
    ui->label_142->show();

}

void MainWindow::on_radioButton_3_clicked()
{
    single3=true;
    ui->label_145->hide();
    ui->lineEdit_84->hide();
    ui->lineEdit_84->clear();
    ui->lineEdit_82->show();
    ui->label_142->show();

}

void MainWindow::on_radioButton_7_clicked()
{
    single3=false;
    ui->lineEdit_82->show();
    ui->label_145->show();
    ui->lineEdit_84->show();
    ui->label_142->show();

}

void MainWindow::on_radioButton_4_clicked()
{
    single4=true;
    ui->label_146->hide();
    ui->lineEdit_87->hide();
    ui->lineEdit_87->clear();
    ui->lineEdit_85->show();
    ui->label_142->show();

}

void MainWindow::on_radioButton_8_clicked()
{
    single4=false;
    ui->label_146->show();
    ui->lineEdit_87->show();
    ui->lineEdit_85->show();
    ui->label_142->show();

}

void MainWindow::on_field_calib_button_clicked()
{

    factory=false;
    field=true;
    ui->tabWidget_8->setCurrentIndex(1);
    ui->tabWidget_8->setTabEnabled(1,true);
    field_reset=true;
    field_calib=1;
    ui->tabWidget_8->setTabEnabled(0,false);
    Noof_standards_show();
    hide_standard1();
    hide_standard2();
    hide_standard3();
    hide_standard4();
    field_pdf=true;
    ui->tabWidget_8->setTabText(1,"Field Calibration");
}

void MainWindow::on_pushButton_3_clicked()
{

    ui->tabWidget->setEnabled(true);
    ui->tabWidget_3->setEnabled(true);
    ui->tabWidget_4->setEnabled(true);
    ui->tabWidget_5->setEnabled(true);
    ui->tabWidget_8->setTabEnabled(2,false);
    QString errMsg;
    QMessageBox msgBox;
     int length = ui->calib_interval->text().toInt();

     if((factory==true)&&(field==false))
     {
         if(ui->calib_interval->text()=="\0")
          {
              QMessageBox::information(this,"user alert", "Please enter number of Months to Calibrate ");
            ui->tabWidget_8->setCurrentIndex(3);
          }
         else if (length > 12 || length < 6)
         {
             errMsg.append("Calibration time interval in between 6 to 12 Months");
             msgBox.critical(0,"Error",errMsg);
             msgBox.setFixedSize(500,200);
             ui->tabWidget_8->setCurrentIndex(3);
         }
         else
         {
             update_Profile();
             QMessageBox::information(this,"User Alert","Sample Calibration Completed Sucessfully.");
             emit send_from_sample_form(true,length);
         }
     }
      else if((factory==false)&&(field==true))
     {
   if(ui->calib_interval->text()=="\0")
    {
        QMessageBox::information(this,"user alert", "Please enter number of days to Calibrate ");
      ui->tabWidget_8->setCurrentIndex(3);
    }
   else if (length > 365 || length < 15)
   {
       errMsg.append("Calibration time interval in between 15 to 365 days");
       msgBox.critical(0,"Error",errMsg);
       msgBox.setFixedSize(500,200);
       ui->tabWidget_8->setCurrentIndex(3);
   }
   else
   {
       update_Profile();
       QMessageBox::information(this,"User Alert","Sample Calibration Completed Sucessfully.");
       emit send_from_sample_form(true,length);
   }
     }

}

void MainWindow::on_pushButton_25_clicked()
{
    int length = ui->calib_interval->text().toInt();
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

void MainWindow::on_tabWidget_4_currentChanged(int index)
{
    if(index==1)
    {
        if(stop_check_calib==false)
        {
         ui->tabWidget_5->setCurrentIndex(0);
        }
    }

}

void MainWindow::on_tabWidget_5_currentChanged(int index)
{
    if(index==1)
    {
        if(stop_check_calib==false)
        {
         ui->tabWidget_8->setCurrentIndex(0);
        }
    }
}


/*void MainWindow::on_tabWidget_8_currentChanged(int index)
{
    if(index==0)
    {
        ui->tabWidget_8->setCurrentIndex(0);
        ui->field_calib_button->hide();
        ui->factory_calib_button->hide();
        ui->lineEdit_89->show();
        ui->lineEdit_88->show();
        ui->pushButton_91->show();
        ui->pushButton_92->show();
        ui->label_150->show();
        ui->label_149->show();
        clear_fields();


    }
    if(index==1)
    {
        ui->tabWidget_8->setCurrentIndex(0);
        ui->field_calib_button->hide();
        ui->factory_calib_button->hide();
        ui->factory_calib_button->hide();
        ui->lineEdit_89->show();
        ui->lineEdit_88->show();
        ui->pushButton_91->show();
        ui->pushButton_92->show();
        ui->label_150->show();
        ui->label_149->show();
        clear_fields();


    }
}



//ui->tabWidget->setTabEnabled(0,false);

void MainWindow::on_pushButton_30_clicked()
{
    QString command = "cp -rL /udisk/backup_data/init.txt /profiles/ ";
         system(qPrintable(command));
         QMessageBox::information(this,"User Alert","Restored successfully");
}*/



void MainWindow::on_reset_clicked()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("User Alert");
    msgBox.setText("Are You Sure You Want to Delete Calibration Data ");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

    msgBox.setButtonText( QMessageBox::Ok, "Yes" );
    msgBox.setButtonText( QMessageBox::Cancel, "No" );
    int ret = msgBox.exec();
    switch (ret)
    {
    case QMessageBox::Ok:

        send_field_calib_factors(0,false);
        field_calib=0;
        for(int i=0;i<4;i++)
        {
            cfield[i]=0;
            cfield_past[i]=0;

            ui->lineEdit_62->clear();
            ui->lineEdit_40->clear();
            ui->lineEdit_67->clear();
            ui->stnd1->clear();
            ui->stnd2->clear();
            ui->stnd3->clear();
            ui->stnd4->clear();
            ui->calib_range1->clear();
            ui->calib_range2->clear();
            ui->calib_range3->clear();
            ui->calib_range4->clear();
            ui->calib_offset1->clear();
            ui->calib_offset2->clear();
            ui->calib_offset3->clear();
            ui->calib_offset4->clear();
        }
        update_Profile();
        break;

    case QMessageBox::Cancel:

        break;
    default:

        break;
    }

}

void MainWindow::on_Initial_melt1_clicked()
{
    buzzer_on();
    beep_timer->singleShot(1, this, SLOT(buzzer_off()));
    man1^=1;
    if(man1==1)
    {
    melt1=true;
    ui->Initial_melt1->setEnabled(false);
    manual_strt1=block_temp;
    ui->Initial_melt1->setStyleSheet("QPushButton {color: red;}");
    ui->Initial_melt1->setText("End Melt1");
    delay_timer->singleShot(2000,this,SLOT(delay_button()));
    }
    else if(man1==0)
    {
     melt1=false;
     manual_end1=block_temp;
     ui->Initial_melt1->setStyleSheet("QPushButton {color: grey;}");
     ui->Initial_melt1->setEnabled(false);

    }

}

void MainWindow::on_Initial_melt2_clicked()
{
   buzzer_on();
   beep_timer->singleShot(1, this, SLOT(buzzer_off()));
    man2^=1;
    if(man2==1)
    {
    melt2=true;
    ui->Initial_melt2->setEnabled(false);
    manual_strt2=block_temp;
    ui->Initial_melt2->setStyleSheet("QPushButton {color: green;}");
    ui->Initial_melt2->setText("End Melt2");
    ui->Initial_melt2->show();
    delay_timer->singleShot(2000,this,SLOT(delay_button()));

    }
    else if(man2==0)
    {
    melt2=false;
    manual_end2=block_temp;
    ui->Initial_melt2->setStyleSheet("QPushButton {color: grey;}");
    ui->Initial_melt2->setEnabled(false);
    }
}

void MainWindow::on_Initial_melt3_clicked()
{
    buzzer_on();
    beep_timer->singleShot(1, this, SLOT(buzzer_off()));
    man3^=1;
    if(man3==1)
    {
    melt3=true;
    ui->Initial_melt3->setEnabled(false);
    manual_strt3=block_temp;
    ui->Initial_melt3->setStyleSheet("QPushButton {color: blue;}");
    ui->Initial_melt3->setText("End Melt3");
    delay_timer->singleShot(2000,this,SLOT(delay_button()));

    }
    else if(man3==0)
    {
     melt3=false;
     manual_end3=block_temp;
     ui->Initial_melt3->setStyleSheet("QPushButton {color: grey;}");
     ui->Initial_melt3->setEnabled(false);
    }
}



void MainWindow::on_manual_clicked()
{
    ui->automatic->setChecked(false);
    ui->manual->setChecked(true);
    manual_operation=1;
}

void MainWindow::on_automatic_clicked()
{
    ui->manual->setChecked(false);
    ui->automatic->setChecked(true);
    manual_operation=0;
}
void MainWindow::delay_button()
{
    if(melt1==true)
        ui->Initial_melt1->setEnabled(true);
    if(melt2==true)
        ui->Initial_melt2->setEnabled(true);
    if(melt3==true)
        ui->Initial_melt3->setEnabled(true);

}
