#ifndef CAMERA_H
#define CAMERA_H

#include <QImage>
#include </output/include/opencv/cv.h>
#include </output/include/opencv/highgui.h>
#include <linux/rtc.h>

using namespace cv;


class camera : public QObject
{
    Q_OBJECT
public:
    explicit camera(QObject *parent = 0);
    bool stoploop,hist_cmp;
    int fd1;
    IplImage *image1,*image2,*image3,*first_image1,*first_image2,*first_image3,*image1_1,*first_image1_1;
    bool first_img,first_time;
    Mat src_base1,src_base2,src_base3, src_test1,src_test2,src_test3, hsv_base1,hsv_base2,hsv_base3, hsv_test1,hsv_test2,hsv_test3,src_test1_1,d1,d2,bitwise_anding,src_base1_blur,src_test1_blur;
    double base_base1,base_base2,base_base3, base_test1, base_test2, base_test3;
    QVector <double> sample1,sample2,sample3,sample4;
    CvRect rect1, rect2,rect3;
    //QImage *image123_1;
    MatND hist_base1,hist_test1;
    MatND hist_base2,hist_test2;
    MatND hist_base3,hist_test3;
    int receive_area_x,receive_area_y;
    double sum_cmp,sum_first;
    long whhw;
    bool first_img_one;

    //QVector <double> tem_array,temp_array,tem_array1,tem_array2;
    //QVector <QTime> time_array;
    unsigned char first_image_cntr;
    uchar ba[614400];
    int x;
    int fd_rtc;
    struct rtc_time rtc_tm;
    uchar*  newdata1;
//    ~camera();
signals:
    //   void sendData(uchar*);
    void sendImage(QImage);
    void sendIntensity(double,double,double);
    //void finished();
    
public slots:

    void update_ROI(CvRect,CvRect,CvRect);
    void camera_on_off(bool);
    void hist_cmp_on_off_c(bool);
    //void receive_end_signal(bool);
    void stop();
    void setup();
    void loop();
    void send_image_area_para(int,int);//11425

    
};

#endif // CAMERA_H
