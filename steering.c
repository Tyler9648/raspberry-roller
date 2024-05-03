#include "steering.h"
extern volatile sig_atomic_t exitThread;

#define     BASE_SPEED       80

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

int initSteering() {
    return Motor_Init();
}

void terminateSteering() {
    Motor_Terminate();
}


int steer() { 
    struct sensorOnLine * lineDetectedBy = malloc(sizeof(struct sensorOnLine)); 
    double timeOffLine = 0;

    getError(lineDetectedBy);
    // TEMPORARY VALUES
    int kp, kd, ki; 
    kp = 2;
    kd = 2;
    ki = 2;


    // TEMPORARY : In a different function we will track when the
    // middle sensor gets on the line
    if (lineSensorThreeArgs->value == 1) {
        printf("middle on line\n");
        // go forward if middle sensor is on line
        timeMiddleOnLine = lineSensorThreeArgs->lastSensorUpdateTime;
        Motor_setVelocity(MOTORA, 50);
        Motor_setVelocity(MOTORB, 50);
        return 0;
    }


    // if middle sensor if off the line
    if (lineSensorThreeArgs->value == 0) { 
        // computer dt
        timeOffLine = lineSensorThreeArgs->lastSensorUpdateTime - timeMiddleOnLine;
    }

    int steerValue = floor((kp * lineDetectedBy->distanceFromMiddle)
    + (kd * lineDetectedBy->distanceFromMiddle / timeOffLine));

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

    while(lineSensorThreeArgs->value != 1 && tracingObstacle == false && exitThread == 0){
        Motor_setVelocity(MOTORA, motorASpeed - steerValue);  //assume currSpeed is updated by setVelocity, and is between -100 to 100
        Motor_setVelocity(MOTORB, motorBSpeed + steerValue);  //this doesn't require encoder cus im lazy, but if needed can be changed
        usleep(3000);                                            //adjustable, steering frequency, lower num = higher freq = smoother
        totalError += ki * sensorDist * 3000 / 1000000;            //adding up total error, with respect to time in seconds
        // time middle sensor has been off the line
        timeOffLine = timeOffLine - lineSensorThreeArgs->lastSensorUpdateTime;
        // timeOffLine = timeOffLine - lastSensorUpdateTime; 
        steerValue = (kp * sensorDist) + (kd * sensorDist/timeOffLine) + totalError;
    }

    free(lineDetectedBy);
    return 0;
}


// use struct to indicate which sensor was read from

int getError(struct sensorOnLine * sensorInfo) {
    // assume line sensors around around 2.5 centimeters apart:
    // 0.025 m (placeholder val), should we use different measurement unit?

    // start with outter sensors?
    const double distBetweenLineSensor = 0.025;

    if (lineSensorOneArgs->value == 1) {
        sensorInfo->distanceFromMiddle = LEFT_SENS * distBetweenLineSensor;
        sensorInfo->sensorUpdateTime = lineSensorOneArgs->lastSensorUpdateTime;
    }
    if (lineSensorFiveArgs->value == 1) {
        sensorInfo->distanceFromMiddle = RIGHT_SENS * distBetweenLineSensor;
        sensorInfo->sensorUpdateTime = lineSensorFiveArgs->lastSensorUpdateTime;
    }
    if (lineSensorTwoArgs->value == 1) {
        sensorInfo->distanceFromMiddle = LEFT_MID_SENS * distBetweenLineSensor;
        sensorInfo->sensorUpdateTime = lineSensorTwoArgs->lastSensorUpdateTime;
    }
    if (lineSensorFourArgs->value == 1) {
        sensorInfo->distanceFromMiddle = RIGHT_MID_SENS * distBetweenLineSensor;
        sensorInfo->sensorUpdateTime = lineSensorFourArgs->lastSensorUpdateTime;
    }
    if (lineSensorThreeArgs->value == 1) { 
        sensorInfo->distanceFromMiddle = MID_SENS * distBetweenLineSensor;
        sensorInfo->sensorUpdateTime = lineSensorThreeArgs->lastSensorUpdateTime;
    }
    
}

int steerTest() {
    //struct sensorOnLine * lineDetectedBy = malloc(sizeof(struct sensorOnLine)); 
        Motor_setVelocity(MOTORA, BASE_SPEED);  
        Motor_setVelocity(MOTORB, BASE_SPEED); 
        int sleepVal = 16000;
    //for sharp turns, slow down the speed
    if (lineSensorThreeArgs->value == 1 && (lineSensorOneArgs->value == 1 || lineSensorFiveArgs->value == 1) && (lineSensorOneArgs->value != lineSensorFiveArgs->value)){
        Motor_setVelocity(MOTORA, BASE_SPEED/2);  
        Motor_setVelocity(MOTORB, BASE_SPEED/2); 
        sleepVal = 11000;
    }
    while(exitThread==0 && getSum() != 0){      //while its not an intersection or not dead center on the line
        int steerValue = getSum();    
        Motor_setVelocity(MOTORA, motorASpeed + steerValue);  //assume currSpeed is updated by setVelocity, and is between -100 to 100
        Motor_setVelocity(MOTORB, motorBSpeed - steerValue);  //this doesn't require encoder cus im lazy, but if needed can be changed
        usleep(sleepVal);     
    }
    /*
    if(lineSensorThreeArgs->value == 1){
        Motor_setVelocity(MOTORA, 25);  
        Motor_setVelocity(MOTORB, 25);      
    }*/
    return 0;
}

int getSum(/*struct sensorOnLine * sensorInfo*/) {
    // assume line sensors around around 2.5 centimeters apart:
    // 0.025 m (placeholder val), should we use different measurement unit?

    // start with outter sensors?
    double sum = 0;
    int sensorsTriggered = 0;
    if (lineSensorOneArgs->value == 1) {
        sum -= 3;
        sensorsTriggered++;
    }
    if (lineSensorTwoArgs->value == 1) {
        sum -= 1;
        sensorsTriggered++;
    }
    if (lineSensorThreeArgs->value == 1) {
        sensorsTriggered++;
    }
    if (lineSensorFourArgs->value == 1) {
        sum += 1;
        sensorsTriggered++;
    }
    if (lineSensorFiveArgs->value == 1) {
        sum += 3;
        sensorsTriggered++;
    }

    if(sensorsTriggered == 0){
        //printf("getSum -> prevSum: %d\n", prevSum);
        return prevSum;
    }

    int intSum;
    if (lineSensorThreeArgs->value == 1 && (lineSensorOneArgs->value == 1 || lineSensorFiveArgs->value == 1) && (lineSensorOneArgs->value != lineSensorFiveArgs->value)){
       intSum = (int) (  sum / (sensorsTriggered - 1)  );       //this is really scuffed but it works
       intSum = (intSum / abs(intSum)) * 4;              //4 is total sum of one side of sensors 
    } else {
       intSum = (int) (sum / sensorsTriggered);         //it is totally intentional to have it 0.5 be rounded down to 0
    }                                                   //when converting double to int, for averaging sum of 1 between 
                                                        //a middle and inner line sensor
    if(intSum != 0){
        prevSum = intSum;
    }
    //printf("getSum -> intSum: %d\n", intSum);
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