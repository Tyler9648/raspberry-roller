#include "steering.h"

#define MAX_DISTANCE 40
#define MIN_DISTANCE 20
#define TEMP_SPEED 50

extern volatile sig_atomic_t exitThread;

void tempAvoidance() {
    Motor_setVelocity(MOTORA, 0);
    Motor_setVelocity(MOTORB, 0);
    
    // REMEMBER TO turn 90 DEGREES RIGHT, or until sensor
    // no longer reads the object

    // turn right until sonar no longer detects object (edge found)
    // while (sonarSensorArgs->value > 0 || sonarSensorArgs->value < 100) {
    //     Motor_setVelocity(MOTORA, 0 - TEMP_SPEED);
    // }

    // Pan_Right();

    // Motor_setVelocity(MOTORA, 0);
    // Motor_setVelocity(MOTORB, 0);

    // while(sonarSensorArgs)

    // loop below should already turn car away from the object, code would be redundant
    // find condition which is true when there is an obstacle in the way
    while (lineSensorThreeArgs != 1 && sonarSensorArgs->value > 0 && exitThread == 0) {
        
        if (sonarSensorArgs->value <= MIN_DISTANCE) {
            // move forward
            printf("move forward parallel to object\n");
            Motor_setVelocity(MOTORA, TEMP_SPEED);
            Motor_setVelocity(MOTORB, TEMP_SPEED);

        } else if (sonarSensorArgs->value >= MAX_DISTANCE) { 
            printf("turn towards object\n");
            // turn closer
            Motor_setVelocity(MOTORA, TEMP_SPEED);
            Motor_setVelocity(MOTORB, 0);
            Pan_Right(); // servo turns after car has detected first edge
        }

        usleep(3000); // gradual change, could make us turn more than 90 degrees from
                      // the object

    }

}