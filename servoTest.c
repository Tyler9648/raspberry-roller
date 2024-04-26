#include "servo.h"
#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>

#define GPIO_SERVO 12

int main()
{
    if (Servo_Init() < 0) {
        printf("Failed to initialize servo motor.\n");
        return 1;
    }
        usleep(2000000);
    printf("Pan right...\n");
    Pan_Right();
        usleep(2000000);
    printf("Pan left...\n");
    Pan_Left();
        usleep(2000000);
    printf("Pan forward...\n");
    Pan_Forward();
        usleep(3000000);
    gpioTerminate(); // Clean up GPIO
    return 0;
}