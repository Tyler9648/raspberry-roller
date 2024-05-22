/***********************************************************************************
 * Class:: CSC-615-01 Spring 2024
 *
 * Group Member Names:: Tyler Hsieh, Anthony Silva, Pedro Grande, Rafael Sant Ana Leitao
 * SFSU IDs: 920216320, 922907645, 921149265, 922965105
 * 
 * Project:: Autonomous Line-Following Car
 *
 * File:: main.c
 *
 * Description:: This file is the entry point for the autonomous car project.
 *               It initializes the GPIO pins and other subsystems such as
 *               sensors and steering. The main loop continuously checks sensor
 *               inputs to adjust the car's steering and speed.
 *               It handles SIGINT (Ctrl+C) signal to gracefully shut down the
 *               hardware interfaces and threads before exiting. Global variables
 *               and mutexes are used to handle thread-safe communication between
 *               sensor readings and steering control. This program provides
 *               real-time monitoring and response based on sensor feedback.
 *************************************************************************************/

#include <signal.h>
#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "sensorLib.h" // already includes sensorThread.h
#include "steering.h"  // includes motor.h
//#include "globals.h"   // global variables
#include "servo.h"
#define UPDATE_FREQ 50000
#define BUTTON_GPIO 27

/* GLOBAL VARIABLE INITIALIZATION */
volatile sig_atomic_t exitThread = 0;
tArg *lineSensorArgs;
tArg *lineSensorOneArgs;
tArg *lineSensorTwoArgs;
tArg *lineSensorThreeArgs;
tArg *lineSensorFourArgs;
tArg *lineSensorFiveArgs;
tArg *avoidSensorArgs;
tArg *testArgs;
tArg *sonarSensorArgs;
tArg *multiLineSensorArgs;

void waitForButton(int pin){          //waits until button is pressed and released

    gpioSetMode(pin, PI_INPUT);
    usleep(1000);
    while(gpioRead(pin) == PI_LOW && exitThread == 0 ){
        printf("waiting for button press\n");
    }           //waiting for button push down
    
    while(gpioRead(pin) == PI_HIGH && exitThread == 0 ){
        printf("waiting for button release\n");
    }           //waiting for button to be released

    
    if (exitThread == 0)
      printf("\nbutton pressed!\n\n");
}

void progExit(int sig)
{ // catch for ctr+c to exit so we can properly
    printf("caught interrupt signal\n");
    exitThread = 1;
}
int main(int argc, char *argv[])
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
    sensorLibInit();
    initSteering();
    Servo_Init();
    signal(SIGINT, progExit);


    waitForButton(BUTTON_GPIO);
    usleep(2500000);    
    while (exitThread == 0)
    {
        
        printf("%d %d %d %d %d\n", multiLineSensorArgs->value5, multiLineSensorArgs->value4, multiLineSensorArgs->value3, multiLineSensorArgs->value2, multiLineSensorArgs->value1);
        
        steer();

        double avoidTimeSinceUpdate = (clock() / CLOCKS_PER_SEC) - avoidSensorArgs->lastSensorUpdateTime;
        printf("\nAvoid sensor last changed %f seconds ago\n", avoidTimeSinceUpdate);
        if (avoidSensorArgs->value == 0)
        {
            printf("Avoid sensor: obstacle detected\n");
        }
        else if (avoidSensorArgs->value == 1)
        {
            printf("Avoid sensor: no obstacles detected\n");
        }
        if (sonarSensorArgs->value >= 0)
        {
            printf("Sonar sensor: distance is %d\n", sonarSensorArgs->value);
        }
        /*
        if(testArgs->value >= 0) {
            printf("test value displaying: %d\n", testArgs->value);
        }*/
        usleep(UPDATE_FREQ); //old 100000
    }
    printf("now terminating\n");
    terminateSteering();
    sensorLibTerminate();
    gpioTerminate();
    return 0;
}