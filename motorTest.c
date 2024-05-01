#include "motor.h"
#include "motorLibrary.h"
#include <time.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pigpio.h>
#include <math.h>
volatile sig_atomic_t exitThread = 0;
int main(){

    if(gpioInitialise() < 0){
        printf("gpio initialization failed, exiting\n");
        exit(0);
    } else {
        printf("gpio initialization succeeded!\n");
    }   

    if(Motor_Init() < 0) {
        printf("Failed to initialize servo motor.\n");
        return 1;
    }


        Motor_setVelocity(MOTORA, 50);
        Motor_setVelocity(MOTORB, 50);
        usleep(300000);       
        Motor_Terminate();
        gpioTerminate();
        return 0;
}