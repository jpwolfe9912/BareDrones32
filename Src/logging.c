/** @file 		logging.c
 *  @brief
 *  	This file logs data to the terminal if connected.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 APR 2022
 */

#include "board.h"

uint8_t log_count = 0;
char *logData = (char*)0x20011000 ;

/* Static Functions */
static void writeLog(char *pLog);

/** @brief Prints logging data to OpenLager based on which type of data you want.
 *
 *	@param logType The type of data you want.
 *  @return Void.
 */
void
printLog(logs_t logType)
{
#ifdef OPENLAGER

	if(logType < 3)
	{
		sprintf(logData, "%d %f %d %f %f %f %f %f %f\r",
				logType,
				battVoltage,
				mode,
				rateCmd[logType],
				sensors.gyro500Hz[logType],
				ratePID[logType],
				attCmd[logType],
				sensors.attitude500Hz[logType],
				attPID[logType]);
	}


	if (logType == 3)
	{
		sprintf(logData, "%d %f %d %u %u %u %u \r",
				logType,
				battVoltage,
				mode,
				motor_value[MOTOR1],
				motor_value[MOTOR2],
				motor_value[MOTOR3],
				motor_value[MOTOR4]);

	}
	writeLog(logData);

#else
	if (logType < 3)
	{
		// Roll Loop
		printf("1,%9.4f,%1d,%9.4f,%9.4f,%9.4f,%9.4f,%9.4f,%9.4f\n",
				battVoltage,
				mode,
				rateCmd[logType],
				sensors.gyro500Hz[logType],
				ratePID[logType],
				attCmd[logType],
				sensors.attitude500Hz[logType],
				attPID[logType]);
	}

	if (logType == 3)
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

	if (logType == 4)
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

/** @brief Loops through the log.
 *
 *  @param ch The character to send.
 *  @return Void.
 */
static void
writeLog(char *pLog)
{
	uint8_t lagerSize = strlen(pLog);
	usart6Write(pLog, lagerSize);
}
