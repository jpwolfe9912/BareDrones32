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

	motor_value[MOTOR1] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR2] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR3] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR4] = DSHOT_CMD_MOTOR_STOP;

	dshotWait(DSHOT_ARM_COUNT);
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

void
motorsSettingsSave(void)
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

	motor_value[MOTOR2] = DSHOT_CMD_SAVE_SETTINGS;
	motor_value[MOTOR4] = DSHOT_CMD_SAVE_SETTINGS;
	dshotWait(DSHOT_SETTINGS_COUNT);

	motor_value[MOTOR2] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR4] = DSHOT_CMD_MOTOR_STOP;
	dshotWait(DSHOT_ARM_COUNT);

	printf("\nSettings Saved\n");
}

void
motorsChangeMode(dshotCommands_e command, motors_grouped_e motors)
{
	dshot_command_count = 0;

	/* Send 0 command 1500 times */
	motor_value[MOTOR1] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR2] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR3] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR4] = DSHOT_CMD_MOTOR_STOP;
	dshotWait(DSHOT_ARM_COUNT);

	/* Send nothing for 10ms */
	motor_initialized = true;
	delay(10);
	motor_initialized = false;

	/* Send command 10 times */
	motor_value[MOTOR1] = ((motors & 0x01) >> 0) * command;
	motor_value[MOTOR2] = ((motors & 0x02) >> 1) * command;
	motor_value[MOTOR3] = ((motors & 0x04) >> 2) * command;
	motor_value[MOTOR4] = ((motors & 0x08) >> 3) * command;
	dshotWait(DSHOT_SETTINGS_COUNT);

	/* Send 0 command 1500 times */
	motor_value[MOTOR1] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR2] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR3] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR4] = DSHOT_CMD_MOTOR_STOP;
	dshotWait(DSHOT_ARM_COUNT);

	/* Change modes */
	if(command == DSHOT_CMD_3D_MODE_ON)
	{
		motors_mode = MOTORS_MODE_3D;
		printf("\nChanged motors 3D mode");
	}
	else if(command == DSHOT_CMD_3D_MODE_OFF)
	{
		motors_mode = MOTORS_MODE_NORMAL;
		printf("\nChanged motors NORMAL mode");
	}
}

void
motorsBeep(motors_beeps_e beep)
{
	motor_value[MOTOR1] = beep;
	motor_value[MOTOR2] = beep;
	motor_value[MOTOR3] = beep;
	motor_value[MOTOR4] = beep;
	dshotWait(10);

	systemReady = false;
	motor_initialized = true;
	delay(300);
	systemReady = true;
	motor_initialized = false;

	motor_value[MOTOR1] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR2] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR3] = DSHOT_CMD_MOTOR_STOP;
	motor_value[MOTOR4] = DSHOT_CMD_MOTOR_STOP;
}

