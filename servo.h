/***************************************************************************************************
 * Class:: CSC-615-01 Spring 2024
 *
 * Group Member Names:: Tyler Hsieh, Anthony Silva, Pedro Grande, Rafael Sant Ana Leitao
 * SFSU IDs: 920216320, 922907645, 921149265, 922965105
 * Github Usernames: Tyler9648, anthonySilva23, PFGrande, Rafael-Leitao
 * 
 * Project:: Autonomous Line-Following Car
 *
 * File:: servo.h
 *
 * Description:: This header file defines the interface for controlling servo motors
 *               in the autonomous car project. It includes definitions for the servo control
 *               pin, PWM frequency, and predefined positions for panning right, left, and forward.
 **************************************************************************************************/

#ifndef __SERVO_
#define __SERVO_

#define SERVO_PIN 12
#define PWD_FREQ 50
#define SERVO_RIGHT 800     //old 700, 620, 500 --- 800 WORKS, ALTHO SMALL MARGIN OF ERROR
                            //700 to 600 work more consistent, but make the car turn wider 
#define SERVO_LEFT 2500
#define SERVO_FORWARD 1700

int Servo_Init(void);
void Pan_Right();
void Pan_Left();
void Pan_Forward();
#endif