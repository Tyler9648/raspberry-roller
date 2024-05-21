/**************************************************************************************************
 * Class:: CSC-615-01 Spring 2024
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