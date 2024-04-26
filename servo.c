#include "servo.h"
#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>

int Servo_Init(void)
{
     // Initialize the pigpio library
    if (gpioInitialise() < 0)
    {
        printf("Failed to initialize pigpio library\n");
        return 1;
    }
        gpioSetMode(SERVO_PIN, PI_OUTPUT);
        gpioSetPWMfrequency(SERVO_PIN, PWD_FREQ);
        return 0;
}

void Pan_Right()
{
    gpioServo(SERVO_PIN, RIGHT);
}

void Pan_Left()
{
    gpioServo(SERVO_PIN, LEFT);
}

void Pan_Forward()
{
    gpioServo(SERVO_PIN, FORWARD);
}

