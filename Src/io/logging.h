/** @file 		logging.h
 *  @brief
 *  	This file logs data to the terminal if connected.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 APR 2022
 */

#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "feature_config.h"

 /* Structs */
typedef struct __attribute__((packed))
{
	uint16_t header;        // 0xDEAD
	uint16_t sequence;		// 0xBEEF

	uint32_t timeUs;

	uint8_t flightMode;
	uint8_t armed;
	uint16_t reserved;

	float battVoltage;

	uint16_t throttleCmd;
	uint16_t motorOutput[4];

	float rateCmd[3];
	float gyro[3];
	float ratePID[3];

	float attCmd[3];
	float attitude[3];
	float attPID[3];

	uint16_t footer;       // 0x5AA5

} wiredLogRecord_t;

_Static_assert(sizeof(wiredLogRecord_t) == 100,
			   "logRecord_t size does not match logger protocol");

typedef struct __attribute__((packed))
{
	uint32_t sequence;
	uint32_t timeUs;

	float rateCmd[3];
	float gyro[3];

	float rateP[3];
	float rateI[3];
	float rateD[3];

	uint16_t throttleCmd;
	uint16_t motor[4];

} flashLogRecord_t;

_Static_assert(sizeof(flashLogRecord_t) == 78,
			   "Unexpected flashLogRecord_t size");


/* Enumerations */
typedef enum
{
	ROLL_LOG,
	PITCH_LOG,
	YAW_LOG,
	MOTORS_LOG,
	SENSORS_LOG,
	SCHEDULER_LOG,
	BATT_LOG,
	NO_LOG = 100
}logs_t;

/* Defines */
#define LOG_SIZE		64

extern volatile bool flash_update_active;

/* Functions */
void wiredLoggerUpdate(void);
void loggerComplete(void);

void flashLoggerProcess(void);
void flashLoggerUpdate(void);

#endif /* __LOGGING_H__ */
