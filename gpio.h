#ifndef GPIO_H
#define GPIO_H

#include <qstring.h>
#include <qobject.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//Header pin number definitions
#define PIN9    160
#define PIN10   161
#define PIN11   162
#define PIN12   163
#define PIN13   164
#define PIN14   165
#define PIN15   166
#define PIN16   192
#define PIN17   193
#define PIN18   195
#define PIN19   197
#define PIN20   198
#define PIN21   199
#define PIN22   201
#define PIN23   202
#define PIN24   203
#define PIN25   139
#define PIN26   140
#define PIN27   141
#define PIN28   194
#define PIN29   142
#define PIN30   143
#define PIN31   32
#define PIN32   33
#define PIN33   233
#define PIN34   234

class GPIO : public QObject
{
    Q_OBJECT
public:
    GPIO(int pin); //pin is the pin nuber on con4 of the FriendlyARM board
    ~GPIO();

    enum Direction {In,Out,Err};
    int openPin();
    int closePin();
    int setDirection(Direction direction); //set direction in/out. returns 0 if ok and -1 on error
    int getDirection(); //returns direction
    int setState(bool state);
    void setValue(bool value);
    bool getState();
public:


private:
        Direction  _direction;
        int _state;
        int _pinNumber;
        QString _directionString;
        QString _valueString;
        QString _strPin;
};
#endif // GPIO_H
