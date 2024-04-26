#ifndef STEERING_H
#define STEERING_H
#include "motor.h"
#include <time.h>
#include "sensorLib.h" // temporarily here, not sure if should be here

extern tArg* lineSensorOneArgs;
extern tArg* lineSensorTwoArgs;
extern tArg* lineSensorThreeArgs;
extern tArg* lineSensorFourArgs;
extern tArg* lineSensorFiveArgs;

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


#endif // STEERING_H
