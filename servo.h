/***************************************************************************************************
 * Class:: CSC-615-01 Spring 2024
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
#define RIGHT 500
#define LEFT 2500
#define FORWARD 1500

int Servo_Init(void);
void Pan_Right();
void Pan_Left();
void Pan_Forward();
#endif