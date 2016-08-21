#ifndef MOVIE_DISP_H
#define MOVIE_DISP_H

#include <QDialog>
#include <QObject>
#include <QImage>

namespace Ui {
class movie_disp;
}

class movie_disp : public QDialog
{
    Q_OBJECT
    
public:
    explicit movie_disp(QWidget *parent = 0);
    char pause_mov_disp;
    bool complete_flag;
    ~movie_disp();

signals:
    void send_play_pause_movie_display(char);
    void send_stop_movie_display(bool);

public slots:
    void update_mov_image(QImage,char);
    void receive_movie_completed();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();



private:
    Ui::movie_disp *ui;
};

#endif // MOVIE_DISP_H
