/**************************************************************************************************
 * Class:: CSC-615-01 Spring 2024
 *
 * Group Member Names:: Tyler Hsieh, Anthony Silva, Pedro Grande, Rafael Sant Ana Leitao
 * SFSU IDs: 920216320, 922907645, 921149265, 922965105
 * Github Usernames: Tyler9648, anthonySilva23, PFGrande, Rafael-Leitao
 * 
 * File:: sensorLib.h
 *
 * Description:: This header file defines the interfaces for the sensor library used in the
 *               autonomous car project. It includes declarations for initializing and terminating
 *               sensor operations within the car's control system.
 **************************************************************************************************/

#ifndef SENSOR_LIB
#define SENSOR_LIB
#include "sensorThread.h"

void sensorLibInit(void);
void sensorLibTerminate(void);

#endif