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