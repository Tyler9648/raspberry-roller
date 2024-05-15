/*********************************************************************************************
 * Class:: CSC-615-01 Spring 2024
 *
 * File:: sensorThread.c
 *
 * Description:: This file contains the implementation of the sensor threads for the autonomous
 *               car project, specifically tailored for a Raspberry Pi. Each thread continuously
 *               monitors a specific type of sensor (line, avoidance, sonar) and updates its
 *               value based on real-time readings.
 ***********************************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <pigpio.h>
#include <math.h>
#include "sensorThread.h"

#define SOUND_DIST_MULT 170

extern volatile sig_atomic_t exitThread;
extern pthread_mutex_t exitLock;

void *sensor_thread(void *arg)
{                                   // thread function that constantly reads a
    tArg *sensorArgs = (tArg *)arg; // sensor's value

    /*
     * as of right now all sensor switch cases are the exact same, but having cases
     * makes it easier to debug or make changes to a specific sensor. also makes it
     * very easy to add more sensors
     * aim of having switches and seperate functions is to make it easier to implement
     * in upcoming car project
     */

    switch (sensorArgs->senType)
    {
    case LINE: // line sensor
        gpioSetMode(sensorArgs->pin, PI_INPUT);
        while (exitThread == 0)
        {
            int oldVal = 0 + sensorArgs->value;
            sensorArgs->value = lineSensor(sensorArgs->pin);
            if (oldVal != sensorArgs->value)
            {
                sensorArgs->lastSensorUpdateTime = clock() / CLOCKS_PER_SEC;
            }
        }
        break;
    case AVOID: // avoidance sensor
        gpioSetMode(sensorArgs->pin, PI_INPUT);
        while (exitThread == 0)
        {
            int oldVal = 0 + sensorArgs->value;
            sensorArgs->value = avoidSensor(sensorArgs->pin);
            if (oldVal != sensorArgs->value)
            {
                sensorArgs->lastSensorUpdateTime = clock() / CLOCKS_PER_SEC;
            }
        }
        break;
    case TEST: // for sensor free thread testing and debugging
        while (exitThread == 0)
        {
            sensorArgs->value = 2;
        }
        break;
    case SONAR:                                      // sonar sensor
        gpioSetMode(sensorArgs->trigger, PI_OUTPUT); // initialize trigger
        gpioWrite(sensorArgs->trigger, PI_LOW);
        gpioSetMode(sensorArgs->pin, PI_INPUT); // initialize echo pin
        while (exitThread == 0)
        {
            double distArr[5];
            int i;
            for(i = 0; i < 5; i++){
                distArr[i] = sonarSensor(sensorArgs->pin, sensorArgs->trigger);
                usleep(2000);
            }
            qsort(distArr, 5, sizeof(double), comp);
            double tempVal = distArr[2];

            //double tempVal = sonarSensor(sensorArgs->pin, sensorArgs->trigger);
            
            //if ((int) tempVal > 0){
                sensorArgs->value = floor( (tempVal * 0.5) + (sensorArgs->value * 0.5) );
                //sensorArgs->value = tempVal;
            //}

            usleep(5000);   //OLD: 10000
        }
        break;
    default: // invalid sensor type
        printf("invalid sensor type in sensorIR_thread: arg -> sensorArgs.senType\n");
        break;
    }

    // exit thread now
    pthread_mutex_lock(&exitLock); // mutex lock to protect argument deallocation
    printf("\nfreeing allocated thread args in thread: %d\n", pthread_self());
    free(arg); // free arg that was malloced from main, move this to main later
    arg = NULL;
    pthread_mutex_unlock(&exitLock);
}

int lineSensor(int pin)
{ // reads 0 (LOW) when line is white, reads 1 (HIGH) when line is black
    int val = gpioRead(pin);
    return val;
}

int avoidSensor(int pin)
{ // reads 0 (LOW) when obstacle detected, reads 1 (HIGH) when no obstacle
    int val = gpioRead(pin);
    return val;
}

// sonar sensor accurately reads between 2 cm and 200 cm so keep that in mind
double sonarSensor(int pin, int trigger)
{                               // returns distance between sonar and closest object in cm
    timedGPIOHigh(trigger, 15); // start the sonar trigger
    uint32_t start = gpioTick();
    clock_t echoUp, echoDown;
    float distance;
    while ((gpioRead(pin) == 0) && ((gpioTick() - start) < 14000))
    { // wait up to 0.014 secs for echo input,

        // echoUp = clock();                                        //
    }                 // once we get echo input,
    echoUp = clock(); // keep track of how when we first receive echo input
    start = gpioTick();
    while ((gpioRead(pin) != 0) && ((gpioTick() - start) < 14000))
    { // wait up to 0.014 secs for echoing to stop

        // echoDown = clock();
    }
    echoDown = clock();
    // -- IMPORTANT -- : if sonar sensor freezes, its probably cos of this line
    //  so if it freezes then add error handler to catch it
    float timeEchoedSecs; // keep track of when echo input stops

    timeEchoedSecs = (float)(echoDown - echoUp) / CLOCKS_PER_SEC; // calculate total uptime the echo input was up for
    distance = timeEchoedSecs * (float)SOUND_DIST_MULT * 100;     // calculate distance between sonar and object in cm
    double doubleDist = (double)distance * 0.6;
    // printf("time echoed in secs %f \n", timeEchoedSecs);

    // printf("measured distance of %f centimeters\n", distance);
    // printf("measured distance of %d centimeters\n", doubleDist);
    if(doubleDist > 150){
        doubleDist = 150;
    } 
    else if(doubleDist < 1){
        //doubleDist = 200;
        usleep(5000);
        return sonarSensor(pin, trigger);
    } /*else if(doubleDist == 0){
        doubleDist = 1;
    }*/
    //printf("raw distance: %f cm\n", doubleDist);
    return doubleDist;
}

int timedGPIOHigh(int trigger, int duration)
{ // takes gpio trigger pin, and duration in microseconds to seend HIGH (1) signal for
    gpioWrite(trigger, 1);
    usleep(duration);
    gpioWrite(trigger, 0);
    return 0;
}

int comp(const void *a, const void *b)
{
  const double *da = (const double *) a;
  const double *db = (const double *) b;

  return (*da > *db) - (*da < *db);
}