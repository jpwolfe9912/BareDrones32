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

/* Defines */
#define MOTOR_COUNT			4

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

typedef enum
{
	MOTORS_NONE,
	MOTORS_1,
	MOTORS_2,
	MOTORS_12,
	MOTORS_3,
	MOTORS_13,
	MOTORS_23,
	MOTORS_123,
	MOTORS_4,
	MOTORS_14,
	MOTORS_24,
	MOTORS_124,
	MOTORS_34,
	MOTORS_134,
	MOTORS_234,
	MOTORS_1234
}motors_grouped_e;

/* Global Variables */
extern uint16_t motor_value[MOTOR_COUNT];

extern semaphore_t motor_initialized;

extern motors_mode_e motors_mode;

/* Function Prototypes */
void motorInit(void);
void motors3dOn(void);
void motors3dOff(void);
void motorsSettingsSave(void);
void motorsChangeMode(dshotCommands_e command, motors_grouped_e motors);

#endif /* __MOTORS_H__ */
