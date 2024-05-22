/*****************************************
 * Class:: CSC-615-01 Spring 2024
 *
 * Group Member Names:: Tyler Hsieh, Anthony Silva, Pedro Grande, Rafael Sant Ana Leitao
 * SFSU IDs: 920216320, 922907645, 921149265, 922965105
 *
 * 
 * Project:: Autonomous Line-Following Car
 *
 * File:: motorLibrary.c
 *
 * Description:: This file implements the low-level motor control library
 *               for the autonomous car project. It encapsulates the functionality
 *               necessary to interact directly with the motor driver via I2C communication.
 *               Functions included allow for initializing the motor driver, setting PWM
 *               frequencies, adjusting PWM duty cycles, and managing GPIO levels for precise
 *               motor control.
 ******************************************/

#include "motorLibrary.h"
#include <pigpio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h> //exit()
#include <fcntl.h>  //define O_RDWR
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

int motorDriverHandle;

int motorLibInit(void)
{ // assumes that gpioInitialise was already called in main, intializes motor library
    motorDriverHandle = i2cOpen(I2C_BUS, HAT_ADDR, 0);
    if (motorDriverHandle < 0)
    {
        printf("i2cOpen did not intialize :/\n");
        return -1;
    }
    i2cWriteByteData(motorDriverHandle, MODE1, 0x00);
    motorLib_setPWMFreq(100);
    return 0;
}

void motorLib_setPWMFreq(uint16_t freq)
{
    freq *= 0.9; // correct for overshoot
    double prescaleVal = 25000000.0;
    prescaleVal /= 4096.0;
    prescaleVal /= freq;
    prescaleVal -= 1;

    uint8_t prescale = floor(prescaleVal + 0.5);

    uint8_t oldmode = i2cReadByteData(motorDriverHandle, MODE1);
    
    // set the ocilator to sleep mode to modify pre_scale register
    uint8_t newmode = (oldmode & 0x7F) | 0x10; // sleep

    i2cWriteByteData(motorDriverHandle, MODE1, newmode);

    // change the prescale (ocilator's frequency)
    i2cWriteByteData(motorDriverHandle, PRESCALE, prescale);

    // wake ocilator and restore previous mode
    i2cWriteByteData(motorDriverHandle, MODE1, oldmode);

}

static void motorLib_setPWM(uint8_t channel, uint16_t on, uint16_t off)
{
    i2cWriteByteData(motorDriverHandle, LED0_ON_L + 4 * channel, on & 0xFF);
    i2cWriteByteData(motorDriverHandle, LED0_ON_H + 4 * channel, on >> 8);
    i2cWriteByteData(motorDriverHandle, LED0_OFF_L + 4 * channel, off & 0xFF);
    i2cWriteByteData(motorDriverHandle, LED0_OFF_H + 4 * channel, off >> 8);
}

void motorLib_setPWMDutyCycle(uint8_t channel, uint16_t pulse)
{
    motorLib_setPWM(channel, 0, pulse * (4096 / 100) - 1);
}

void motorLib_setLevel(uint8_t channel, uint16_t value)
{
    if (value == 1)
    {
        motorLib_setPWM(channel, 0, 4095);
    }
    else
    {
        motorLib_setPWM(channel, 0, 0);
    }
}

void motorLib_terminate(void)
{
    i2cClose(motorDriverHandle);
}