/*****************************************
 * Class:: CSC-615-01 Spring 2024
 *
 * Group Member Names:: Tyler Hsieh, Anthony Silva, Pedro Grande, Rafael Sant Ana Leitao
 * SFSU IDs: 920216320, 922907645, 921149265, 922965105
 * Github Usernames: Tyler9648, anthonySilva23, PFGrande, Rafael-Leitao
 * 
 * Project:: Autonomous Line-Following Car
 *
 * File:: motorTest.c
 *
 * Description:: This file serves as a test suite for the motor control functionality
 *               of the autonomous car project. It initializes the GPIO and motor library,
 *               sets velocities for the motors, and then terminates the motor operations
 *               gracefully.
 ******************************************/

#include "motor.h"
#include "motorLibrary.h"
#include <time.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pigpio.h>
#include <math.h>
volatile sig_atomic_t exitThread = 0;
int main()
{

    if (gpioInitialise() < 0)
    {
        printf("gpio initialization failed, exiting\n");
        exit(0);
    }
    else
    {
        printf("gpio initialization succeeded!\n");
    }

    if (Motor_Init() < 0)
    {
        printf("Failed to initialize servo motor.\n");
        return 1;
    }

    Motor_setVelocity(MOTORA, 50);
    Motor_setVelocity(MOTORB, 50);
    usleep(300000);
    Motor_Terminate();
    gpioTerminate();
    return 0;
}