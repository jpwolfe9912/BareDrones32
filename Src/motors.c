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
uint16_t motor_value[MOTOR_COUNT];

semaphore_t motor_initialized = false;

motors_mode_e motors_mode = MOTORS_MODE_NORMAL;


/** @brief Sets motor_value to 0 for all motors.
 *
 *  @return Void.
 */
void
motorInit(void)
{
	printf("\nInitializing Motors\n");

	motor_value[0] = DSHOT_CMD_MOTOR_STOP;
	motor_value[1] = DSHOT_CMD_MOTOR_STOP;
	motor_value[2] = DSHOT_CMD_MOTOR_STOP;
	motor_value[3] = DSHOT_CMD_MOTOR_STOP;

	dshotWait(1000);
}

/** @brief Changes motor mode to 3D.
 *
 *  @return Void.
 */
void
motors3dOn(void)
{
	motor_value[MOTOR1] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR2] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR3] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR4] = DSHOT_CMD_MOTOR_STOP;
	dshotWait(DSHOT_ARM_COUNT);

	motor_initialized = true;
	delay(10);
	motor_initialized = false;

	motor_value[MOTOR2] = DSHOT_CMD_3D_MODE_ON;
	motor_value[MOTOR4] = DSHOT_CMD_3D_MODE_ON;
	dshotWait(DSHOT_SETTINGS_COUNT);

	motor_value[MOTOR2] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR4] = DSHOT_CMD_MOTOR_STOP;
	dshotWait(DSHOT_ARM_COUNT);

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
	dshot_command_count = 0;

	motor_value[MOTOR1] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR2] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR3] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR4] = DSHOT_CMD_MOTOR_STOP;
	dshotWait(DSHOT_ARM_COUNT);

	motor_initialized = true;
	delay(10);
	motor_initialized = false;

	motor_value[MOTOR2] = DSHOT_CMD_3D_MODE_OFF;
	motor_value[MOTOR4] = DSHOT_CMD_3D_MODE_OFF;
	dshotWait(DSHOT_SETTINGS_COUNT);

	motor_value[MOTOR2] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR4] = DSHOT_CMD_MOTOR_STOP;
	dshotWait(DSHOT_ARM_COUNT);

	motors_mode = MOTORS_MODE_NORMAL;
	printf("\nChanged motors 2 and 4 to NORMAL mode\n");
}

