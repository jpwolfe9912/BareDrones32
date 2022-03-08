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

/* Global Variables */
extern uint16_t motor_value[4];

extern semaphore_t motor_initialized;

/* Global Enums */
typedef enum
{
	MOTOR1,
	MOTOR2,
	MOTOR3,
	MOTOR4
}motors_e;

/* Function Prototypes */
void motorInit(void);
void motors3dOn(motors_e motor);

#endif /* __MOTORS_H__ */
