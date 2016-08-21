#ifndef PROFILE_H
#define PROFILE_H

#include <QObject>

class Profile : public QObject
{
    Q_OBJECT
public:
    explicit Profile(QObject *parent = 0);
       ~Profile();
    void clear();
    QString Chemical_Name;
    int auto_mode,manual_mode;
    float Start_temp, Stop_temp, Ramp_rate, On_set, Single, Clear,single_sel;//11425

signals:
    
public slots:

private:


    
};

#endif // PROFILE_H
