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
}

/** @brief Changes motor mode to 3D.
 *
 *  @return Void.
 */
void
motors3dOn(void)
{
	delay(1000);
	motor_value[MOTOR2] = DSHOT_CMD_3D_MODE_ON;
	motor_value[MOTOR4] = DSHOT_CMD_3D_MODE_ON;
	delay(10);

	motor_value[MOTOR2] = 0;
	motor_value[MOTOR4] = 0;
	delay(1500);
}

