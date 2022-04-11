/** @file 		logging.c
 *  @brief
 *  	This file logs data to the terminal if connected.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 APR 2022
 */

#include "board.h"

uint8_t *pLog;

/** @brief Prints logging data to OpenLager based on which type of data you want.
 *
 *	@param logType The type of data you want.
 *  @return Void.
 */
void
printLog(logs_t logType)
{
#ifdef OPENLAGER
	float log[10];
	pLog = (uint8_t*)&log;

	log[0] = logType;

	if(logType < 3)
	{
		log[1] = battVoltage;
		log[2] = mode;
		log[3] = rateCmd[logType];
		log[4] = sensors.gyro500Hz[logType];
		log[5] = ratePID[logType];
		log[6] = attCmd[logType];
		log[7] = sensors.attitude500Hz[logType];
		log[8] = attPID[logType];
	}


	if (logType == 3)
	{
		log[1] = battVoltage;
		log[2] = mode;
		log[3] = motor_value[MOTOR1];
		log[4] = motor_value[MOTOR2];
		log[5] = motor_value[MOTOR3];
		log[6] = motor_value[MOTOR4];
	}
	lagerWriteLog(pLog);

#else
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
#endif
}
