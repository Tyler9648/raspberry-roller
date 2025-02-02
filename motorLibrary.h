/*****************************************
 * Class:: CSC-615-01 Spring 2024
 *
 * Group Member Names:: Tyler Hsieh, Anthony Silva, Pedro Grande, Rafael Sant Ana Leitao
 * SFSU IDs: 920216320, 922907645, 921149265, 922965105
 * Github Usernames: Tyler9648, anthonySilva23, PFGrande, Rafael-Leitao
 * 
 * Project:: Autonomous Line-Following Car
 *
 * File:: motorLibrary.h
 *
 * Description:: This header file defines the interface for the motor control library
 *               used in the autonomous car project. It includes declarations for
 *               functions that manage motor initialization, PWM frequency adjustments,
 *               PWM duty cycle settings, and motor control levels. The file also
 *               defines constants for I2C communication, PCA channels, and motor driver
 *               register addresses which are essential for motor operation on the
 *               hardware level.
 ******************************************/

#ifndef __MOTORLIBRARY_H_
#define __MOTORLIBRARY_H_

#include <stdint.h>

#define SUBADR1 0x02
#define SUBADR2 0x03
#define SUBADR3 0x04
#define MODE1 0x00
#define PRESCALE 0xFE
#define LED0_ON_L 0x06
#define LED0_ON_H 0x07
#define LED0_OFF_L 0x08
#define LED0_OFF_H 0x09
#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD

#define PCA_CHANNEL_0 0
#define PCA_CHANNEL_1 1
#define PCA_CHANNEL_2 2
#define PCA_CHANNEL_3 3
#define PCA_CHANNEL_4 4
#define PCA_CHANNEL_5 5
#define PCA_CHANNEL_6 6
#define PCA_CHANNEL_7 7
#define PCA_CHANNEL_8 8
#define PCA_CHANNEL_9 9
#define PCA_CHANNEL_10 10
#define PCA_CHANNEL_11 11
#define PCA_CHANNEL_12 12
#define PCA_CHANNEL_13 13
#define PCA_CHANNEL_14 14
#define PCA_CHANNEL_15 15

#define I2C_BUS 1
#define HAT_ADDR 0x40

int motorLibInit(void);
void motorLib_setPWMFreq(uint16_t freq);
static void motorLib_setPWM(uint8_t channel, uint16_t on, uint16_t off);
void motorLib_setPWMDutyCycle(uint8_t channel, uint16_t pulse);
void motorLib_setLevel(uint8_t channel, uint16_t value);
void motorLib_terminate(void);
#endif