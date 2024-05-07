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
extern tArg* sonarSensorArgs;
extern int motorASpeed;
extern int motorBSpeed;

enum direction {
    FORWARD, BACKWARD, LEFT, RIGHT
};

// PLACEHOLDER WEIGHTS FOR SENSOR
enum lineWeight {
    LEFT_SENS = -2, 
    LEFT_MID_SENS = -1,
    MID_SENS = 0,
    RIGHT_MID_SENS = 1, 
    RIGHT_SENS = 2
};

// struct to hold information about sensor to calculate error
struct sensorOnLine {
    double sensorUpdateTime;
    double distanceFromMiddle;
};

int initSteering();
void terminateSteering();
int steer();
int getError(struct sensorOnLine * sensorInfo);

int steerTest();
int getSum();


#endif // STEERING_H
