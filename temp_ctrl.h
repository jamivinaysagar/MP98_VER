#ifndef TEMP_CTRL_H
#define TEMP_CTRL_H

#include "gpio.h"
#include <QVector>
#include  <QTimer>

class Temp_Ctrl : public QObject
{
    Q_OBJECT

public:

//    QTimer* timer;
    bool run_loop,pre_button,fan_run,end_temp_reached;
    int fd_spi;
    int pwm_fd,sbt_cntl,initial_integral;
    bool var,pidstrt,start_melt,timer,preht,oshoot,vari,heater_on,pid_tune,strt_temp_reached;
    float overshoot,dty_cycl,intgrl_new,prsnt_val;
    unsigned char rx_buff[3];
    unsigned char tx_buff;
    float init_integral[15];
    QVector <float> accum_temp;
//
 //   double Rcal1,Rprevcal1;
//    double ADCprevcal1,ADCcal1;
 //   double ADCnorm1;
   // QVector <float> accum_temp;
    //float accum_temp[10];
    //unsigned char var_inc;
    void adc_init();
    void pid_tuning();//14547 //pid tuning is done here
    void fan_cond();
    explicit Temp_Ctrl(QObject *parent = 0);
    float pre_val;
    QTimer *ovrsht_timer;

signals:
    void finished();
   // void send_fan_signal(bool);
    //  void calc_pid_values();
   // void send_pwm_signal(bool);
 //   void call_pid_output();
//    void set_freq(float);
    void emit_start_temp_reached();
    void emit_end_temp_reached();
    void display_pv();
    void send_pid_strt(float,float);


public slots:

//    void receive_from_mw(bool);
    void ramp_setpoint();
    void fan_signal(bool x);
    void set_freq(float);
    void pid_output();
    void pre_temp();//14547 // to find one sec delayed temp
    //void pre_heat();

    void start_loop(bool);
    void control_loop();
    void adc_reinit();
    void receive_pid_signal(bool);
    void receive_fan_setpoint(float);//14547
    void receive_setpoint(float,float,float,float,float,float,float,int,bool);
    void receive_sample_calib_factors(double,double,bool);//11425
 //   void receive_pv(float);
//    void fan_signal(bool);
//    void ramp_setpoint();
 //  void pid_output();
    void receive_pwm_signal(bool);
    void receive_pid_strt(bool);
    void receive_sbt_cntrl(bool);
    void receive_melting(bool);
    void receive_field_calib_factors(double,bool);


public:
    //float block_temp;
    float start_temp;
    double cfield,cfield_present;
    bool field,present_calib;
    float end_temp,rate,temp_rate;
    float set_point,fan_stpt;
    float ramp_const;
    bool pid_run;
    bool start_signal_emitted,fan_on;
    bool end_signal_emitted,ramp_start,pwm_ready;
    float pterm, dterm, result, ferror,deadband;
    float ramp_sp,integral,temp_integral;
    int ret,fd,time_cycle;
    bool SBT_flag;
    bool pwm_open_flag;
    bool limit,start_melt_integral_flag;
    double receive_avg_mulfactor,receive_avg_offset,receive_status;//11425
    GPIO *test;
    float p,Ti,sbt;
};

#endif // TEMP_CTRL_H
