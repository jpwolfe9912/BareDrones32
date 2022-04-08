/** @file 		logging.c
 *  @brief
 *  	This file logs data to the terminal if connected.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 APR 2022
 */

#include "board.h"

/** @brief Prints logging data based on which type of data you want.
 *
 *	@param logType The type of data you want.
 *  @return Void.
 */
void
printLog(uint16_t logType)
{
	if (logType == 1)
	{
		// Roll Loop
		printf("1,%9.4f,%1d,%9.4f,%9.4f,%9.4f,%9.4f,%9.4f,%9.4f\n",
				battVoltage,
				mode,
				rateCmd[ROLL],
				sensors.gyro500Hz[ROLL],
				ratePID[ROLL],
				attCmd[ROLL],
				sensors.attitude500Hz[ROLL],
				attPID[ROLL]);
	}

	if (logType == 2)
	{
		// Pitch Loop
		printf("2,%9.4f,%1d,%9.4f,%9.4f,%9.4f,%9.4f,%9.4f,%9.4f\n",
				battVoltage,
				mode,
				rateCmd[PITCH],
				sensors.gyro500Hz[PITCH],
				ratePID[PITCH],
				attCmd[PITCH],
				sensors.attitude500Hz[PITCH],
				attPID[PITCH]);
	}

	if (logType == 3)
	{
		// Yaw Loop
		printf("2,%9.4f,%1d,%9.4f,%9.4f,%9.4f\n",
				battVoltage,
				mode,
				rateCmd[YAW],
				sensors.gyro500Hz[YAW],
				ratePID[YAW]);
	}

	if (logType == 4)
	{
		// Sensors
		printf("3,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,\n",
				battVoltage,
				sensors.accel500Hz[XAXIS],
				sensors.accel500Hz[YAXIS],
				sensors.accel500Hz[ZAXIS],
				sensors.gyro500Hz[ROLL],
				sensors.gyro500Hz[PITCH],
				sensors.gyro500Hz[YAW],
				sensors.attitude500Hz[ROLL],
				sensors.attitude500Hz[PITCH],
				sensors.attitude500Hz[YAW]);

	}

	if (logType == 5)
	{
		printf("4,%9.4f,%u,%u,%u,%u,\n",
				battVoltage,
				motor_value[0],
				motor_value[1],
				motor_value[2],
				motor_value[3]);
	}
}
