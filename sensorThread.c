/*********************************************************************************************
 * Class:: CSC-615-01 Spring 2024
 *
 * Group Member Names:: Tyler Hsieh, Anthony Silva, Pedro Grande, Rafael Sant Ana Leitao
 * SFSU IDs: 920216320, 922907645, 921149265, 922965105
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

#define SOUND_DIST_MULT 170 // speed of sound / 2   (since we must wait for sound wave to travel and then bounce back)
#define SONAR_DIST_CAP 150  //sonar gets too innacurate with distances past 150 cm

extern volatile sig_atomic_t exitThread;

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

    case MULTI_LINE:  // cluster of 5 line sensors 
        gpioSetMode(sensorArgs->pin1, PI_INPUT);
        gpioSetMode(sensorArgs->pin2, PI_INPUT);
        gpioSetMode(sensorArgs->pin3, PI_INPUT);
        gpioSetMode(sensorArgs->pin4, PI_INPUT);
        gpioSetMode(sensorArgs->pin5, PI_INPUT);
        while (exitThread == 0)
        {
            sensorArgs->value1 = lineSensor(sensorArgs->pin1);
            sensorArgs->value2 = lineSensor(sensorArgs->pin2);
            sensorArgs->value3 = lineSensor(sensorArgs->pin3);
            sensorArgs->value4 = lineSensor(sensorArgs->pin4);
            sensorArgs->value5 = lineSensor(sensorArgs->pin5);
            
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
        while (exitThread == 0)                     //  finds the median distance of 5 sonar calls
        {                                           //  to reduce the chance of outliers
            double distArr[5];
            int i;
            for(i = 0; i < 5; i++){
                distArr[i] = sonarSensor(sensorArgs->pin, sensorArgs->trigger);
    
            }
            qsort(distArr, 5, sizeof(double), comp);
            double tempVal = distArr[2];
            
            sensorArgs->value = floor( (tempVal * 0.5) + (sensorArgs->value * 0.5) );   //  reduces effect of sharp changes/outliers 
                                                                                        //  from sonar readings 
            usleep(1000);   
        }
        break;
    default: // invalid sensor type
        printf("invalid sensor type in sensorIR_thread: arg -> sensorArgs.senType\n");
        break;
    }

    // exit thread now
    printf("\nfreeing allocated thread args in thread: %d\n", pthread_self());
    free(arg); // free arg that was malloced from main, move this to main later
    arg = NULL;
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
    { // wait up to 0.014 secs for echo input,                                      //
    }                 // once we get echo input,
    echoUp = clock(); // keep track of how when we first receive echo input
    start = gpioTick();
    while ((gpioRead(pin) != 0) && ((gpioTick() - start) < 14000))
    { // wait up to 0.014 secs for echoing to stop
    }
    echoDown = clock();

    float timeEchoedSecs; // keep track of when echo input stops

    timeEchoedSecs = (float)(echoDown - echoUp) / CLOCKS_PER_SEC; // calculate total uptime the echo input was up for
    distance = timeEchoedSecs * (float)SOUND_DIST_MULT * 100;     // calculate distance between sonar and object in cm
    double doubleDist = (double)distance;  
    
    if(doubleDist > SONAR_DIST_CAP){
        doubleDist = SONAR_DIST_CAP;
    } 
    else if(doubleDist < 1){         //call until we get a proper reading, anything less than 1 is invalid
        usleep(5000);
        return sonarSensor(pin, trigger);
    } 
    return doubleDist;
}

int timedGPIOHigh(int trigger, int duration)
{ // takes gpio trigger pin, and duration in microseconds to seend HIGH (1) signal for
    gpioWrite(trigger, PI_HIGH);
    usleep(duration);
    gpioWrite(trigger, PI_LOW);
    return 0;
}

int comp(const void *a, const void *b)
{
  const double *da = (const double *) a;
  const double *db = (const double *) b;

  return (*da > *db) - (*da < *db);
}