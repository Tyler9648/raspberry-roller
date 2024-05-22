/************************************************************************************************
 * Class:: CSC-615-01 Spring 2024
 *
 * Group Member Names:: Tyler Hsieh, Anthony Silva, Pedro Grande, Rafael Sant Ana Leitao
 * SFSU IDs: 920216320, 922907645, 921149265, 922965105
 * 
 * Project:: Autonomous Line-Following Car
 *
 * File:: servoTest.c
 *
 * Description:: This file is a test suite for the servo control functionality
 *               in the autonomous car project. It includes tests to initialize the servo motor
 *               and move it to various predefined positions: right, left, and forward.
 ************************************************************************************************/

#include "servo.h"
#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>

#define GPIO_SERVO 12

int main()
{
    if (Servo_Init() < 0)
    {
        printf("Failed to initialize servo motor.\n");
        return 1;
    }
    usleep(2000000);
    printf("Pan right...\n");
    Pan_Right();
    usleep(2000000);
    printf("Pan left...\n");
    Pan_Left();
    usleep(2000000);
    printf("Pan forward...\n");
    Pan_Forward();
    usleep(3000000);
    gpioTerminate(); // Clean up GPIO
    return 0;
}