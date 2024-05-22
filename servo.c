/************************************************************************************************
 * Class:: CSC-615-01 Spring 2024
 *
 * Group Member Names:: Tyler Hsieh, Anthony Silva, Pedro Grande, Rafael Sant Ana Leitao
 * SFSU IDs: 920216320, 922907645, 921149265, 922965105
 * Github Usernames: Tyler9648, anthonySilva23, PFGrande, Rafael-Leitao
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
