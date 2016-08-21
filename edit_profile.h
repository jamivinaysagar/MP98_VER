#ifndef EDIT_PROFILE_H
#define EDIT_PROFILE_H

#include <QDialog>
#include "profile.h"
namespace Ui {
class edit_profile;
}

class edit_profile : public QDialog
{
    Q_OBJECT
    
public:
    explicit edit_profile(QWidget *parent = 0);
    ~edit_profile();
    int onset_clear_sel;//11425
    int automatic,manual;
private:
    Ui::edit_profile *ui;
    Profile* temp_profile;

  public  slots:
    void update_form(Profile*);


signals:
    void form_Data(Profile*);
private slots:
    //void on_buttonBox_clicked(QAbstractButton *button);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_onset_clear_button_clicked();

    void on_automode_clicked();
    void on_manualmode_clicked();
};

#endif // EDIT_PROFILE_H
