/** @file 		config.h
 *  @brief
 *  	This file configures the EEPROM settings of the flight controller.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

 /////////

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>
#include <stdbool.h>

#include "pid.h"
#include "osd_widgets.h"

/* Defines */
#define FLASH_WRITE_EEPROM_ADDR  0x08060000  // FLASH_Sector_7

/* Structures */
typedef struct eepromConfig_t
{


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

	double rateCoeffAlpha;
	float rateCoeffBravo;
	float yawRateScaling;

	float attitudeScaling;

	float midCommand;
	float minCheck;
	float maxCheck;
	float minThrottle;
	float maxThrottle;

	PIDconfig_t PID[NUMBER_OF_PIDS];

	uint16_t activeTelemetry;

	float yawDirection;

	uint8_t osdEnabled;              // 0 = Disabled, 1 = Enabled
	uint8_t defaultVideoStandard;    // 0 = NTSC, 1 = PAL
	uint8_t metricUnits;             // 1 = metric

	uint8_t osdDisplayAH;            // 1 = Display OSD Artificial Horizon
	uint8_t osdDisplayAtt;           // 1 = Display OSD Attitude

	osdWidgetConfig_t osdLogo;

	osdWidgetConfig_t osdAlt;
	uint8_t osdDisplayAltHoldState;  // 1 = display altitude hold state, 0 = don't display

	osdWidgetConfig_t osdHdg;

	osdWidgetConfig_t osdHdgBar;

	osdWidgetConfig_t osdVoltage;

	osdWidgetConfig_t osdCurrent;

	osdWidgetConfig_t osdThrottle;

	osdWidgetConfig_t osdRSSI;

	osdWidgetConfig_t osdTimer;

} eepromConfig_t;

/* Global Variables */
extern eepromConfig_t eepromConfig;

/* Function Prototypes */
void readEEPROM(void);
void writeEEPROM(void);
void checkFirstTime(bool eepromReset);

/////////
#endif
