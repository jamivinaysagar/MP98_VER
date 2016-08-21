#ifndef LOGOFF_H
#define LOGOFF_H

#include <QDialog>

namespace Ui {
class logoff;
}

class logoff : public QDialog
{
    Q_OBJECT
    
public:
    explicit logoff(QWidget *parent = 0);
    ~logoff();

    
private:
    Ui::logoff *ui;
};

#endif // LOGOFF_H
