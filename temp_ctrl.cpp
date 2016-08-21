#include "temp_ctrl.h"

#include  <QTimer>
#include "gpio.h"
#include <QDebug>
#include <math.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <QApplication>
#include <QTime>

#define Z1 -0.0039083
#define Z2  0.0000175848
#define Z3 -0.00000002310
#define Z4  0.000001155
//---------------------
#define Z5 -242.02
#define Z6  2.2228
#define Z7  0.0025859
#define Z8 -0.0000048260
#define Z9 -0.000000028183
#define Z10 0.00000000015243
#define Z11 -242.97
#define Z12 2.2838
#define Z13 0.0014727


extern float block_temp;
extern double ADCnorm1;
extern double ADCprevcal1,ADCcal1;
extern double Rcal1,Rprevcal1;
extern float SCALE1,ROFFSET1;
float ovrsht[15]={11.5,10.5,9.5,9.0,7.7,6.9,5.8,4.9,4.0,3.0,2.5,1.8,1.2,0.5,0.2};
float duty_cycle[15]={10.70,13.22,17.01,21.17,26.03,30.93,36.30,42.17,49.24,56.16,63.95,71.11,79.70,89.30,99.90};
//extern bool startmelt_enable_flag;

Temp_Ctrl::Temp_Ctrl(QObject *parent) :
    QObject(parent)
{
    start_signal_emitted = false;
    end_signal_emitted = false;
    deadband = 0;
    fan_on = false;
    pid_run = false;
    pwm_open_flag = false;
    limit = false;
    var =false;
    SBT_flag = false;
    ramp_start = true;
   // var_inc = 0;
    fan_signal(false);
    pidstrt=false;//14547
    timer=false;
    preht=true;
    vari=true;
    oshoot=false;// one time variable for overshoot
    start_melt=false;//14547 variable to detected mode of operation
    pid_tune=false;// 14547 used for pid tuning
    pre_button=false;//14547 variable for preheat mode
    fan_run=false;//14547 for fan conditions
    initial_integral=false;//one time variable for initial integral
    present_calib=false;
    for (int i=0; i<15; i++)
   {
        init_integral[i]=duty_cycle[i]-(0.08*duty_cycle[i]);
    }
   // accum_temp.resize(10);
    ovrsht_timer = new QTimer;
    connect(ovrsht_timer,SIGNAL(timeout()),this,SLOT(pre_temp()));
    //ovrsht_timer->start(10000);

   }



void Temp_Ctrl::control_loop()
{
    unsigned char spi_brk_cntr = 0;
    // initializing ADC
    static uint8_t mode;
    static uint8_t bits = 8;
    static uint32_t speed = 1000000;

    //------------------------- spi init
    int ret = 0;

    fd_spi = open("/dev/spidev0.0", O_RDWR);

    if (fd_spi < 0)
        printf("can't open device");

    ret = ioctl(fd_spi, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
        printf("can't set spi mode");

    ret = ioctl(fd_spi, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
        printf("can't get spi mode");

    ret = ioctl(fd_spi, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
        printf("can't set bits per word");

    ret = ioctl(fd_spi, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
        printf("can't get bits per word");

    ret = ioctl(fd_spi, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        printf("can't set max speed hz");

    ret = ioctl(fd_spi, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        printf("can't get max speed hz");

    printf("spi mode: %d\n", mode);
    printf("bits per word: %d\n", bits);
    printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

    tx_buff = 0x03;
    write(fd_spi,&tx_buff,1);

    tx_buff = 0x0d;
    write(fd_spi,&tx_buff,1);

    //mode register
    tx_buff = 0x01;
    write(fd_spi,&tx_buff,1);

    tx_buff = 0x12;
    write(fd_spi,&tx_buff,1);

    //I/O control register
    tx_buff = 0x07;
    write(fd_spi,&tx_buff,1);

    tx_buff = 0x30;
    write(fd_spi,&tx_buff,1);

    //ADC control register
    tx_buff = 0x02;
    write(fd_spi,&tx_buff,1);

    tx_buff = 0x9b;
    write(fd_spi,&tx_buff,1);

    usleep(1000000);

    tx_buff = 0x40;
    write(fd_spi,&tx_buff,1);
    //reading chomp1
    read(fd_spi,rx_buff,1);
    //-------------------------
    //Init PWM
     pwm_fd = open("/dev/pwm",0);

    while(run_loop)
    {


        //this->exec();
        //this->
        //     qDebug() << "Start ADC time" << QTime::currentTime()<<"   "<<QTime::currentTime().msec();
        QApplication::processEvents();
        //  qDebug() << "Start ADC time" << QTime::currentTime().msec();
        long adcval;
        float temp1chvalue;//tempr1,tempr2;
        float temp_ch;
        float chvalue;

        while((rx_buff[0]&0x80) != 0)
        {
            if(spi_brk_cntr++ > 1)
            {
                adc_init();
                spi_brk_cntr = 2;
            }
        }

        {
            spi_brk_cntr = 0;
            tx_buff = 0x44;
            write(fd_spi,&tx_buff,1);

            read(fd_spi,rx_buff,3);

            adcval = 0; // clear adcval

            for(int i=0; i<2; i++) // arrange 3 bytes of adc reading
            {					// into a single long value
                adcval |= rx_buff[i];
                adcval = adcval << 8;
            }

            adcval |= rx_buff[2];

            for(int i=0; i<3; i++)
                rx_buff[i] = 0;

            chvalue = adcval - 0;

            ADCnorm1=chvalue*0.000000060381;//0.000000060954;
            chvalue=(ADCnorm1*SCALE1)+ROFFSET1;//resistance out
            if(chvalue>=100)
            {
                temp1chvalue=Z2+(Z3*chvalue);
                chvalue =-((Z1+(sqrt(temp1chvalue)))/Z4)*10.0;
            }
            else
            {
                //fifthorder
                chvalue=Z5+(Z6*chvalue)+(Z7*chvalue*chvalue)+(Z8*chvalue*chvalue*chvalue)+(Z9*chvalue*chvalue*chvalue*chvalue)+(Z10*chvalue*chvalue*chvalue*chvalue*chvalue);
                chvalue=chvalue*10;
            }
            temp_ch = chvalue/10.0;
            if(receive_status==false)
            {
                temp_ch=(temp_ch*receive_avg_mulfactor)+receive_avg_offset;
               // qDebug() <<"slope"<<receive_avg_mulfactor;
                //qDebug() <<"offset"<<receive_avg_offset;

            }
            if(field==true)
            {
                temp_ch=temp_ch+cfield;
                //qDebug() <<"cfield"<<cfield;

            }

           // qDebug() << "Temp_ch: "<<temp_ch;
            adc_reinit();
        }
      // qDebug() << "temp_ch:"<<temp_ch<< "time" << QTime::currentTime()<<"in msec"<<QTime::currentTime().msec();;
        if(accum_temp.size() > 10)
        {
           //     qDebug() << " In accum _temp loop" << accum_temp.size();
            limit = true;
            accum_temp.remove(0);
            //accum_temp.pop_front();
        }
        accum_temp.append(temp_ch);

        float sum = 0;

      //  for(int i=0;i<10;i++)

          //  qDebug()<<"accum_temp.at(i):"<< accum_temp.at(i)<<"time" << QTime::currentTime()<<"in msec  "<<QTime::currentTime().msec();

       for(int i=0;i<10;i++)

            sum = sum + accum_temp.at(i);

           // qDebug() << "sum = "<<sum;


        float temp= sum/10;

        block_temp = (int)(temp / 0.1) * 0.1;
       // qDebug()<<"block_temp = "<<block_temp;//<< "time:"<< QTime::currentTime()<<"in msec"<<QTime::currentTime().msec();

        if(limit)
        {
            emit display_pv();

        }

        {
            // qDebug() << "in Ramp_setpoint";
            ramp_setpoint();
            fan_cond();
        }
       // qDebug() <<"before usleep"<<QTime::currentTime()<<"   "<<QTime::currentTime().msec();
        usleep(90000);

    }
    emit finished(); // check this agiain


}

void Temp_Ctrl::start_loop(bool x)
{

    run_loop = x;
}

void Temp_Ctrl::receive_setpoint(float x, float a, float b, float c, float d, float e, float f, int time_cycle1,bool x1)
{
    // qDebug() << "In set point mode";
    ovrsht_timer->start(1000);//timer of 1sec used for overshoot
    qDebug()<<"timer strtd";
    pidstrt=true;
    set_point = x;
    start_temp = a;
    end_temp = b;
//    ramp_start = true;
    rate = c;
    temp_rate = rate;
    p = d;

    Ti  = e;
    sbt = f;
    time_cycle = time_cycle1;
    var = x1;
    if(set_point<=62.5)
    {overshoot=ovrsht[0];intgrl_new=init_integral[0]; }
    else if((set_point>62.5)&&(set_point<=87.5))
    {overshoot=ovrsht[1];intgrl_new=init_integral[1]; }
    else if((set_point>87.5)&&(set_point<=112.5))
     {overshoot=ovrsht[2];intgrl_new=init_integral[2]; }
    else if((set_point>112.5)&&(set_point<=137.5))
     {overshoot=ovrsht[3];intgrl_new=init_integral[3]; }
    else if((set_point>137.5)&&(set_point<=162.5))
     {overshoot=ovrsht[4];intgrl_new=init_integral[4]; }
    else if((set_point>162.5)&&(set_point<=187.5))
     {overshoot=ovrsht[5];intgrl_new=init_integral[5]; }
    else if((set_point>187.5)&&(set_point<=212.5))
     {overshoot=ovrsht[6];intgrl_new=init_integral[6]; }
    else if((set_point>212.5)&&(set_point<=237.5))
     {overshoot=ovrsht[7];intgrl_new=init_integral[7]; }
    else if((set_point>237.5)&&(set_point<=262.5))
     {overshoot=ovrsht[8];intgrl_new=init_integral[8]; }
    else if((set_point>262.5)&&(set_point<=287.5))
     {overshoot=ovrsht[9];intgrl_new=init_integral[9]; }
    else if((set_point>287.5)&&(set_point<=312.5))
     {overshoot=ovrsht[10];intgrl_new=init_integral[10]; }
    else if((set_point>312.5)&&(set_point<=337.5))
     {overshoot=ovrsht[11];intgrl_new=init_integral[11]; }
    else if((set_point>337.5)&&(set_point<=362.5))
     {overshoot=ovrsht[12];intgrl_new=init_integral[12]; }
    else if((set_point>362.5)&&(set_point<=387.5))
     {overshoot=ovrsht[13];intgrl_new=init_integral[13]; }
    else
     {overshoot=ovrsht[14];intgrl_new=init_integral[14]; }

    if(var==false)
    {
      //  start_melt=false;
        pid_tune=false;
        oshoot=false;
       // preht=true;
    }




}
void Temp_Ctrl::receive_melting(bool x)
{
    strt_temp_reached=x;
}
void Temp_Ctrl::receive_fan_setpoint(float x)
{
     fan_stpt=x;
     fan_run=true;
}

void Temp_Ctrl::receive_pid_signal(bool x )
{

    pid_run = x;
    ramp_start = true;
    if(x == false)
        start_melt_integral_flag = false;
    integral = 0;
    SBT_flag = false;
    start_signal_emitted = false;
    end_signal_emitted = false;


}
void Temp_Ctrl::receive_pid_strt(bool x)
{
    if(x==false)
    {
   oshoot=false;
   preht=true;
   heater_on=true;
   start_melt=false;
   pre_button=true;
   initial_integral=true;

    }
   else
   {
      preht=false;
      start_melt=true;
      pre_button=false;
      initial_integral=false;
   }

}
void Temp_Ctrl::receive_sbt_cntrl(bool x)
{
    sbt_cntl=x;
}

void Temp_Ctrl::fan_signal(bool x)
{
    QString command;
    if(x)
    {


        command = "echo \"high\" > /sys/class/gpio/gpio160/direction";

        system(qPrintable(command));

        command = "echo \"1\" > /sys/class/gpio/gpio160/value";

        system(qPrintable(command));
    }
    else
    {

        command = "echo \"high\" > /sys/class/gpio/gpio160/direction";

        system(qPrintable(command));

        command = "echo \"0\" > /sys/class/gpio/gpio160/value";

        system(qPrintable(command));
    }



}
void Temp_Ctrl::fan_cond()//fan controlling
{
    if(fan_run == true)
    {

        if((block_temp > (fan_stpt)))
        {
            if(!fan_on)
            {

                fan_signal(true);

                fan_on  = true;


            }
        }
        else
        {
            if(fan_on == true)
            {
                fan_signal(false);
                fan_on = false;
            }
       }
    }
}
void Temp_Ctrl::ramp_setpoint()
{



    if(pid_run == true)
    {

        if((start_melt==false)&&(pre_button==true))//preheat condition
    {


         if(block_temp <= (set_point-overshoot-2))
        {
            set_freq(99.9);//heater 99.9% on
            heater_on=true;
           // qDebug()<<"heater on 99%";

        }
         else
        {



             if((pid_tune==false))//making pwm freq 0
             {
                   set_freq(0);
                  // qDebug()<<"heater off ";

             }

               if((preht==true))
                {
                 //
                if((pre_val>block_temp))//comparision for overshoot
               {

                 oshoot=true;
                // qDebug()<<"In overshoot";
                // preht=false;
                }
             }

            if((oshoot==true)&&(block_temp<=set_point))//pid on after overshoot
              {

                 pid_tune=true;
               //  oshoot=false;
            }
            if(pid_tune==true)
            {
                //qDebug()<<"in pid tuning";
                pid_tuning();

            }


        }
    }
        else // pid tuning during melting process
         {


            if((block_temp>end_temp))
             {
                 set_freq(0);
                 strt_temp_reached=false;

             }
            if((block_temp<=set_point))
            {
               strt_temp_reached=true;

            }
            if( strt_temp_reached==true)
            {
                pid_tuning();

            }


     }
}
}

void Temp_Ctrl::pre_temp()
{

    if(vari==true)
    {
    pre_val=block_temp;
    vari=false;
    }
    else
    {
        prsnt_val=block_temp;
        vari=true;
    }
    ovrsht_timer->start(1000);

}
void Temp_Ctrl::pid_tuning()
{

    if (ramp_start)
       {
           ramp_sp = block_temp + rate;
           ramp_start = false;
           fan_signal(false);
       }



       if(block_temp < set_point)
       {
           // qDebug
           if ((set_point - block_temp) < sbt)
           {
               if ((!SBT_flag) && (sbt_cntl))
               {
                   //integral = 0;//integral/4;//11425

                   ramp_const = set_point - block_temp;
                   SBT_flag = true;
               }

               rate = temp_rate * (set_point - block_temp)/sbt; // change this. we need from rate to zero
           }

       }

       if ((ramp_sp - block_temp) <= rate)
       {
           ramp_sp = ramp_sp + (rate * time_cycle)/(qreal(6e4)); // converting rate from deg/min to deg/msec
       }
            //qDebug() << "Ramp set point : " << ramp_sp;
       if (ramp_sp >= set_point)
           ramp_sp = set_point;
       pid_output();


}
void Temp_Ctrl::pid_output()
{
    ferror = ramp_sp - block_temp;

    pterm = p * ferror;
    if (pterm > 100 || pterm < -100)
        integral = 0.0;
    else
    {
        if((start_melt==false)&&(initial_integral==true))
        {
        integral = intgrl_new;//assigning initial integral value
        initial_integral=false;
        }

        integral += (ferror*time_cycle)/(Ti*1000);

        if (integral > 100.0)
            integral=100.0;
        else if (integral < 0.0)
            integral=0.0;
    }
    dterm = 0;//(err - last_error) * dgain;
    result = pterm + integral + dterm;


    if (result > 99.9)
        result = 99.9;
    else
        if(result < 0)
            result = 0;
   // qDebug() <<"initial integral"<<intgrl_new;


   // qDebug() << "setpoint"<<set_point;
    //qDebug() << "p"<<p;
    //qDebug() << "I"<<Ti;
  // qDebug() <<"Time"<<QTime::currentTime();
    //qDebug() <<"block_temp"<<block_temp;
    //qDebug() << "pterm"<<pterm;
   // qDebug() <<"integral"<<integral;
   // qDebug() <<"result"<<result;

    set_freq(result);

    if (block_temp >= start_temp && !start_signal_emitted && ~var)
    {
        if(start_melt_integral_flag == true)
        {
            start_melt_integral_flag = false;
            integral = temp_integral;
            //qDebug() << "Integral value" << integral;
        }
        temp_integral = integral;
        if((block_temp>=start_temp)&&(strt_temp_reached==false))
        {
        emit emit_start_temp_reached();
        start_signal_emitted = true;
        }
        else if((block_temp<=start_temp)&&(strt_temp_reached==true))
        {
          emit emit_start_temp_reached();
          start_signal_emitted = true;
        }


    }
    if (block_temp >= end_temp && !end_signal_emitted && var)
    {
        start_melt_integral_flag = true;
        emit emit_end_temp_reached();
        end_signal_emitted = true;
    }

}

void Temp_Ctrl::adc_reinit()
{
    tx_buff = 0x01;
    write(fd_spi,&tx_buff,1);
    tx_buff = 0x12;
    write(fd_spi,&tx_buff,1);
    tx_buff = 0x40;
    write(fd_spi,&tx_buff,1);
    read(fd_spi,rx_buff,1);
}

void Temp_Ctrl::set_freq(float x)
{
    int y = (int)x;
    ioctl(pwm_fd,1,y);

}

void Temp_Ctrl::receive_pwm_signal(bool x)
{
    if(x)
    {

    }
    else
    {

        ioctl(pwm_fd,0);
    }
}

void Temp_Ctrl::adc_init()
{
    tx_buff = 0x03;
    write(fd_spi,&tx_buff,1);

    tx_buff = 0x0d;
    write(fd_spi,&tx_buff,1);

    //mode register
    tx_buff = 0x01;
    write(fd_spi,&tx_buff,1);

    tx_buff = 0x12;
    write(fd_spi,&tx_buff,1);

    //I/O control register
    tx_buff = 0x07;
    write(fd_spi,&tx_buff,1);

    tx_buff = 0x30;
    write(fd_spi,&tx_buff,1);

    //ADC control register
    tx_buff = 0x02;
    write(fd_spi,&tx_buff,1);

    tx_buff = 0x9b;
    write(fd_spi,&tx_buff,1);

    tx_buff = 0x40;
    write(fd_spi,&tx_buff,1);

    read(fd_spi,rx_buff,1);
}

void Temp_Ctrl::receive_sample_calib_factors(double x,double y,bool z)
{
  receive_avg_mulfactor=x;
  receive_avg_offset=y;
  receive_status=z;
}
void Temp_Ctrl::receive_field_calib_factors(double x,bool z)
{
    cfield=x;
    field=z;
}
