#include "profile.h"

Profile::Profile(QObject *parent) :
    QObject(parent)
{
    Chemical_Name.clear();

    Start_temp = 0;
    Stop_temp = 0;
    Ramp_rate = 0;
    On_set = 0;
    Single = 0;
    Clear = 0;
    single_sel=0;//11425
    auto_mode=0;
    manual_mode=0;
}

Profile::~Profile()
{

}

void Profile::clear()
{
    Chemical_Name.clear();

    Start_temp = 0;
    Stop_temp = 0;
    Ramp_rate = 0;
    On_set = 0;
    Single = 0;
    Clear = 0;
    single_sel=0;//11425
    auto_mode=0;
    manual_mode=0;

}


