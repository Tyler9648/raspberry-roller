/************************************************************************************************
 * Class:: CSC-615-01 Spring 2024
 *
 * Project:: Autonomous Line-Following Car
 *
 * File:: servo.c
 *
 * Description:: This source file contains the implementation of servo motor control functions
 *               for the autonomous car project. It includes initialization of the pigpio library
 *               and servo motors, along with functions to pan the servo motor in different
 *               directions (right, left, forward).
 ************************************************************************************************/
#include "servo.h"
#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>

int Servo_Init(void)
{
    // Initialize the pigpio library
    // if (gpioInitialise() < 0)
    // {
    //     printf("Failed to initialize pigpio library\n");
    //     return 1;
    // }
    gpioSetMode(SERVO_PIN, PI_OUTPUT);
    gpioSetPWMfrequency(SERVO_PIN, PWD_FREQ);
    Pan_Forward();
    return 0;
}

void Pan_Right()
{
    gpioServo(SERVO_PIN, SERVO_RIGHT);
}

void Pan_Left()
{
    gpioServo(SERVO_PIN, SERVO_LEFT);
}

void Pan_Forward()
{
    gpioServo(SERVO_PIN, SERVO_FORWARD);
}
