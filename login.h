#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>


namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = 0);

    ~login();
    QString temp_USER[11],temp_PWD[11];//11425
/*private slots://11425
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();*/

signals:
    void send_from_login(char,int);//11425
    //void close_login();//14547
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();


public slots:
    void receive_usr_pwd(QString,QString);//11425

private:
    Ui::login *ui;

};

#endif // LOGIN_H
