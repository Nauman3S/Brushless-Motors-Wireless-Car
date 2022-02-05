#include "escHandler.h"
#include "mqttHandler.h"

void setup()
{

    Serial.begin(115200);
    setupESC();
    setupGPRS();
}

void loop()
{

    //joystick 1 gives angle in degrees
    if (getJoystick1()==90.00)
    {
        forward();
    }
    else if (getJoystick1() == 180.00)
    {
        left();
    }
    else if (getJoystick1() == 0.00)
    {
        right();
    }
    else if (getJoystick1() == 270.00)
    {
        stop();
    }

    //joystick 2 gives angle in degrees
    if (getJoystick2() ==90.00)
    {
    }

    else if (getJoystick2() == 180.00)
    {
    }
    else if (getJoystick2() == 0.00)
    {
    }
    else if (getJoystick2() == 270.00)
    {
    }

    loopGPRS();
}