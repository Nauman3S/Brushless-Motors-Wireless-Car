
#include <Servo.h>
Servo ESC1; // create servo object to control the ESC
Servo ESC2; // create servo object to control the ESC
Servo ESC3; // create servo object to control the ESC
Servo ESC4; // create servo object to control the ESC
int speed = 180;
int potValue; // value from the analog pin
void setupESC()
{
    // Attach the ESC on pins
    ESC1.attach(8, 1000, 2000);  // (pin, min pulse width, max pulse width in microseconds)
    ESC2.attach(9, 1000, 2000);  // (pin, min pulse width, max pulse width in microseconds)
    ESC3.attach(10, 1000, 2000); // (pin, min pulse width, max pulse width in microseconds)
    ESC4.attach(11, 1000, 2000); // (pin, min pulse width, max pulse width in microseconds)
}

void forward()
{
    ESC1.write(speed);
    ESC2.write(speed);
    ESC3.write(speed);
    ESC4.write(speed);
}

void left()
{

    ESC1.write(speed);
    ESC2.write(0);
    ESC3.write(speed);
    ESC4.write(0);
}

void right()
{

    ESC1.write(0);
    ESC2.write(speed);
    ESC3.write(0);
    ESC4.write(speed);
}

void stop()
{
    ESC1.write(0);
    ESC2.write(0);
    ESC3.write(0);
    ESC4.write(0);
}