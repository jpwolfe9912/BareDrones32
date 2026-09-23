/** @file 		config.h
 *  @brief
 *  	This file configures the EEPROM settings of the flight controller.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

///////////////////////////////////////////////////////////////////////////////

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>
#include <stdbool.h>

#include "baredrones32.h"

/* Defines */
#define FLASH_WRITE_EEPROM_ADDR  0x08060000  // FLASH_Sector_7

/* Structures */
typedef struct eepromConfig_t
{
	///////////////////////////////////

	uint8_t version;

	float accelBiasMPU[3];          // Bias for MPU60x0 Accel
	float accelScaleFactorMPU[3];   // Scale factor for MPU60x0 Accel

	float accelTCBiasSlope[3];
	float accelTCBiasIntercept[3];

	float gyroTCBiasSlope[3];
	float gyroTCBiasIntercept[3];

	float accelCutoff;

	float KpAcc;

	float KiAcc;

	uint8_t dlpfSetting;

	uint8_t sensorOrientation;

	///////////////////////////////////

	double rateCoeffAlpha;
	float rateCoeffBravo;
	float yawRateScaling;

	float attitudeScaling;

	///////////////////////////////////

	float midCommand;
	float minCheck;
	float maxCheck;
	float minThrottle;
	float maxThrottle;

	///////////////////////////////////

	PIDconfig_t PID[NUMBER_OF_PIDS];

	///////////////////////////////////

	uint16_t activeTelemetry;

	///////////////////////////////////

	float yawDirection;


} eepromConfig_t;

/* Enumerations */
enum { eepromConfigNUMWORD =  sizeof(eepromConfig_t)/sizeof(uint32_t) };

/* Global Variables */
extern const char rcChannelLetters[12];

extern semaphore_t eepromChanged;

extern eepromConfig_t eepromConfig;

/* Function Prototypes */
void readEEPROM(void);
void writeEEPROM(void);
void checkFirstTime(bool eepromReset);

///////////////////////////////////////////////////////////////////////////////
#endif
