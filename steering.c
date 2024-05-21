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
#define MIN_TURNING_SPEED -20
#define DETECT_OBS_DIST 20
#define MIN_OBS_DIST 12 //old 17
#define MAX_OBS_DIST 28

#define MIN_OBS_AVOID_SPEED (AVOIDANCE_SPEED - 25)
#define MAX_OBS_AVOID_SPEED (AVOIDANCE_SPEED + 25)

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

int obsDetected(void){
    if ((sonarSensorArgs->value <= DETECT_OBS_DIST && sonarSensorArgs->value > 0) || avoidSensorArgs->value == 0){   //recent
        return 1;
    } else {
        return 0;
    }
   
}

int steer()
{
    Motor_setVelocity(MOTORA, BASE_SPEED);
    Motor_setVelocity(MOTORB, BASE_SPEED);
    int sleepVal = 10000;

    // Obstacle detected
    if (obsDetected() == 1)
    {
        printf("Obstacle detected: distance is %d\n", sonarSensorArgs->value);
        avoidObstacle();
        
    }

    // for sharp turns, slow down the speed
    if (multiLineSensorArgs->value3 == 1 && (multiLineSensorArgs->value1 == 1 || multiLineSensorArgs->value5 == 1) && (multiLineSensorArgs->value1 != multiLineSensorArgs->value5))
    {
        Motor_setVelocity(MOTORA, BASE_SPEED / 2);
        Motor_setVelocity(MOTORB, BASE_SPEED / 2);
        sleepVal *= 0.65;
    }
    int steerValue = getSum();
    while (exitThread == 0 && /*getSum()*/ steerValue != 0 && obsDetected() == 0)
    { // while its not an intersection or not dead center on the line
        steerValue = getSum();
        
        if((motorASpeed + steerValue) > MIN_TURNING_SPEED)
            Motor_setVelocity(MOTORA, motorASpeed + steerValue); // assume currSpeed is updated by setVelocity, and is between -100 to 100
        if((motorBSpeed - steerValue) > MIN_TURNING_SPEED)
            Motor_setVelocity(MOTORB, motorBSpeed - steerValue); // this doesn't require encoder cus im lazy, but if needed can be changed
        usleep(sleepVal);
    }

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
    while (multiLineSensorArgs->value3 != 1 && exitThread == 0)
    {
        printf("sonar distance: %d\n", sonarSensorArgs->value);
        int motorAadjust = 1;
        int motorBadjust = 1;

        if (sonarSensorArgs->value < MIN_OBS_DIST)            //too close to obstacle
        {
            if((motorASpeed - 1) <= MIN_OBS_AVOID_SPEED){
                motorAadjust = 0;
            }
            if((motorBSpeed + 1) >= MAX_OBS_AVOID_SPEED){
                motorBadjust = 0;
            }
            if(sonarSensorArgs->value < MIN_OBS_DIST * 0.7){        //IF IT IS REALLY WAY TOO CLOSE TO THE OBSTACLE
                motorAadjust *= 2;
                motorBadjust *= 2;
            }
            Motor_setVelocity(MOTORA, motorASpeed - motorAadjust);
            Motor_setVelocity(MOTORB, motorBSpeed + motorBadjust);
        }
        else if (                                   //optimal distance
            (sonarSensorArgs->value < MAX_OBS_DIST))
            {
                Motor_setVelocity(MOTORA, AVOIDANCE_SPEED);
                Motor_setVelocity(MOTORB, AVOIDANCE_SPEED);
            }
        else
        {
            if((motorASpeed + 1) >= MAX_OBS_AVOID_SPEED){
                motorAadjust = 0;
            }
            if((motorBSpeed - 1) <= MIN_OBS_AVOID_SPEED){
                motorBadjust = 0;
            }
            Motor_setVelocity(MOTORA, motorASpeed + motorAadjust);
            Motor_setVelocity(MOTORB, motorBSpeed - motorBadjust);
        }
        usleep(8000);      //old: 20000
    }
    Pan_Forward();
    printf("\nwere back on the line\n");
    Motor_setVelocity(MOTORA, -30);
    Motor_setVelocity(MOTORB, 30);
    usleep(1000000);
     Motor_setVelocity(MOTORA, 0);
    Motor_setVelocity(MOTORB, 0);
    usleep(1000);
    Motor_setVelocity(MOTORA, AVOIDANCE_SPEED);
    Motor_setVelocity(MOTORB, AVOIDANCE_SPEED);
    prevSum = 2;
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

int getSum()
{

    // start with outter sensors?
    double sum = 0;
    int sensorsTriggered = 0;
    if (multiLineSensorArgs->value1 == 1)
    {
        sum += 3;
        sensorsTriggered++;
    }
    if (multiLineSensorArgs->value2 == 1)
    {
        sum += 1;
        sensorsTriggered++;
    }
    if (multiLineSensorArgs->value3 == 1)
    {
        sensorsTriggered++;
    }
    if (multiLineSensorArgs->value4 == 1)
    {
        sum -= 1;
        sensorsTriggered++;
    }
    if (multiLineSensorArgs->value5 == 1)
    {
        sum -= 3;
        sensorsTriggered++;
    }

    if (sensorsTriggered == 0)
    {
        return prevSum;
    }

    int intSum;
    if (multiLineSensorArgs->value3 == 1 && (multiLineSensorArgs->value1 == 1 || multiLineSensorArgs->value5 == 1) && (multiLineSensorArgs->value1 != multiLineSensorArgs->value5))
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
    
    return intSum;
}
