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

#include "baredrones32.h"

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

    uint8_t osdDisplayAlt;           // 1 = Display OSD Altitude
    uint8_t osdDisplayAltRow;
    uint8_t osdDisplayAltCol;
    uint8_t osdDisplayAltHoldState;  // 1 = display altitude hold state, 0 = don't display

    uint8_t osdDisplayAH;            // 1 = Display OSD Artificial Horizon
    uint8_t osdDisplayAtt;           // 1 = Display OSD Attitude

    uint8_t osdDisplayHdg;           // 1 = Display OSD Heading
    uint8_t osdDisplayHdgRow;
    uint8_t osdDisplayHdgCol;

    uint8_t osdDisplayHdgBar;	     // 1 = Display OSD Heading Bar (more visual)
    uint8_t osdDisplayHdgBarRow;
    uint8_t osdDisplayHdgBarCol;

    uint8_t osdDisplayVoltage;	     // 1 = Display OSD Voltage
    uint8_t osdDisplayVoltageRow;
    uint8_t osdDisplayVoltageCol;

    uint8_t osdDisplayCurrent;	     // 1 = Display OSD instantaneous current and used current
    uint8_t osdDisplayCurrentRow;
    uint8_t osdDisplayCurrentCol;

    uint8_t osdDisplayThrot;	     // 1 = Display OSD throttle - for now, just rx input, maybe some day show rx input and processed throttle (alt,gps,etc)
    uint8_t osdDisplayThrotRow;
    uint8_t osdDisplayThrotCol;

   	uint8_t osdDisplayRSSI;          // 1 = Display OSD RSSI
   	uint8_t osdDisplayRSSIRow;
   	uint8_t osdDisplayRSSICol;

    uint8_t osdDisplayTimer;
    uint8_t osdDisplayTimerRow;
    uint8_t osdDisplayTimerCol;

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

/////////
#endif
