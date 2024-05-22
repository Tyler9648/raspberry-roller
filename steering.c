/*******************************************************************************************************************
 * Class:: CSC-615-01 Spring 2024
 * 
 * Group Member Names:: Tyler Hsieh, Anthony Silva, Pedro Grande, Rafael Sant Ana Leitao
 * SFSU IDs: 920216320, 922907645, 921149265, 922965105
 * Github Usernames: Tyler9648, anthonySilva23, PFGrande, Rafael-Leitao
 * 
 * Project:: Autonomous Line-Following Car
 *
 * File:: steering.c
 *
 * Description:: This source file contains the implementation of the steering logic for the autonomous car project.
 *               It integrates sensor readings with motor control to dynamically adjust the car's steering based on
 *               its position relative to a track. Key functionalities include initializing and terminating steering
 *               mechanisms, executing main steering function to keep itself on the line, remembering the last turn (prevSum), 
 *               and more complex maneuvers like error correction based on sharp turns, intersections, and obstacle avoidance.
 ********************************************************************************************************************/
#include "steering.h"
extern volatile sig_atomic_t exitThread;

#define BASE_SPEED 70
#define STEER_FREQ_DELAY 5000  //how long steer loop takes to adjust in microsecs
                               //higher delay means slower turning, and vice versa
                
#define OBS_FREQ_DELAY 8000    //similar to steer freq delay, affects how fast or slow
                               //the car adjusts itself. higher delay means slower turning, and vice versa

#define AVOIDANCE_SPEED 60 // base speed when going around in obstacle
#define MIN_TURNING_SPEED -20   
#define DETECT_OBS_DIST 20      //in cm
#define MIN_OBS_DIST 12         //in cm
#define MAX_OBS_DIST 28         //in cm


#define MIN_OBS_AVOID_SPEED (AVOIDANCE_SPEED - 25)
#define MAX_OBS_AVOID_SPEED (AVOIDANCE_SPEED + 25)

double timeMiddleOnLine; // time before middle sensor left the line
int prevSum = 0;         // keeps track of last turn

int initSteering()
{

    return Motor_Init();
}

void terminateSteering()
{
    Motor_Terminate();
}

// check if there is an obstacle within 20 cm of the sonar
int obsDetected(void){
    //if obstacle detected
    if ((sonarSensorArgs->value <= DETECT_OBS_DIST && 
        sonarSensorArgs->value > 0) || avoidSensorArgs->value == 0
        ) 
    {   
        return 1;
    } else {
        return 0;
    }
   
}

// Default behavior that allows the car to follow the line
int steer()
{
    Motor_setVelocity(MOTORA, BASE_SPEED);
    Motor_setVelocity(MOTORB, BASE_SPEED);
    int sleepVal = STEER_FREQ_DELAY;

    // Obstacle detected
    if (obsDetected() == 1)
    {
        printf("Obstacle detected: distance is %d\n", sonarSensorArgs->value);
        avoidObstacle();
        
    }

    // for sharp turns, slow down the speed
    if (
         multiLineSensorArgs->value3 == 1 && 
        (multiLineSensorArgs->value1 == 1 || multiLineSensorArgs->value5 == 1) && 
        (multiLineSensorArgs->value1 != multiLineSensorArgs->value5)
        )
    {
        Motor_setVelocity(MOTORA, BASE_SPEED / 2);
        Motor_setVelocity(MOTORB, BASE_SPEED / 2);
        sleepVal *= 0.65;
    }
    int steerValue = getSum();

    // while its not an intersection or not dead center on the line, 
    // correct the position of the car using the value from getSum()
    while (exitThread == 0 && steerValue != 0 && obsDetected() == 0)
    {
        steerValue = getSum();
        
        // currSpeed is in the range of -100 to 100
        // currSpeed is updated by Motor_setVelocity() 
        if((motorASpeed + steerValue) > MIN_TURNING_SPEED)
            Motor_setVelocity(MOTORA, motorASpeed + steerValue); 
        if((motorBSpeed - steerValue) > MIN_TURNING_SPEED) 
            Motor_setVelocity(MOTORB, motorBSpeed - steerValue); // Function does not use encoder
        usleep(sleepVal);
    }

    return 0;
}

// behavior of car after an obstacle is detected
void avoidObstacle()
{
    // To prevent motors from frying, they are stopped before changing
    // direction

    // Stop car
    Motor_setVelocity(MOTORA, 0);
    Motor_setVelocity(MOTORB, 0);
    usleep(1000000);

    // point sonar at the obstacle
    Pan_Right();

    // Steer car left
    Motor_setVelocity(MOTORA, -40);
    Motor_setVelocity(MOTORB, 40);
    usleep(1000000);

    Motor_setVelocity(MOTORA, 0);
    Motor_setVelocity(MOTORB, 0);
    usleep(1000000);

    // go around an object at a set speed, not the speed at which the car
    // was going at on the line
    Motor_setVelocity(MOTORA, AVOIDANCE_SPEED);
    Motor_setVelocity(MOTORB, AVOIDANCE_SPEED);
    
    // Keep going around the obstacle until we find the line or Ctr + C
    // maintain a set distance between the car and the obstacle
    // to allow the car to circle around it
    while (multiLineSensorArgs->value3 != 1 && exitThread == 0)
    {
        printf("sonar distance: %d\n", sonarSensorArgs->value);
        int motorAadjust = 1; //how much to adjust motor speed by 
        int motorBadjust = 1;

        //we're too close to obstacle, so turn away
        if (sonarSensorArgs->value < MIN_OBS_DIST) 
        {   
            // dont want to go too slow
            if((motorASpeed - 1) <= MIN_OBS_AVOID_SPEED){ 
                motorAadjust = 0;
            }

            //also dont want to go too fast
            if((motorBSpeed + 1) >= MAX_OBS_AVOID_SPEED){ 
                motorBadjust = 0;
            }

            //IF IT IS REALLY WAY TOO CLOSE TO THE OBSTACLE
            if(sonarSensorArgs->value < MIN_OBS_DIST * 0.7){ 
                motorAadjust *= 2; //THEN ADJUST EVEN MORE
                motorBadjust *= 2;
            }
            Motor_setVelocity(MOTORA, motorASpeed - motorAadjust);
            Motor_setVelocity(MOTORB, motorBSpeed + motorBadjust);
        }
        else if ( //optimal distance, so go straight
            (sonarSensorArgs->value < MAX_OBS_DIST))
            {
                Motor_setVelocity(MOTORA, AVOIDANCE_SPEED);
                Motor_setVelocity(MOTORB, AVOIDANCE_SPEED);
            }
        else //were too far from obstacle, so turn towards it 
        {
            if((motorASpeed + 1) >= MAX_OBS_AVOID_SPEED){
                motorAadjust = 0;
            }
            if((motorBSpeed - 1) <= MIN_OBS_AVOID_SPEED){
                motorBadjust = 0;
            }
            // adjust the motors based on distance from object
            Motor_setVelocity(MOTORA, motorASpeed + motorAadjust);
            Motor_setVelocity(MOTORB, motorBSpeed - motorBadjust);
        }
        usleep(OBS_FREQ_DELAY);     
    }
    // set servo to face forward
    Pan_Forward();
    printf("\nwere back on the line\n");

    // Return to the line after done circling around obstacle
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

// Each sensor's assigned weight and sign determine the direction and speed 
// of the car
int getSum()
{
    double sum = 0;
    int sensorsTriggered = 0;

    // check return value for all line sensors
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

    // When the car is off the line, return the direction the
    // car was going in befor leaving the line
    if (sensorsTriggered == 0)
    {
        return prevSum;
    }

    int intSum;

    // Adjust turn speed when encountering sharp turns with the outer sensors and
    // go through intersections
    if  (multiLineSensorArgs->value3 == 1 && 
        (multiLineSensorArgs->value1 == 1 || multiLineSensorArgs->value5 == 1) && 
        (multiLineSensorArgs->value1 != multiLineSensorArgs->value5)
        )
    {
        intSum = (int)(sum / (sensorsTriggered - 1));
        intSum = (intSum / abs(intSum)) * 4;          // 4 is total sum of one side of sensors
    }
    else
    {
        intSum = (int)(sum / sensorsTriggered); // it is totally intentional to have it 0.5 be rounded down to 0
    }                                           // when converting double to int, for averaging sum of 1 between
                                                // a middle and inner line sensor

    // update the previous sum while the car is on the line
    if (intSum != 0)
    {
        prevSum = intSum;
    }
    
    return intSum;
}
