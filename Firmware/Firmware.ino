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

    //joystick 1
    if (getJoystick1() == String("up"))
    {
        forward();
    }
    else if (getJoystick1() == String("left"))
    {
        left();
    }
    else if (getJoystick1() == String("right"))
    {
        right();
    }
    else if (getJoystick1() == String("down"))
    {
        stop();
    }

    //joystick 2
    if (getJoystick2() == String("up"))
    {
    }

    else if (getJoystick2() == String("left"))
    {
    }
    else if (getJoystick2() == String("right"))
    {
    }
    else if (getJoystick2() == String("down"))
    {
    }

    loopGPRS();
}