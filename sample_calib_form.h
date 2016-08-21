#ifndef SAMPLE_CALIB_FORM_H
#define SAMPLE_CALIB_FORM_H

#include <QDialog>

namespace Ui {
class sample_calib_form;
}

class sample_calib_form : public QDialog
{
    Q_OBJECT
    
public:
     QString range1,range2,range3,range_1,range_2,range_3,range_4;
     bool factory,single;
     double range[4];
    explicit sample_calib_form(QWidget *parent = 0);

    ~sample_calib_form();


signals:
    void send_from_sample_form(bool,int);//11425
    
public slots:
    void receive_sample_profile(float,float,float,int,bool,int);//11425
    bool sample_profile_check();
    void standard1_show();
    void standard2_show();
    void standard3_show();
    void standard4_show();
    void standard1_hide();
    void standard2_hide();
    void standard3_hide();
    void standard4_hide();
    QString res_adding1(float receive_temp);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::sample_calib_form *ui;
};

#endif // SAMPLE_CALIB_FORM_H
