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

/* Static Functions */
float pid_mix_flight(float mixX, float mixY, float mixZ, float mixT);

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
	float motor_buffer[4] = {};

	if (armed == true)
	{
		motor_buffer[0] = pid_mix_flight( 1.0f, -1.0f, -1.0f, 1.0f);      // Rear Right  CW
		motor_buffer[1] = pid_mix_flight( 1.0f,  1.0f,  1.0f, 1.0f);      // Front Right CCW
		motor_buffer[2] = pid_mix_flight(-1.0f, -1.0f,  1.0f, 1.0f);      // Rear Left   CCW
		motor_buffer[3] = pid_mix_flight(-1.0f,  1.0f, -1.0f, 1.0f);      // Front Left  CW

		float maxDeltaThrottle;
		float minDeltaThrottle;
		float deltaThrottle;

		maxDeltaThrottle = eepromConfig.maxThrottle - throttleCmd;
		minDeltaThrottle = throttleCmd - eepromConfig.minThrottle;
		deltaThrottle = (minDeltaThrottle < maxDeltaThrottle) ? minDeltaThrottle : maxDeltaThrottle;

		float dshotThrottleScale = (float)(DSHOT_MAX_THROTTLE - DSHOT_IDLE_THROTTLE) /
			(float)(eepromConfig.maxThrottle - eepromConfig.minThrottle);

		for (uint8_t i = 0; i < NUMBER_OF_MOTORS; i++)
		{
			motor_buffer[i] = constrain(motor_buffer[i], throttleCmd - deltaThrottle, throttleCmd + deltaThrottle);

			motor_buffer[i] = DSHOT_IDLE_THROTTLE + (motor_buffer[i] - eepromConfig.minThrottle) * dshotThrottleScale;

			motor_value[i] = constrain16(motor_buffer[i], DSHOT_IDLE_THROTTLE, DSHOT_MAX_THROTTLE);
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

float
pid_mix_flight(float mixX, float mixY, float mixZ, float mixT)
{
	float mix_output = ((ratePID[ROLL] * (mixX)) + 
						(ratePID[PITCH] * (mixY)) + 
						(ratePID[YAW] * (mixZ) * eepromConfig.yawDirection) + 
						(throttleCmd * (mixT)));
	return mix_output;
}