/*
 * motors.c
 *
 *  Created on: Feb 23, 2022
 *      Author: jeremywolfe
 */

#include "board.h"

uint16_t motor_value[4];

semaphore_t motor_initialized = 0;

void motorInit(void)
{
	printf("\nInitializing Motors\n");

	motor_value[0] = 0;
	motor_value[1] = 0;
	motor_value[2] = 0;
	motor_value[3] = 0;
//	delay(1000);


}

void motorZeroCommand(void)
{
//	motor_value[0] = 48;
//	motor_value[1] = 48;
//	motor_value[2] = 48;
//	motor_value[3] = 48;
//	delay(10);
}

void
motors3dOn(motors_e motor)
{
	motor_value[motor] = DSHOT_CMD_3D_MODE_ON;
	delay(10);
	motor_value[motor] = 0;
	delay(1500);
//	motor_value[motor] = DSHOT_CMD_SAVE_SETTINGS;
//	delay(6);


}

