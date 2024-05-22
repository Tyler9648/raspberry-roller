/**************************************************************************************************
 * Class:: CSC-615-01 Spring 2024
 *
 * Group Member Names:: Tyler Hsieh, Anthony Silva, Pedro Grande, Rafael Sant Ana Leitao
 * SFSU IDs: 920216320, 922907645, 921149265, 922965105
 * 
 * File:: sensorLib.c
 *
 * Description:: This file implements the sensor library for the autonomous car project,
 *               providing functions to initialize, manage, and terminate sensor threads.
 *               It supports multiple sensor types, including line, avoidance, and sonar sensors.
 *               The library handles thread safety using mutexes to ensure that sensor data
 *               is accessed safely across different parts of the program.
 **************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pigpio.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "sensorLib.h" //uncomment for car implementation
// #include "sensorThread.h"

#define LINESENSOR_GPIO 19 // sensor gpio pins

#define LINESENSOR_ONE_GPIO 22
#define LINESENSOR_TWO_GPIO 23
#define LINESENSOR_THREE_GPIO 24
#define LINESENSOR_FOUR_GPIO 25
#define LINESENSOR_FIVE_GPIO 26

#define AVOIDSENSOR_GPIO 15 // add EVERY GPIO pin used by sensors here
#define SONARSENSOR_GPIO 13
#define TRIGGER_GPIO 14

#define ENABLE_LINESENSOR 0     // add ENABLE_SENSOR for each sensor or sensor cluster
#define ENABLE_LINESENSOR_ONE 0 // set to 1 to enable, 0 to disable
#define ENABLE_LINESENSOR_TWO 0
#define ENABLE_LINESENSOR_THREE 0
#define ENABLE_LINESENSOR_FOUR 0
#define ENABLE_LINESENSOR_FIVE 0
#define ENABLE_AVOIDSENSOR 1
#define ENABLE_SONARSENSOR 1
#define ENABLE_TEST 0
#define ENABLE_MULTI_LINESENSOR 1

#define NUMTHREADS ( (5 * ENABLE_MULTI_LINESENSOR) + ENABLE_LINESENSOR + ENABLE_AVOIDSENSOR + ENABLE_TEST + ENABLE_SONARSENSOR + ENABLE_LINESENSOR_ONE + ENABLE_LINESENSOR_TWO + ENABLE_LINESENSOR_THREE + ENABLE_LINESENSOR_FOUR + ENABLE_LINESENSOR_FIVE) // one for each sensor
                                                                                                                                                                                                                           // can be changed if more sensors/components are added

extern volatile sig_atomic_t exitThread; // 0 by default, move to main in car implementation
                                         // 1 to exit all threads and main
extern tArg *lineSensorArgs;
extern tArg *lineSensorOneArgs;
extern tArg *lineSensorTwoArgs;
extern tArg *lineSensorThreeArgs;
extern tArg *lineSensorFourArgs;
extern tArg *lineSensorFiveArgs;
extern tArg *multiLineSensorArgs;
extern tArg *avoidSensorArgs;
extern tArg *testArgs;
extern tArg *sonarSensorArgs;

int activeThreads; // uncomment if implemented with car

pthread_t threadID[NUMTHREADS]; // sensor threads

void sensorLibInit(void)
{ // handles creation of all sensor threads
  // NOT used when testing main in sensorLib, only for car's main
  // completely replaces main in sensorLib in car implementation

    printf("# of sensors: %d\n", NUMTHREADS);

    // thread args are globals, declared in this sensorLib.c file

    activeThreads = 0;

    // initialize and launch line sensor thread
    if (ENABLE_LINESENSOR == 1)
    {
        lineSensorArgs = (tArg *)malloc(sizeof(tArg));
        lineSensorArgs->senType = LINE;
        lineSensorArgs->pin = LINESENSOR_GPIO;
        lineSensorArgs->value = -1;
        pthread_create(&threadID[activeThreads], NULL, &sensor_thread, (void *)lineSensorArgs);
        activeThreads++;
    }
    if (ENABLE_LINESENSOR_ONE == 1)
    {
        lineSensorOneArgs = (tArg *)malloc(sizeof(tArg));
        lineSensorOneArgs->senType = LINE;
        lineSensorOneArgs->pin = LINESENSOR_ONE_GPIO;
        lineSensorOneArgs->value = -1;
        lineSensorOneArgs->lastSensorUpdateTime = 0;
        pthread_create(&threadID[activeThreads], NULL, &sensor_thread, (void *)lineSensorOneArgs);
        activeThreads++;
    }
    if (ENABLE_LINESENSOR_TWO == 1)
    {
        lineSensorTwoArgs = (tArg *)malloc(sizeof(tArg));
        lineSensorTwoArgs->senType = LINE;
        lineSensorTwoArgs->pin = LINESENSOR_TWO_GPIO;
        lineSensorTwoArgs->value = -1;
        lineSensorTwoArgs->lastSensorUpdateTime = 0;
        pthread_create(&threadID[activeThreads], NULL, &sensor_thread, (void *)lineSensorTwoArgs);
        activeThreads++;
    }
    if (ENABLE_LINESENSOR_THREE == 1)
    {
        lineSensorThreeArgs = (tArg *)malloc(sizeof(tArg));
        lineSensorThreeArgs->senType = LINE;
        lineSensorThreeArgs->pin = LINESENSOR_THREE_GPIO;
        lineSensorThreeArgs->value = -1;
        lineSensorThreeArgs->lastSensorUpdateTime = 0;
        pthread_create(&threadID[activeThreads], NULL, &sensor_thread, (void *)lineSensorThreeArgs);
        activeThreads++;
    }
    if (ENABLE_LINESENSOR_FOUR == 1)
    {
        lineSensorFourArgs = (tArg *)malloc(sizeof(tArg));
        lineSensorFourArgs->senType = LINE;
        lineSensorFourArgs->pin = LINESENSOR_FOUR_GPIO;
        lineSensorFourArgs->value = -1;
        lineSensorFourArgs->lastSensorUpdateTime = 0;
        pthread_create(&threadID[activeThreads], NULL, &sensor_thread, (void *)lineSensorFourArgs);
        activeThreads++;
    }
    if (ENABLE_LINESENSOR_FIVE == 1)
    {
        lineSensorFiveArgs = (tArg *)malloc(sizeof(tArg));
        lineSensorFiveArgs->senType = LINE;
        lineSensorFiveArgs->pin = LINESENSOR_FIVE_GPIO;
        lineSensorFiveArgs->value = -1;
        lineSensorFiveArgs->lastSensorUpdateTime = 0;
        pthread_create(&threadID[activeThreads], NULL, &sensor_thread, (void *)lineSensorFiveArgs);
        activeThreads++;
    }
    if (ENABLE_MULTI_LINESENSOR == 1)
    {
        multiLineSensorArgs = (tArg *)malloc(sizeof(tArg));
        multiLineSensorArgs->senType = MULTI_LINE;
        multiLineSensorArgs->pin1 = LINESENSOR_ONE_GPIO;
        multiLineSensorArgs->pin2 = LINESENSOR_TWO_GPIO;
        multiLineSensorArgs->pin3 = LINESENSOR_THREE_GPIO;
        multiLineSensorArgs->pin4 = LINESENSOR_FOUR_GPIO;
        multiLineSensorArgs->pin5 = LINESENSOR_FIVE_GPIO;
        multiLineSensorArgs->value1 = -1;
        multiLineSensorArgs->value2 = -1;
        multiLineSensorArgs->value3 = -1;
        multiLineSensorArgs->value4 = -1;
        multiLineSensorArgs->value5 = -1;
        multiLineSensorArgs->lastSensorUpdateTime = 0;
        pthread_create(&threadID[activeThreads], NULL, &sensor_thread, (void *)multiLineSensorArgs);
        activeThreads++;
    }
 
    // prepare and launch avoidance sensor thread
    if (ENABLE_AVOIDSENSOR == 1)
    {
        avoidSensorArgs = (tArg *)malloc(sizeof(tArg));
        avoidSensorArgs->senType = AVOID;
        avoidSensorArgs->pin = AVOIDSENSOR_GPIO;
        avoidSensorArgs->value = -1;
        avoidSensorArgs->lastSensorUpdateTime = 0;
        pthread_create(&threadID[activeThreads], NULL, &sensor_thread, (void *)avoidSensorArgs);
        activeThreads++;
    }
    // prepare and launch sonar sensor thread
    if (ENABLE_SONARSENSOR == 1)
    {
        sonarSensorArgs = (tArg *)malloc(sizeof(tArg));
        sonarSensorArgs->senType = SONAR;
        sonarSensorArgs->pin = SONARSENSOR_GPIO;
        sonarSensorArgs->trigger = TRIGGER_GPIO;
        sonarSensorArgs->value = -1;
        sonarSensorArgs->lastSensorUpdateTime = 0;
        pthread_create(&threadID[activeThreads], NULL, &sensor_thread, (void *)sonarSensorArgs);
        activeThreads++;
    }
    // prepare and launch test sensor thread w/o pins
    if (ENABLE_TEST == 1)
    {
        testArgs = (tArg *)malloc(sizeof(tArg));
        testArgs->senType = TEST;
        testArgs->pin = -1; // no pin being used
        testArgs->value = -1;
        pthread_create(&threadID[activeThreads], NULL, &sensor_thread, (void *)testArgs);
        activeThreads++;
    }
    // feel free to add more, if more sensors are added
    // must CREATE SEPERATE THREAD FOR EVERY SENSOR (unless theyre clustered)
    // sensor clusters must be added to sensorThread as if they're a completely new sensor
    // IMPORTANT -- make sure main loop in car's main that reads sensor vals is protected w exitLock
}

void sensorLibTerminate(void)
{ // call after motors are terminated
  // ONLY used for car implementation
    // close threads to prepare for clean up
    printf("\nClosing threads, %d threads currently running\n", activeThreads);
    int i;
    for (i = 0; i < NUMTHREADS; i++)
    {
        if (threadID[i])
        {
            pthread_join(threadID[i], NULL);
            activeThreads--;
            printf("\nThread closed, %d still running\n", activeThreads);
        }
    }
    printf("\nAll threads closed, now cleaning up and exiting\n");

}