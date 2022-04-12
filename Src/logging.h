/** @file 		logging.h
 *  @brief
 *  	This file logs data to the terminal if connected.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 APR 2022
 */

#ifndef __LOGGING_H__
#define __LOGGING_H__

/* Enumerations */
typedef enum
{
	ROLL_LOG,
	PITCH_LOG,
	YAW_LOG,
	MOTORS_LOG,
	SENSORS_LOG
}logs_t;

/* Defines */
#define LOG_SIZE		(12 * 9)

/* Functions */
void printLog(uint8_t logType);

#endif /* __LOGGING_H__ */
