/***********************************************************************************
 * Class:: CSC-615-01 Spring 2024
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
#include "globals.h"   // global variables

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
extern pthread_mutex_t exitLock;

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
    signal(SIGINT, progExit);

    while (exitThread == 0)
    {
        pthread_mutex_lock(&exitLock);
        // steer();
        // Motor_setVelocity(MOTORB, -50);
        printf("%d %d %d %d %d\n", lineSensorOneArgs->value, lineSensorTwoArgs->value, lineSensorThreeArgs->value, lineSensorFourArgs->value, lineSensorFiveArgs->value);
        steerTest();
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
        pthread_mutex_unlock(&exitLock);
        usleep(100000);
    }
    printf("now terminating\n");
    terminateSteering();
    sensorLibTerminate();
    gpioTerminate();
    return 0;
}