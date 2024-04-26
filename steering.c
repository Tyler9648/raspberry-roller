#include "steering.h"

// I don't know what to name this variable, just stores if the 
// car is going forwards or backwards
int behavior = FORWARD; 
int leftSpeed = 0;
int rightSpeed = 0;

/*
e = error                       // distance of sensor from line (distance between middle and the side sensor that detected the line)
de/dt = e/t                  //t is time elapsed since middle sensor veered off line, rate of error increase
 ∫e dt  = total error   //total error since elapsed

-- constants --          // will have to test these manually to get optimal value, should be somewhat proportional to cars turning rate relative to distance
                                       // between the middle line sensor and a tire (doesnt matter which tire cos its the same)
Kp 
Kd
Ki

*/

int getError() { // computes the error from the line    
    
    // check amount of time sensor has been off line
    if (lineSensorThreeArgs->value == 0) {
        double lastTimeOnLine = lineSensorThreeArgs->lastSensorUpdateTime;

        // current time - last time middle sensor was on line = time middle sensor
        // has been off the line
        double timeOffLine = (clock() / CLOCKS_PER_SEC) - lastTimeOnLine;
    }

    
    int Kp = 5; // TEMPORARY VALUE, WILL BE ADJUSTED LATER

    // read from line sensor
    double distanceFromMid; // determine the distance between the sensors using the weights;
    double totalError = 0;
    double dt = timeOffLine - lastTime; 
    double steerVal = (Kp * sensorDist) + (Kd * getWeight()/dt);

    /*
    getting the lastest sensor that has updated:
        start by checking the outter sensors
    
    
    
    */

}

double getWeight() {
    // assume line sensors around around 2.5 centimeters apart:
    // 0.025 m

    // start with outter sensors?
    const double distBetweenLineSensor = 0.025;
    if (lineSensorOneArgs->value == 1) {
        return LEFT_SENS * distBetweenLineSensor;
    }
    if (lineSensorFiveArgs->value == 1) {
        return RIGHT_SENS * distBetweenLineSensor;
    }
    if (lineSensorTwoArgs->value == 1) {
        return LEFT_MID_SENS * distBetweenLineSensor;
    }
    if (lineSensorThreeArgs->value == 1) {
        return RIGHT_MID_SENS * distBetweenLineSensor;
    }
    // if (lineSensorFourArgs->value == 1) { if the sensor has been off the line, this is not needed
    //     return MID_SENS; // returns 0
    // }
    
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