/*******************************************************************************************************
 * Class:: CSC-615-01 Spring 2024
 *
 * Group Member Names:: Tyler Hsieh, Anthony Silva, Pedro Grande, Rafael Sant Ana Leitao
 * SFSU IDs: 920216320, 922907645, 921149265, 922965105
 * Github Usernames: Tyler9648, anthonySilva23, PFGrande, Rafael-Leitao
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
#include <pigpio.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include "sensorLib.h"
#include "servo.h"

extern tArg* multiLineSensorArgs;
extern tArg* lineSensorOneArgs;
extern tArg* lineSensorTwoArgs;
extern tArg* lineSensorThreeArgs;
extern tArg* lineSensorFourArgs;
extern tArg* lineSensorFiveArgs;
extern tArg *sonarSensorArgs;
extern tArg* avoidSensorArgs;
extern int motorASpeed;
extern int motorBSpeed;

int initSteering();
void terminateSteering();
void avoidObstacle();

int steer();
int getSum();

int obsDetected();

#endif // STEERING_H
