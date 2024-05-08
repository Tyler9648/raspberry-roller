/*******************************************************************************************************************
 * Class:: CSC-615-01 Spring 2024
 *
 * Project:: Autonomous Line-Following Car
 *
 * File:: steering.c
 *
 * Description:: This source file contains the implementation of the steering logic for the autonomous car project.
 *               It integrates sensor readings with motor control to dynamically adjust the car's steering based on
 *               its position relative to a track. Key functionalities include initializing and terminating steering
 *               mechanisms, executing basic steering commands (left, right, forward), and more complex maneuvers
 *               like error correction based on line detection and obstacle avoidance.
 ********************************************************************************************************************/
#include "steering.h"
extern volatile sig_atomic_t exitThread;

#define BASE_SPEED 70
#define AVOIDANCE_SPEED 60 // base speed when going around in obstacle

// I don't know what to name this variable, just stores if the
// car is going forwards or backwards
int behavior = FORWARD;
int leftSpeed = 0;
int rightSpeed = 0;
double timeMiddleOnLine; // time before middle sensor left the line
int prevSum = 0;
// should we have 2 different functions for correcting the error on the line
// and another for steering around an object?
// CURRENTLY IT ONLY STEERS RIGHT

int initSteering()
{

    return Motor_Init();
}

void terminateSteering()
{
    Motor_Terminate();
}

int steer()
{
    struct sensorOnLine *lineDetectedBy = malloc(sizeof(struct sensorOnLine));
    double timeOffLine = 0;

    getError(lineDetectedBy);
    // TEMPORARY VALUES
    int kp, kd, ki;
    kp = 2;
    kd = 2;
    ki = 2;

    // TEMPORARY : In a different function we will track when the
    // middle sensor gets on the line
    if (lineSensorThreeArgs->value == 1)
    {
        printf("middle on line\n");
        // go forward if middle sensor is on line
        timeMiddleOnLine = lineSensorThreeArgs->lastSensorUpdateTime;
        Motor_setVelocity(MOTORA, 50);
        Motor_setVelocity(MOTORB, 50);
        return 0;
    }

    // if middle sensor if off the line
    if (lineSensorThreeArgs->value == 0)
    {
        // computer dt
        timeOffLine = lineSensorThreeArgs->lastSensorUpdateTime - timeMiddleOnLine;
    }

    int steerValue = floor((kp * lineDetectedBy->distanceFromMiddle) + (kd * lineDetectedBy->distanceFromMiddle / timeOffLine));

    printf("STEER VALUE: %d", steerValue);

    bool tracingObstacle = false; // TEMPORARY, will be set by the sonar

    int totalError = 0;
    /*
     * create variables for motorA and motorB current speeds
     * create variables for tracingObstacle
     *
     */

    // continues to turn until reaching the line again?
    int sensorDist = lineDetectedBy->distanceFromMiddle;

    while (lineSensorThreeArgs->value != 1 && tracingObstacle == false && exitThread == 0)
    {
        Motor_setVelocity(MOTORA, motorASpeed - steerValue); // assume currSpeed is updated by setVelocity, and is between -100 to 100
        Motor_setVelocity(MOTORB, motorBSpeed + steerValue); // this doesn't require encoder cus im lazy, but if needed can be changed
        usleep(3000);                                        // adjustable, steering frequency, lower num = higher freq = smoother
        totalError += ki * sensorDist * 3000 / 1000000;      // adding up total error, with respect to time in seconds
        // time middle sensor has been off the line
        timeOffLine = timeOffLine - lineSensorThreeArgs->lastSensorUpdateTime;
        // timeOffLine = timeOffLine - lastSensorUpdateTime;
        steerValue = (kp * sensorDist) + (kd * sensorDist / timeOffLine) + totalError;
    }

    free(lineDetectedBy);
    return 0;
}

// use struct to indicate which sensor was read from

int getError(struct sensorOnLine *sensorInfo)
{
    // assume line sensors around around 2.5 centimeters apart:
    // 0.025 m (placeholder val), should we use different measurement unit?

    // start with outter sensors?
    const double distBetweenLineSensor = 0.025;

    if (lineSensorOneArgs->value == 1)
    {
        sensorInfo->distanceFromMiddle = LEFT_SENS * distBetweenLineSensor;
        sensorInfo->sensorUpdateTime = lineSensorOneArgs->lastSensorUpdateTime;
    }
    if (lineSensorFiveArgs->value == 1)
    {
        sensorInfo->distanceFromMiddle = RIGHT_SENS * distBetweenLineSensor;
        sensorInfo->sensorUpdateTime = lineSensorFiveArgs->lastSensorUpdateTime;
    }
    if (lineSensorTwoArgs->value == 1)
    {
        sensorInfo->distanceFromMiddle = LEFT_MID_SENS * distBetweenLineSensor;
        sensorInfo->sensorUpdateTime = lineSensorTwoArgs->lastSensorUpdateTime;
    }
    if (lineSensorFourArgs->value == 1)
    {
        sensorInfo->distanceFromMiddle = RIGHT_MID_SENS * distBetweenLineSensor;
        sensorInfo->sensorUpdateTime = lineSensorFourArgs->lastSensorUpdateTime;
    }
    if (lineSensorThreeArgs->value == 1)
    {
        sensorInfo->distanceFromMiddle = MID_SENS * distBetweenLineSensor;
        sensorInfo->sensorUpdateTime = lineSensorThreeArgs->lastSensorUpdateTime;
    }
}

int obsDetected(void){
    if (sonarSensorArgs->value <= 38 && sonarSensorArgs->value > 0){
        return 1;
    } else {
        return 0;
    }
   
}

int steerTest()
{
    // struct sensorOnLine * lineDetectedBy = malloc(sizeof(struct sensorOnLine));
    Motor_setVelocity(MOTORA, BASE_SPEED);
    Motor_setVelocity(MOTORB, BASE_SPEED);
    int sleepVal = 16000;

    // Obstacle detected
    //if (sonarSensorArgs->value <= 30 && sonarSensorArgs->value > 0)
    if (obsDetected() == 1)
    {
        printf("Obstacle detected: distance is %d\n", sonarSensorArgs->value);
        avoidObstacle();
        //testAvoidObstacle();
    }

    // for sharp turns, slow down the speed
    if (lineSensorThreeArgs->value == 1 && (lineSensorOneArgs->value == 1 || lineSensorFiveArgs->value == 1) && (lineSensorOneArgs->value != lineSensorFiveArgs->value))
    {
        Motor_setVelocity(MOTORA, BASE_SPEED / 2);
        Motor_setVelocity(MOTORB, BASE_SPEED / 2);
        sleepVal = 11000;
    }
    while (exitThread == 0 && getSum() != 0 && obsDetected() == 0)
    { // while its not an intersection or not dead center on the line
        int steerValue = getSum();
        Motor_setVelocity(MOTORA, motorASpeed + steerValue); // assume currSpeed is updated by setVelocity, and is between -100 to 100
        Motor_setVelocity(MOTORB, motorBSpeed - steerValue); // this doesn't require encoder cus im lazy, but if needed can be changed
        usleep(sleepVal);
    }
    /*
    if(lineSensorThreeArgs->value == 1){
        Motor_setVelocity(MOTORA, 25);
        Motor_setVelocity(MOTORB, 25);
    }*/
    return 0;
}

void avoidObstacle()
{

    // Stop
    Motor_setVelocity(MOTORA, 0);
    Motor_setVelocity(MOTORB, 0);
    usleep(1000000);
    Pan_Right();
    // Turn left
    Motor_setVelocity(MOTORA, -40);
    Motor_setVelocity(MOTORB, 40);
    usleep(1000000);

    Motor_setVelocity(MOTORA, 0);
    Motor_setVelocity(MOTORB, 0);
    usleep(1000000);

    Motor_setVelocity(MOTORA, AVOIDANCE_SPEED);
    Motor_setVelocity(MOTORB, AVOIDANCE_SPEED);
    // loop aroun in circle intil
    //  Move Forward
    while (lineSensorThreeArgs->value != 1 && exitThread == 0)
    {
        printf("sonar distance: %d\n", sonarSensorArgs->value);
        int motorAadjust = 1;
        int motorBadjust = 1;

        if (sonarSensorArgs->value < 28)
        {
            if((motorASpeed - 1) <= 40){
                motorAadjust = 0;
            }
            if((motorBSpeed + 1) >= 80){
                motorBadjust = 0;
            }
            Motor_setVelocity(MOTORA, motorASpeed - motorAadjust);
            Motor_setVelocity(MOTORB, motorBSpeed + motorBadjust);
        }
        else if (//(sonarSensorArgs->value > 15)&&
            (sonarSensorArgs->value < 35))
            {
                Motor_setVelocity(MOTORA, AVOIDANCE_SPEED);
                Motor_setVelocity(MOTORB, AVOIDANCE_SPEED);
            }
        else
        {
            if((motorASpeed + 1) >= 80){
                motorAadjust = 0;
            }
            if((motorBSpeed - 1) <= 40){
                motorBadjust = 0;
            }
            Motor_setVelocity(MOTORA, motorASpeed + motorAadjust);
            Motor_setVelocity(MOTORB, motorBSpeed - motorBadjust);
        }
        usleep(20000);
    }
    Pan_Forward();
    printf("\nwere back on the line\n");
    //     usleep(2000000);
    //    // Pan_Forward();
    //     Motor_setVelocity(MOTORA, 0);
    //     Motor_setVelocity(MOTORB, 0);
    //     usleep(1000000);
    //     // maybe if statement
    //     Motor_setVelocity(MOTORA, 40 * (1 + sonarSensorArgs->value);
    //     Motor_setVelocity(MOTORB, -40 * (1 + sonarSensorArgs->value);
    //     usleep(1000000);
    //     Motor_setVelocity(MOTORA, 50);
    //     Motor_setVelocity(MOTORB, 50);
    //     usleep(1000000);
}

void testAvoidObstacle()
{

    // Stop
    Motor_setVelocity(MOTORA, 0);
    Motor_setVelocity(MOTORB, 0);
    usleep(1000000);
    Pan_Right();
    // Turn left

    //Motor_setVelocity(MOTORA, -40);
    Motor_setVelocity(MOTORB, 40);
    //usleep(1000000);
    usleep(2000000);

    Motor_setVelocity(MOTORA, 0);
    Motor_setVelocity(MOTORB, 0);
    usleep(1000000);

    Motor_setVelocity(MOTORA, AVOIDANCE_SPEED);
    Motor_setVelocity(MOTORB, AVOIDANCE_SPEED);
    // loop aroun in circle intil
    //  Move Forward
    while (lineSensorThreeArgs->value != 1 && exitThread == 0)
    {
        printf("sonar distance: %d\n", sonarSensorArgs->value);
        if (sonarSensorArgs->value >= 20 && sonarSensorArgs->value <= 38){
            Motor_setVelocity(MOTORA, AVOIDANCE_SPEED);
            Motor_setVelocity(MOTORB, AVOIDANCE_SPEED);
        }
        else {
        double adjust = (sonarSensorArgs->value - 25) * pow( 1.05, abs(30 - sonarSensorArgs->value)/12 );
        printf("adjsut val: %f\n", adjust);
        if (adjust < 0){
            adjust *= 3;
        }
        Motor_setVelocity(MOTORA, motorASpeed + (int) (adjust * 0.02));
        Motor_setVelocity(MOTORB, motorBSpeed - (int) (adjust * 0.02));
        }
        usleep(150000);
    }
    Pan_Forward();                              //cat: w3e0p-2p
    printf("\nwere back on the line\n");
}

int getSum(/*struct sensorOnLine * sensorInfo*/)
{
    // assume line sensors around around 2.5 centimeters apart:
    // 0.025 m (placeholder val), should we use different measurement unit?

    // start with outter sensors?
    double sum = 0;
    int sensorsTriggered = 0;
    if (lineSensorOneArgs->value == 1)
    {
        sum -= 3;
        sensorsTriggered++;
    }
    if (lineSensorTwoArgs->value == 1)
    {
        sum -= 1;
        sensorsTriggered++;
    }
    if (lineSensorThreeArgs->value == 1)
    {
        sensorsTriggered++;
    }
    if (lineSensorFourArgs->value == 1)
    {
        sum += 1;
        sensorsTriggered++;
    }
    if (lineSensorFiveArgs->value == 1)
    {
        sum += 3;
        sensorsTriggered++;
    }

    if (sensorsTriggered == 0)
    {
        // printf("getSum -> prevSum: %d\n", prevSum);
        return prevSum;
    }

    int intSum;
    if (lineSensorThreeArgs->value == 1 && (lineSensorOneArgs->value == 1 || lineSensorFiveArgs->value == 1) && (lineSensorOneArgs->value != lineSensorFiveArgs->value))
    {
        intSum = (int)(sum / (sensorsTriggered - 1)); // this is really scuffed but it works
        intSum = (intSum / abs(intSum)) * 4;          // 4 is total sum of one side of sensors
    }
    else
    {
        intSum = (int)(sum / sensorsTriggered); // it is totally intentional to have it 0.5 be rounded down to 0
    }                                           // when converting double to int, for averaging sum of 1 between
                                                // a middle and inner line sensor
    if (intSum != 0)
    {
        prevSum = intSum;
    }
    // printf("getSum -> intSum: %d\n", intSum);
    return intSum;
}

// int followLine() { // used to test steering
//     Motor_Init();

//     // add code that corrects for the error once we get info from sensors

// }

// // MOTORA = LEFT MOTOR
// // MOTORB = RIGHT MOTOR
// // followLine calls this function and determines when the steering should stop
// int steerDirection(int direction) {
//     switch (direction) {
//     case LEFT:

//         break;

//     case RIGHT:

//         break;
//     default:
//         break;
//     }
// }

// // starts both motors at same velocity
// // int startMotors(int endVelocity) {
// //     Motor_Accelerate(MOTORA, 0, endVelocity, );
// // }

// // stops / pauses the motors, can resume with startMotor
// int stopMotors() {
//     Motor_setVelocity(MOTORA, 0);
//     Motor_setVelocity(MOTORB, 0);
// }