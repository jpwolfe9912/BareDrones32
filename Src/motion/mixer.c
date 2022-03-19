/** @file 		mixer.c
 *  @brief
 *  	This file takes the output of the PID controller and assigns values to the motors.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

/* Includes */
#include "board.h"

/* Global Variables */
uint8_t numberMotor = 4;
uint16_t throttleCmd;
int16_t steerCmd, speedCmd;
int16_t motor_temp[4];

/** @brief Pulses the motors.
 *
 *  @return Void.
 */
void
pulseMotors(void)
{
	motor_value[0] = DSHOT_CMD_BEACON1;
	motor_value[1] = DSHOT_CMD_BEACON1;
	motor_value[2] = DSHOT_CMD_BEACON1;
	motor_value[3] = DSHOT_CMD_BEACON1;
	delay(10);
}

/** @brief Mixes the values from the PID controller and assigns values to the motors.
 *
 *  @return Void.
 */
void
mixTable(void)
{
	uint8_t i;

	if((mode == FLIGHT) &&
			(armed == true) &&
			(failsafe == false))
	{
		motor_temp[0] = PIDMIXFLIGHT( -1.0f,  1.0f, -1.0f, 1.0f );      // Rear Right  CW
		motor_temp[1] = PIDMIXFLIGHT( -1.0f, -1.0f,  1.0f, 1.0f );      // Front Right CCW
		motor_temp[2] = PIDMIXFLIGHT(  1.0f,  1.0f,  1.0f, 1.0f );      // Rear Left   CCW
		motor_temp[3] = PIDMIXFLIGHT(  1.0f, -1.0f, -1.0f, 1.0f );      // Front Left  CW

		float maxDeltaThrottle;
		float minDeltaThrottle;
		float deltaThrottle;

		maxDeltaThrottle = (float)MAXCOMMAND - rxCommands[THROTTLE];
		minDeltaThrottle = rxCommands[THROTTLE] - eepromConfig.minThrottle;
		deltaThrottle    = (minDeltaThrottle < maxDeltaThrottle) ? minDeltaThrottle : maxDeltaThrottle;

		for (i = 0; i < numberMotor; i++)
		{
			motor_value[i] = constrain16(motor_temp[i], rxCommands[THROTTLE] - deltaThrottle, rxCommands[THROTTLE] + deltaThrottle) - 1953;

			if ((rxCommands[THROTTLE]) < eepromConfig.minCheck)
				motor_value[i] = eepromConfig.minThrottle - 1952;

		}
	}
	else if((mode == ROVER) &&
			(armed == true) &&
			(failsafe == false))
	{

		motor_temp[0] = PIDMIXROVER(  1.0f,  1.0f);	// front left
		motor_temp[1] = PIDMIXROVER( -1.0f,  1.0f);	// front right

		motor_value[0] = 0;
		motor_value[1] = dshot3dFormat(motor_temp[1]);
		motor_value[2] = 0;
		motor_value[3] = dshot3dFormat(motor_temp[0]);
	}
	else
	{
		motor_value[0] = 0;
		motor_value[1] = 0;
		motor_value[2] = 0;
		motor_value[3] = 0;
	}
}
