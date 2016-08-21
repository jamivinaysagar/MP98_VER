#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "profile.h"
#include "edit_profile.h"
#include "login.h"
#include "movie_disp.h"
#include "temp_ctrl.h"
#include <QProcess>
#include <QRegExpValidator>
#include "qcustomplot.h"
#include <QDirModel>
//#include <QFileSystemModel>
//#include "result_viewer.h"
#include "camera.h"
#include "logoff.h"
#include <QWSServer>

extern "C"
{
#include "/install/ffmpeg/usr/include/libavcodec/avcodec.h"
#include "/install/ffmpeg/usr/include/libavformat/avformat.h"
#include "/install/ffmpeg/usr/include/libswscale/swscale.h"
}
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    FILE *ffmpeg;
    int num_of_profiles; // this may not work. we may have to hardcode 12.
    Profile* profiles[12];
    Profile* curr_profile;
    Profile* temp_profile;
    movie_disp* movie_display;
    edit_profile*  profile_form;
    QString equ_id;
    login* login_form;//11425
    logoff* logoff_form;
    QString sample_calib_name[4];//11425
    float sample_calib_rgl[4],sample_calib_rgh[4],cmp_fac[4],Mmp_fac[4],mfactory[3],cfactory[3],cfield[4],cfield_past[4];//11425
    float cfield_temp[4];
    int single_range[4];
    int sample_profile_sel;//11425
    int manual_operation;
    // QProcess* spi_process;
    QProcess* recorder_process;
    char *ffmpeg_filename;
    bool mlt1,mlt2,mlt3,fin1,fin2,fin3;
    bool toggle_graph1,toggle_graph2,toggle_graph3;
    bool factory_signin,authorized_signin;
    bool stop_check_calib,factory,field;
    //   float off_set,slope;
    //   float block_temp;
    //    float SCALE1,ROFFSET1;
    //    double Rcal1,Rprevcal1;
    //    int chkram;
    //    double ADCprevcal1,ADCcal1;
    //    double ADCnorm1;
//    QImage imagex,imagey;
//    QMutex qm;
    bool screen;//14547 07-10-14
    bool results_saved;
    int profile_shown,calib_cnt;
    bool hist_cmp,fan;
    bool thermo_calib_flag;//11425
    bool password_ok_flag;
    bool preheat,fanon,end,start;//14547
    float high_value,low_value,start_temp,end_temp,ramp_rate,p,Ti,d,sbt;
    int ramp_cnt,sl_date_flag;//11425
    float smallest, largest;
    float avg_start,avg_end,sum_start,sum_end;
    int melt_min,melt_max,end_min,end_max;
    int final_start_melt1,final_end_melt1,final_start_melt2,final_end_melt2,final_start_melt3,final_end_melt3;
    float start_mp1,end_mp1,start_mp2,end_mp2,start_mp3,end_mp3;
    float MP_therm1,MP_therm2,MP_therm3,min_range,max_range,index1,index2,index3;
    float onset_pnt1,onset_pnt2,onset_pnt3,clear_pnt1,clear_pnt2,clear_pnt3;
    bool clear1,clear2,clear3;
    int index_min1,index_min2,index_min3;
    float start_temp_raw1,start_temp_raw2,start_temp_raw3,end_temp_raw1,end_temp_raw2,end_temp_raw3;
    //    int pwm_fd;
    //  QVector <float> accum_temp;
    QVector <double> new_sample1_slope,new_sample2_slope,new_sample3_slope,new_sample1,new_sample2,new_sample3,new_temp,temp_array,temp_array_final,temp_array1,temp_sample1,temp_sample2,temp_sample3;
    QVector <double> sample1,sample2,sample3,sample11,sample22,sample33,sample44,sample4,sample_raw1,sample_raw2,sample_raw3,temp_raw,sample_raw_slope1,sample_raw_slope2,sample_raw_slope3,temp_raw_slope,sample1_slope,temperature_sample1_slope;
    QVector <double> sample_raw_int1,sample_raw_int2,sample_raw_int3,sample_raw_int_slope1,sample_raw_int_slope2,sample_raw_int_slope3,temp_raw_int_slope,sample_raw_smooth1,sample_raw_smooth2,sample_raw_smooth3;
    QVector <double> sample_raw_smooth_slope1,sample_raw_smooth_slope2,sample_raw_smooth_slope3,temp_raw_smooth_slope,sample_raw_int_smooth1,sample_raw_int_smooth2,sample_raw_int_smooth3,sample_raw_int_smooth_slope1,sample_raw_int_smooth_slope2,sample_raw_int_smooth_slope3,temp_raw_int_smooth_slope;
   QVector <double> sample_raw_round1,sample_raw_round2,sample_raw_round3,sample_raw_round_slope1,temp_raw_round_slope,sample_raw_5point_slope1,temp_raw_5point_slope;
   QVector <double> sample1_norm,sample1_gauss,sample2_norm,sample3_norm;
   QVector <QTime> time_array;
    QCustomPlot *customPlot;
    int time_cycle,calib_days,calib_factory_months;
    int first_time_okkey;//11425
    int toogle_status;
    //   QTime time;
    QTimer /*pid_timer,*spi_timer,*/*camera_timer, *temp_timer,*ovrsht_timer, *delay_timer;
    QString sPath;
    // QThread *thread_1;
    QThread /*thread,*/*thread_spi;
    QRect qr1, qr2, qr3;
    camera* camera1;
    //bool image_received;
    CvRect cvRect1,cvRect2,cvRect3;
    bool sample_1_onset_detected_bool;
    float sample_1_onset_temp;
    bool sample_2_onset_detected_bool;
    float sample_2_onset_temp;
    bool sample_3_onset_detected_bool;
    float sample_3_onset_temp;
    bool reset,field_reset;
    bool sample_1_single_detected_bool;
    float sample_1_single_temp;
    bool sample_2_single_detected_bool;
    float sample_2_single_temp;
    bool sample_3_single_detected_bool;
    float sample_3_single_temp;
    float pterm,integral;//14547

    bool sample_1_clear_detected_bool;
    float sample_1_clear_temp;
    bool sample_2_clear_detected_bool;
    float sample_2_clear_temp;
    bool sample_3_clear_detected_bool;
    float sample_3_clear_temp;
    int area_x,area_y;//camera image area
    double sample_avg_ini_calib[4],avg_mulfactor,avg_offset,sample_avg_final_calib[4],thermo_avg_final_calib[6];//11425

    double final_sample_avg_ini_calib[4],final_sample_avg_final_calib[4];
    float final_sample_calib_rgl[4],final_sample_calib_rgh[4];
    float smallest_init[4],largest_final[4];//14547 calibration mim max range

    float thermo_ramp_rate[6],thermo_start_temp,thermo_stop_temp,thermo_correction_factor;//11425
    bool onset_clear;
    int No_ramp_cycles,Noofsamples,final_Noofsamples,first_flag,second_flag,flag_array[2];//11425
    QString thermo_chemical_name;
    QString Company_name,company_add1,company_add2,company_add3,left_tag,right_tag;//14547
    bool calib_flag;//11425
    int past_calib,present_calib;
    double start_temp_mp1,end_temp_mp1,start_temp_mp2,end_temp_mp2,start_temp_mp3,end_temp_mp3;
    bool sample_mp_flag1,sample_mp_flag2,sample_mp_flag3,sample_mp_flag4,sample_mp_flag5,sample_mp_flag6;
    bool graph_enable_flag1,graph_enable_flag2,graph_enable_flag3;

    double temp_onset1,temp_onset2,temp_onset3,temp_clear1,temp_clear2,temp_clear3;
    double stop_sample1,stop_sample2,stop_sample3;
    double stop_temp_index1, stop_temp_index2, stop_temp_index3;
    double temp_final_measured[4],temp_final_actual[4],temp_val,slopeman,offsetman;
    double min1,min2,min3,min4,max1,max2,max3,max4,min11,min22,min33,max11,max22,max33,min_slope,max_slope;
    float mean_avg,start_sample1,start_sample2,start_sample3;
    int avg_den;
    bool found1,found2,found3;
    bool save_mov,recording,calib_pdf_report;
    QString filename_pdf;
    QString filename_jpg;
    QString filename_mov,filename22;
    QString fname;
    Temp_Ctrl* temp_control;


    AVFormatContext *pFormatCtx; // important to set NULL
    int              videoStream;
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;
    AVFrame         *pFrame;
    AVFrame         *pFrameRGB;
    AVPacket        packet;
    int             frame_finished;
    int             numBytes;
    uint8_t         *buffer;
    AVStream *video_st;
    bool pause,stop_clicked;
    QString filename,filename11;
    char *pp;
    QTimer *mov_timer,*login_timer,*blink_timer,*logoff_timer,*beep_timer;//11425
    bool camera_once_flag;
    //calibration variables
    double avg_ini_melt_pnt,avg_final_melt_pnt;//11425
    char avg_var;//11425
    int factory_calib,field_calib,field_calib_present;
    double min_sample1,min_sample2,min_sample3,min_sample4;
    double max_sample1,max_sample2,max_sample3,max_sample4;
    bool one_time_flag,stop_button_flag,treeview_open_flag;
    double pmin1, pmin2, pmin3, pmax1,pmax2,pmax3;
    bool log;
    QString operator_name;
    bool factory_pdf,field_pdf;
    int calib_cmplt;
    bool single1,single2,single3,single4;
//    bool startmelt_enable_flag;
    int users,calib_receive_usr,calib_usr,loged_user;//11425
    int temp_area_x,temp_area_y;//11425
    int model_no,temp_model_no;//14547
    QString ip,mask,gateway;
    QString password,username; //11425
    QString ip1,ip2,ip3,ip4,mk1,mk2,mk3,mk4,gw1,gw2,gw3,gw4;
    int index_tabwidget,update_index;
    QModelIndex index5;
    QString user_name[11],user_password[11],calib_date,calib_factory_date,serial_no,mfg_date,calib_due_date,dt_cur,calib_factory_duedate;
    QString am_pm,receive_button_text;
    QString calib_measure_values[4];
    QString calib_alert_date ;
    QImage strt_img1,strt_img2,strt_img3,end_img1,end_img2,end_img3;
    int denom;//14547
    int frame_no_strt1,frame_no_strt2,frame_no_strt3,frame_no_end1,frame_no_end2,frame_no_end3;
    void avg_save_file(void);
    void pdf_report(void);
    void buzzer_on();
    void read_rtc();
    int fd_rtc;
    struct rtc_time rtc_tm;
    int man1,man2,man3;
    float manual_strt1,manual_strt2,manual_strt3,manual_end1,manual_end2,manual_end3;
    bool melt1,melt2,melt3;


    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QDirModel *model; //change to qfile system model?      QFileSystemModel *model = new QFileSystemModel;
public slots:
    void update_status(wchar_t );
    void receive_from_login(char,int);//11425
    void receive_display_pv();
    void receive_play_pause_movie_display(char);
    void receive_stop_movie_display(bool);
    //void update_status(QString);
    bool check_setup_params();
    void initialize_profiles();
    bool new_profile_check(Profile* check_profile);
    bool thermo_dynamic_screen_check();//11425
    void thermo_sample_params();//11425
    void update_Profile();
    void update_profile_screen();
    void receiveImage(QImage);
    void update_profile_form(int);
    void ok_profile_form(Profile*);
    void start_temp_reached();
    void end_temp_reached();
    void shutdown();
    void update_mw_params(int);
    void updateGray(double,double,double);
    //void start_worker_thread();
    void save_Results();
    // void WritetoRecorder(QImage);
    //void hist_cmp_on_off_m(bool);

    void recording_Start();
    void recording_Stop();

    void save_movie(QString);

    void start_player();
    void loop_player();
    void stop_player();
    void setup_player();
    void raise_login_form();//11425
    void receive_from_sample_form(bool,int);//11425
    void thermo_sample_display();//11425
    void update_sample_params(int i);//11425
    void mainscreen();//11425
    void calc_mulfactor_offset();//11425
    void hide_sample_pushbuttons();
    QString res_adding(float receive_temp);//11425
    void user_push_buttons_hide();
    void user_push_buttons_show();
    void Authorised_hide();
    void Authorised_show();
    void user_hide();
    void user_show();
    void updated_username_password(int receive_user);
    void update_password_screen();//11425
    void user_password_lineEdit_show(QString user_name1,QString password1);
    void ramprate_configuration_screen_hide();
    void camera_calib_image_reading();
    void staus_message();
    void hide_main_tab(char first,char second,char third,char fourth,char fifth);
    void hide_main_tab3(char first,char second,char third,char fourth,char fifth,char sixth);
    void conformation_delete_data();
    void show_standard1();
    void show_standard2();
    void show_standard3();
    void show_standard4();
    void hide_standard1();
    void hide_standard2();
    void hide_standard3();
    void hide_standard4();
    void receive_standard(QString standard,float start_temp,float end_temp,int standard_no,int x);
    QString button_text(QString chemical_name,float start,float end,bool x);
    void Show_button_text1();
    void Show_button_text2();
    void Show_button_text3();
    void Show_button_text4();
    void Noof_standards_show();
    void Noof_standards_hide();
    void calibration_paasword_show();
    void calibration_paasword_hide();
    void calibration_History();
    void calculate_offset_mulfactor();
    void hiding_calib(bool status_flag);
    void report_tags_hide();//14547
    void report_tags_show();//14547
    void cool_mode();
    void log_off();
    void buzzer_off();
    void clear_fields();
    void delay_button();

   // void receive_pid(float,float);


private slots:

    void on_pushButton_39_clicked();
    void on_pushButton_40_clicked();
    void on_pushButton_41_clicked();
    void on_pushButton_42_clicked();
    void on_pushButton_43_clicked();
    void on_pushButton_44_clicked();
    void on_pushButton_45_clicked();
    void on_pushButton_46_clicked();
    void on_pushButton_47_clicked();
    void on_pushButton_48_clicked();
    void on_pushButton_49_clicked();
    void on_pushButton_50_clicked();

    void on_pushButton_clicked();
    void on_pushButton_18_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_16_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_14_clicked();
    void on_pushButton_15_clicked();

    void on_pushButton_17_clicked();

//    void on_treeView_doubleClicked(const QModelIndex &index);

    void on_pushButton_21_clicked();

    void on_pushButton_22_clicked();

    void on_pushButton_24_clicked();

    void on_pushButton_23_clicked();

    void on_pushButton_20_clicked();

    void on_tabWidget_currentChanged(int index);
/************ roi push buttons**************************/
   /* void on_pushButton_26_clicked();

    void on_pushButton_28_clicked();

    void on_pushButton_27_clicked();

    void on_pushButton_29_clicked();*/

    //void on_treeView_clicked(const QModelIndex &index);

    void on_delete_button_clicked();

    void on_tabWidget_3_currentChanged(int index);

    void on_pushButton_19_clicked();

    void on_open_button_clicked();

   // void on_treeView_clicked(const QModelIndex &index);

//    void on_dateTimeEdit_dateTimeChanged(const QDateTime &date);

    void on_pushButton_54_clicked();

    void on_single_sel_button_clicked();

    void on_pushButton_99_clicked();

    void on_pushButton_102_clicked();

    void on_pushButton_101_clicked();

    void on_pushButton_98_clicked();

    void on_pushButton_32_clicked();

    void on_pushButton_33_clicked();

    void on_pushButton_34_clicked();

    void on_pushButton_96_clicked();

    void on_pushButton_100_clicked();

    void on_pushButton_97_clicked();

    void on_pushButton_38_clicked();

    void on_pushButton_51_clicked();

    void on_pushButton_66_clicked();

    void on_pushButton_52_clicked();

    void on_pushButton_65_clicked();

    void on_pushButton_53_clicked();

    void on_pushButton_55_clicked();

    void on_pushButton_56_clicked();

    void on_pushButton_58_clicked();

    void on_pushButton_59_clicked();

    void on_pushButton_60_clicked();

    void on_pushButton_61_clicked();

    void on_pushButton_57_clicked();

    void on_pushButton_62_clicked();

    void on_pushButton_63_clicked();

    void on_pushButton_64_clicked();

    void on_pushButton_67_clicked();

    void on_pushButton_68_clicked();

    void on_pushButton_69_clicked();

    void on_pushButton_70_clicked();

    void on_pushButton_89_clicked();

    void on_pushButton_90_clicked();

    void on_pushButton_91_clicked();

    void on_pushButton_95_clicked();

    void on_pushButton_74_clicked();

    void on_pushButton_75_clicked();

    void on_pushButton_76_clicked();

    void on_Model_no_clicked();

    void on_pushButton_78_clicked();

    void on_pushButton_92_clicked();


    void on_pushButton_81_clicked();

    void on_pushButton_82_clicked();

    //void on_pushButton_77_clicked();

    void on_pushButton_83_clicked();

    void on_pushButton_84_clicked();

    void on_pushButton_85_clicked();


    void on_factory_calib_button_clicked();

    void on_radioButton_clicked();

    void on_radioButton_5_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_6_clicked();

    void on_radioButton_3_clicked();

    void on_radioButton_7_clicked();

    void on_radioButton_4_clicked();

    void on_radioButton_8_clicked();

    void on_field_calib_button_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_25_clicked();

    void on_tabWidget_4_currentChanged(int index);

    void on_tabWidget_5_currentChanged(int index);

   // void on_tabWidget_8_currentChanged(int index);


   // void on_pushButton_30_clicked();


    void on_reset_clicked();

    void on_Initial_melt1_clicked();

    void on_Initial_melt2_clicked();

    void on_Initial_melt3_clicked();



    void on_manual_clicked();

    void on_automatic_clicked();

signals:
    void send_rtc_values(bool);
    void send_from_sample_form(bool,int);//11425
   // void receive_from_login(char);//11425
   // void send_end_signal(bool x);
//    void send_to_temp_ctrl(bool);
    void emit_start_loop(bool);
    void movie_completed();
 // void finished();
//  void update_all(float);
    void call_update_form(Profile*);
    void send_pid_signal(bool);
    void send_setpoint(float,float,float,float,float,float,float,int,bool);
    void send_pwm_signal(bool);
    void emit_display_picture(QString);
    void emit_display_movie(QString);
    void emit_start_worker_thread();
    void reset_window_buttons();
    void emit_update_roi(CvRect,CvRect,CvRect);
    void sendImageX(QImage);
    void emit_start_stop_cam(bool);
    void send_hist_cmp(bool);
    void send_Start_Recording();
    void send_Stop_Recording();
    void call_setup_camera();
    void call_stop_camera();
    void frameready(QImage,char);
    void pause_clicked(bool);
    void stop_clicked_1(bool);
    void send_usr_psw(QString,QString);//11425
    void send_sample_profile(float,float,float,int,bool,bool);//11425
    void send_sample_calib_factors(double,double,bool);
    void send_image_area_para(int,int);
    void pid_strt(bool);
    void sbt_cntrl(bool);//14547
    void fan_setpoint(float);
    void start_melting(bool);
    void send_field_calib_factors(double,bool);
    void send_field_calib_factors_present(double);



};

#endif // MAINWINDOW_H
