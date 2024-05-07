/*****************************************
 * Class:: CSC-615-01 Spring 2024
 *
 * File:: sensorThread.h
 *
 * Description:: This header file defines the structures and enumerations for managing sensors
 *               in the autonomous car project. It includes the `tArg` structure which holds the
 *               necessary arguments for sensor threads such as sensor type, pin configurations,
 *               and sensor values. The file also declares functions for reading from different
 *               types of sensors (line, avoidance, and sonar) and a function for controlling
 *               GPIO pins.
 ******************************************/

#ifndef SENSOR_THREAD
#define SENSOR_THREAD

typedef enum
{              //  --  Types of sensors  --
    LINE = 0,  // -> Line sensor
    AVOID = 1, // -> Obstacle sensor
    TEST = 2,  // -> used for thread debugging
    SONAR = 3  // -> Sonar sensor, requires 2 pins
} SENSORTYPE;

typedef struct tArg
{ // sensor thread arguments
    SENSORTYPE senType;
    int pin, trigger;            // -> sensor's gpio pin, trigger pin only present for sonar
    int value;                   // -> sensor's read value, will be read from main loop
    double lastSensorUpdateTime; //
} tArg;

void *sensor_thread(void *arg);
int lineSensor(int pin);                      // directly reads line sensor
int avoidSensor(int pin);                     // directly reads avoid sensor
double sonarSensor(int pin, int trigger);     // directly returns sonar distance
int timedGPIOHigh(int trigger, int duration); // sets pin to HIGH (1) for duration in micro secs
#endif