/***************************************************************************
 * Class:: CSC-615-01 Spring 2024
 *
 * Project:: Autonomous Line-Following Car
 *
 * File:: globals.h
 *
 * Description:: This header file contains global variables and
 *               signal flags used across different modules of the
 *               autonomous car project. It centralizes the extern
 *               declarations for shared use, ensuring that all modules
 *               can access necessary data and flags.
 ****************************************************************************/

#ifndef __GLOBALS_
#define __GLOBALS_

extern volatile sig_atomic_t exitThread;

// extern tArg* lineSensorArgs;
// extern tArg* lineSensorOneArgs;
// extern tArg* lineSensorTwoArgs;
// extern tArg* lineSensorThreeArgs;
// extern tArg* lineSensorFourArgs;
// extern tArg* lineSensorFiveArgs;
// extern tArg* avoidSensorArgs;
// extern tArg* testArgs;
// extern tArg* sonarSensorArgs;

#endif