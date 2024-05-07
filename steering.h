/*******************************************************************************************************
 * Class:: CSC-615-01 Spring 2024
 *
 * Project:: Autonomous Line-Following Car
 *
 * File:: steering.h
 *
 * Description:: This header file defines the interface for steering control in the autonomous
 *               car project. It includes declarations for functions to initialize and terminate
 *               steering, adjust steering based on sensor input, and perform test maneuvers. The file
 *               also defines several enumerations and structures to manage steering directions and
 *               sensor-based steering corrections.
 ******************************************************************************************************/

#ifndef STEERING_H
#define STEERING_H
#include "motor.h"
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include "sensorLib.h" // temporarily here, not sure if should be here

extern tArg* lineSensorOneArgs;
extern tArg* lineSensorTwoArgs;
extern tArg* lineSensorThreeArgs;
extern tArg* lineSensorFourArgs;
extern tArg* lineSensorFiveArgs;
extern int motorASpeed;
extern int motorBSpeed;

enum direction
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// PLACEHOLDER WEIGHTS FOR SENSOR
enum lineWeight
{
    LEFT_SENS = -2,
    LEFT_MID_SENS = -1,
    MID_SENS = 0,
    RIGHT_MID_SENS = 1,
    RIGHT_SENS = 2
};

// struct to hold information about sensor to calculate error
struct sensorOnLine
{
    double sensorUpdateTime;
    double distanceFromMiddle;
};

int initSteering();
void terminateSteering();
int steer();
int getError(struct sensorOnLine *sensorInfo);

int steerTest();
int getSum();

#endif // STEERING_H
