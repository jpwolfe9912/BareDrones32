/** @file 		motors.c
 *  @brief
 *  	This file contains functions to initialize the ESC or change modes
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

/* Includes */
#include "board.h"

/* Global Variables */
uint16_t motor_value[4];

semaphore_t motor_initialized = 0;

motors_mode_e motors_mode = MOTORS_MODE_NORMAL;


/** @brief Sets motor_value to 0 for all motors.
 *
 *  @return Void.
 */
void
motorInit(void)
{
	printf("\nInitializing Motors\n");

	motor_value[0] = 0;
	motor_value[1] = 0;
	motor_value[2] = 0;
	motor_value[3] = 0;

	delay(1000);
}

/** @brief Changes motor mode to 3D.
 *
 *  @return Void.
 */
void
motors3dOn(void)
{
	motor_value[MOTOR1] = 0;
	motor_value[MOTOR2] = 0;
	motor_value[MOTOR3] = 0;
	motor_value[MOTOR4] = 0;

	delay(1000);

	motor_value[MOTOR2] = DSHOT_CMD_3D_MODE_ON;
	motor_value[MOTOR4] = DSHOT_CMD_3D_MODE_ON;
	delay(50);

	motor_value[MOTOR2] = 0;
	motor_value[MOTOR4] = 0;
	delay(1500);

	motors_mode = MOTORS_MODE_3D;
	printf("\nChanged motors 2 and 4 to 3D mode\n");
}

/** @brief Changes motor mode to 3D.
 *
 *  @return Void.
 */
void
motors3dOff(void)
{
	motor_value[MOTOR1] = 0;
	motor_value[MOTOR2] = 0;
	motor_value[MOTOR3] = 0;
	motor_value[MOTOR4] = 0;

	delay(1000);

	motor_value[MOTOR2] = DSHOT_CMD_3D_MODE_OFF;
	motor_value[MOTOR4] = DSHOT_CMD_3D_MODE_OFF;
	delay(50);

	motor_value[MOTOR2] = 0;
	motor_value[MOTOR4] = 0;
	delay(1500);

	motors_mode = MOTORS_MODE_NORMAL;
	printf("\nChanged motors 2 and 4 to NORMAL mode\n");
}

