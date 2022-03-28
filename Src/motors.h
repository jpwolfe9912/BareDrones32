/** @file 		motors.h
 *  @brief
 *  	This file contains functions to initialize the ESC or change modes
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

#ifndef __MOTORS_H__
#define __MOTORS_H__

/* Global Enums */
typedef enum
{
	MOTOR1,
	MOTOR2,
	MOTOR3,
	MOTOR4
}motors_e;

typedef enum
{
	MOTORS_MODE_NORMAL,
	MOTORS_MODE_3D
}motors_mode_e;

/* Global Variables */
extern uint16_t motor_value[4];

extern semaphore_t motor_initialized;

extern motors_mode_e motors_mode;

/* Function Prototypes */
void motorInit(void);
void motors3dOn(void);
void motors3dOff(void);

#endif /* __MOTORS_H__ */
