#include "gpio.h"

GPIO::GPIO(int pin)
{       _pinNumber = pin;
        _valueString = QString("/sys/class/gpio/gpio%1/value").arg(pin);
        _directionString =
QString("/sys/class/gpio/gpio%1/direction").arg(pin);
        _strPin = QString("%1").arg(pin);
}

GPIO::~GPIO()
{
FILE * fp;
//This will create the folder /sys/class/gpio/gpio37
                if ((fp = fopen("/sys/class/gpio/unexport", "ab")) == NULL)
return;
                rewind(fp);             //Set pointer to begining of thefile
                fwrite(_strPin.toLatin1(), sizeof(char),_strPin.length() ,fp);
                fclose(fp);
}

int GPIO::openPin()
{
FILE * fp;
//This will create the folder /sys/class/gpio/gpio37
        if ((fp = fopen("/sys/class/gpio/export", "ab")) == NULL) return
-1;
        rewind(fp);//Set pointer to begining of the file
        fwrite(_strPin.toLatin1(), sizeof(char),_strPin.length() , fp);
        fclose(fp);
        return 0;
}

int GPIO::closePin()
{
FILE * fp;
//This will create the folder /sys/class/gpio/gpio37
                if ((fp = fopen("/sys/class/gpio/unexport", "ab")) == NULL)
return -1;
                rewind(fp);//Set pointer to begining of the file
                fwrite(_strPin.toLatin1(), sizeof(char),_strPin.length() ,
fp);
                fclose(fp);
                return 0;
}

int GPIO::setDirection(Direction direction)
{ //set direction in/out. returns 0 if ok and -1 on error
        FILE * fp;
        if ((fp = fopen(_directionString.toLatin1(), "rb+")) == NULL)
return -1;
        rewind(fp);//Set pointer to begining of the file
        if(direction == In)  fwrite("in", sizeof(char),2, fp);
        if(direction == Out) fwrite("out", sizeof(char),3, fp);
        fclose(fp);
        return 0;
}


int GPIO::setState(bool state)
{//state is 0 or 1. No effect if other value. returns the new state or -1 on error
    FILE * fp;
    if ((fp = fopen(_valueString .toLatin1(), "rb+")) == NULL) return -1;
    rewind(fp);//Set pointer to begining of the file
    if(state) fwrite("high", sizeof(char),4, fp);
    else fwrite("low", sizeof(char),3, fp);
    fclose(fp);
    return 0;
}

bool GPIO::getState()
{       //returns 1 or 0 - the pin state. Returns -1 on error
FILE * fp;
char value;
        if ((fp = fopen(_valueString.toLatin1(), "rb+")) == NULL) return
false;
        rewind(fp);//Set pointer to begining of the file
        fread(&value, sizeof(char),1, fp);
        fclose(fp);
        if(value=='1') return true;
        if(value=='0') return false;
        return false;
}
