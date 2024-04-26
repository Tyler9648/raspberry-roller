#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>

#define GPIO_SERVO 12

int main()
{
    // Initialize the pigpio library
    if (gpioInitialise() < 0)
    {
        printf("Failed to initialize pigpio library\n");
        return 1;
    }

    // Set GPIO_SERVO pin as an output
    gpioSetMode(GPIO_SERVO, PI_OUTPUT);

    // Set PWM frequency to 50 Hz (standard for most servos)
    gpioSetPWMfrequency(GPIO_SERVO, 50);

    // Move servo to its minimum position
    gpioServo(GPIO_SERVO, 500); // 500 is the pulse width for minimum position

    // Wait for 2 seconds
    usleep(2000000);

    // Move servo to its maximum position
    gpioServo(GPIO_SERVO, 2500); // 2500 is the pulse width for maximum position

    // Wait for 2 seconds
    usleep(2000000);

    // Move servo to its middle position
    gpioServo(GPIO_SERVO, 1500); // 1500 is the pulse width for middle position

    // Wait for 2 seconds
    usleep(2000000);

    // Turn off PWM
    gpioPWM(GPIO_SERVO, 0);

    // Cleanup GPIO and pigpio library
    gpioTerminate();

    return 0;
}