#include "camera.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <QDebug>
//#include"sccb.h"
#include <QApplication>

#include "mainwindow.h"
#include "myinputpanelcontext.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

extern float block_temp;
extern int day,month,year,hour,minute,sec;
using namespace std;
using namespace cv;
int h_bins = 50; int s_bins = 60;//to declare as global variables
int histSize[] = { h_bins, s_bins };
// hue varies from 0 to 256, saturation from 0 to 180
float h_ranges[] = { 0, 256 };
float s_ranges[] = { 0, 180 };
const float* ranges[] = { h_ranges, s_ranges };
// Use the o-th and 1-st channels
int channels[] = { 0, 1 };
double psnr, mse,res1,res2,res3,res2_2 ,sse ,norm_d1_d2,norm_bitwise;


camera::camera(QObject *parent) :
    QObject(parent)
{
    fd1 = open("/dev/camera",0);
    first_img_one = false;
   // first_time=true;

}


void camera::setup()
{
    first_img = true;

/*    rect1.x = 0; rect1.y = 0; rect1.width = 100; rect1.height = 100;
    rect2.x = 100; rect2.y = 0; rect2.width = 100; rect2.height = 100;
    rect3.x = 200; rect3.y = 0; rect3.width = 100; rect3.height = 100;*/
//    hist_cmp = false; // change this to false. true is only for testig.// do we need this anymore?
    x = 0;
    first_image_cntr = 0;

    first_image1 = cvCreateImageHeader( cvSize(400, 300), IPL_DEPTH_8U, 4);
   // qDebug() << "first_image created";
    first_image2 = cvCreateImageHeader( cvSize(400, 300), IPL_DEPTH_8U, 4);
   // qDebug() << "second_image created";
    first_image3 = cvCreateImageHeader( cvSize(400, 300), IPL_DEPTH_8U, 4);
    //first_image12 = cvCreateImageHeader( cvSize(400, 300), IPL_DEPTH_8U, 4);
    //qDebug() << "third_image created";
    newdata1 = (uchar*) malloc(sizeof(uchar) * 480000);//400*300*4(4 channels R,G,B,alpha)
}

void camera::loop()
{

    while(x<10) // why?
    {
        x++;
        read(fd1,ba,614400);
        //qDebug() << "x = " << x;
    }

    read(fd1,ba,614400);
    QImage image123(ba,640,480,1280,QImage::Format_RGB16);//2 bytes of data(R-5,G-6,B-5)

    image123 = image123.copy(receive_area_x,receive_area_y,400,300); //to adjust area of projection

    if (hist_cmp)
    {
        QImage image_32 = image123.convertToFormat(QImage::Format_RGB32);

        image1 = cvCreateImageHeader( cvSize(400, 300), IPL_DEPTH_8U, 4);
        image2 = cvCreateImageHeader( cvSize(400, 300), IPL_DEPTH_8U, 4);
        image3 = cvCreateImageHeader( cvSize(400, 300), IPL_DEPTH_8U, 4);

        uchar* newdata = (uchar*) malloc(sizeof(uchar) * 480000); // do we need to create newdata every time? create before loop and delete after loop?

        memcpy(newdata,image_32.bits(),480000);


        image1->imageData = (char*) newdata;//creating pointers
        image2->imageData = (char*) newdata;
        image3->imageData = (char*) newdata;

        cvSetImageROI(image1,rect1);
        cvSetImageROI(image2,rect2);
        cvSetImageROI(image3,rect3);


        if (first_img)
        {
            if(++first_image_cntr >= 2)
            {
                first_img_one = false;
                first_img = false;


                memcpy(newdata1,image_32.bits(),480000);
                first_image1->imageData = (char*) newdata1;//creating pointers
                first_image2->imageData = (char*) newdata1;
                first_image3->imageData = (char*) newdata1;
                cvSetImageROI(first_image1,rect1);
                cvSetImageROI(first_image2,rect2);
                cvSetImageROI(first_image3,rect3);



                src_base1 = cvarrToMat(first_image1);

                src_base2 = cvarrToMat(first_image2);

                src_base3 = cvarrToMat(first_image3);

            }
        }
        else
        {
            first_image_cntr = 0;

            src_test1 = cvarrToMat(image1);

            src_test2 = cvarrToMat(image2);
            src_test3 = cvarrToMat(image3);



            /***********************1)normalize algorithm NORM_L1**********************/
           // qDebug() << "in normalize algorithm NORM_L1";
            res1 = cv::norm(src_base1,src_test1,cv::NORM_L1);
            res2 = cv::norm(src_base2,src_test2,cv::NORM_L1);
            res3 = cv::norm(src_base3,src_test3,cv::NORM_L1);




            emit sendIntensity( res1,res2,res3);



        }
        // release all images
        cvReleaseImageHeader(&image1);
        //qDebug() << "image1 freed";
        cvReleaseImageHeader(&image2);
        //qDebug() << "image2 freed";
        cvReleaseImageHeader(&image3);

        //qDebug() << "image3 freed";
        free(newdata);

        emit sendImage(image_32); // this used to be image123.
    }
    else
    {
        emit sendImage(image123);
    }

}



void camera::stop()
{
    bool cam_flag = false;
    cvReleaseImageHeader(&first_image1);
    cvReleaseImageHeader(&first_image2);
    cvReleaseImageHeader(&first_image3);
 //   cvReleaseImageHeader(&first_image12);
    free(newdata1);
    if(cam_flag == true)
        close(fd1);
}

void camera::update_ROI(CvRect r1, CvRect r2, CvRect r3)
{
    rect1 = r1;
    rect2 = r2;
    rect3 = r3;
}

void camera::camera_on_off(bool state)
{
    stoploop = state;
}

void camera::hist_cmp_on_off_c(bool state)
{
    hist_cmp = state;
}

void camera::send_image_area_para(int para1,int para2)
{
    receive_area_x=para1;
    receive_area_y=para2;
}
