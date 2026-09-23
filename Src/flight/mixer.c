/** @file 		mixer.c
 *  @brief
 *  	This file takes the output of the PID controller and assigns values to the motors.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

/* Includes */
#include "mixer.h"

#include "drv_system.h"
#include "utilities.h"
#include "process_commands.h"
#include "motors.h"

/* Global Variables */
uint8_t numberMotor = 4;
uint16_t throttleCmd;
int16_t steerCmd, speedCmd;
float motor_temp[4];

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

	if(armed == true)
	{
		motor_temp[0] = PIDMIXFLIGHT( -1.0f,  1.0f, -1.0f, 1.0f );      // Rear Right  CW
		motor_temp[1] = PIDMIXFLIGHT( -1.0f, -1.0f,  1.0f, 1.0f );      // Front Right CCW
		motor_temp[2] = PIDMIXFLIGHT(  1.0f,  1.0f,  1.0f, 1.0f );      // Rear Left   CCW
		motor_temp[3] = PIDMIXFLIGHT(  1.0f, -1.0f, -1.0f, 1.0f );      // Front Left  CW

		float maxDeltaThrottle;
		float minDeltaThrottle;
		float deltaThrottle;

		maxDeltaThrottle = eepromConfig.maxThrottle - throttleCmd;
		minDeltaThrottle = throttleCmd - eepromConfig.minThrottle;
		deltaThrottle    = (minDeltaThrottle < maxDeltaThrottle) ? minDeltaThrottle : maxDeltaThrottle;

		float dshotThrottleScale = (float)(DSHOT_MAX_THROTTLE - DSHOT_IDLE_THROTTLE) /
								   (float)(eepromConfig.maxThrottle - eepromConfig.minThrottle);

		for (i = 0; i < numberMotor; i++)
		{
			motor_temp[i] = constrain(motor_temp[i], throttleCmd - deltaThrottle, throttleCmd + deltaThrottle);

			motor_temp[i] = DSHOT_IDLE_THROTTLE + (motor_temp[i] - eepromConfig.minThrottle) 
						  * dshotThrottleScale;
			motor_value[i] = constrain16(motor_temp[i], DSHOT_IDLE_THROTTLE, DSHOT_MAX_THROTTLE);
		}
	}
	else
	{
		motor_value[MOTOR1] = 0;
		motor_value[MOTOR2] = 0;
		motor_value[MOTOR3] = 0;
		motor_value[MOTOR4] = 0;
	}
}
