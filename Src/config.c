/** @file 		config.c
 *  @brief
 *  	This file configures the EEPROM settings of the flight controller.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

/* Includes */
#include "board.h"

/* Global Variabels */
const char rcChannelLetters[] = "AERT12345678";

semaphore_t eepromChanged = false;

/* Static Variables */
static uint8_t checkNewEEPROMConf = 29;

void readEEPROM(void)
{
	eepromConfig_t *dst = &eepromConfig;

	*dst = *(eepromConfig_t*)FLASH_WRITE_EEPROM_ADDR;
}

///////////////////////////////////////////////////////////////////////////////

HAL_StatusTypeDef
writeEEPROM(void)
{
	color(BLUE, YES);
	printf("\nWriting to EEPROM...\n");

	int32_t i;

	HAL_StatusTypeDef status;

	FLASH_EraseInitTypeDef erase;

	erase.TypeErase = FLASH_TYPEERASE_SECTORS;
	erase.NbSectors = 1;
	erase.Sector = FLASH_SECTOR_6;
	erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	uint32_t err = 0;

	eepromConfig_t *src = &eepromConfig;
	uint32_t       *dst = (uint32_t*)FLASH_WRITE_EEPROM_ADDR;

	// there's no reason to write these values to EEPROM, they'll just be noise

	HAL_FLASH_Unlock();

	status = HAL_FLASHEx_Erase(&erase, &err);

	///////////////////////////////////

	i = -1;

	while (i++ < eepromConfigNUMWORD )
		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)&dst[i], ((uint32_t*)src)[i]);

	///////////////////////////////////

	HAL_FLASH_Lock();

	readEEPROM();

	if(status == HAL_OK)
		color(GREEN, YES);
	else
		color(RED, YES);

	printf("\nEEPROM write complete. Return status of %X\n", status);
	colorDefault();
	return status;
}

/** @brief Sets all the default config values
 *
 *	@param bool eepromReset Whether or not you want to write config values to EEPROM.
 *  @return Void.
 */
void
checkFirstTime(bool eepromReset)
{
	uint8_t test_val;

	test_val = *(uint8_t *)FLASH_WRITE_EEPROM_ADDR;

	if (eepromReset || (test_val != checkNewEEPROMConf))
	{
		// Default settings
		eepromConfig.version = checkNewEEPROMConf;

		///////////////////////////////

		eepromConfig.accelBiasMPU[XAXIS] = 0.0f;
		eepromConfig.accelBiasMPU[YAXIS] = 0.0f;
		eepromConfig.accelBiasMPU[ZAXIS] = 0.0f;

		///////////////////////////////

		eepromConfig.accelScaleFactorMPU[XAXIS] = 0.00119708f;  // (1/8192) * 9.8065  (8192 LSB = 1 G)
		eepromConfig.accelScaleFactorMPU[YAXIS] = 0.00119708f;  // (1/8192) * 9.8065  (8192 LSB = 1 G)
		eepromConfig.accelScaleFactorMPU[ZAXIS] = 0.00119708f;  // (1/8192) * 9.8065  (8192 LSB = 1 G)

		///////////////////////////////

		eepromConfig.accelTCBiasSlope[XAXIS] = 0.0f;
		eepromConfig.accelTCBiasSlope[YAXIS] = 0.0f;
		eepromConfig.accelTCBiasSlope[ZAXIS] = 0.0f;

		///////////////////////////////

		eepromConfig.accelTCBiasIntercept[XAXIS] = 0.0f;
		eepromConfig.accelTCBiasIntercept[YAXIS] = 0.0f;
		eepromConfig.accelTCBiasIntercept[ZAXIS] = 0.0f;

		///////////////////////////////

		eepromConfig.gyroTCBiasSlope[ROLL ] = 0.0f;
		eepromConfig.gyroTCBiasSlope[PITCH] = 0.0f;
		eepromConfig.gyroTCBiasSlope[YAW  ] = 0.0f;

		///////////////////////////////

		eepromConfig.gyroTCBiasIntercept[ROLL ] = 0.0f;
		eepromConfig.gyroTCBiasIntercept[PITCH] = 0.0f;
		eepromConfig.gyroTCBiasIntercept[YAW  ] = 0.0f;

		///////////////////////////////

		eepromConfig.accelCutoff = 0.25f;

		///////////////////////////////

		eepromConfig.KpAcc = 1.0f;    // proportional gain governs rate of convergence to accelerometer
		eepromConfig.KiAcc = 0.0f;    // integral gain governs rate of convergence of gyroscope biases

		///////////////////////////////

		eepromConfig.dlpfSetting = BITS_DLPF_CFG_98HZ;

		///////////////////////////////////

		eepromConfig.sensorOrientation = 1;  // No rotation

		///////////////////////////////////

		eepromConfig.rateCoeffAlpha 		= 8.5e-7;  // Stick to rate scaling for 100 DPS
		eepromConfig.rateCoeffBravo			= 0.15;
		eepromConfig.yawRateScaling			= 500.0 / 180000.0 * PI;  // Stick to rate scaling for 500 DPS

		eepromConfig.attitudeScaling         = 30.0  / 180000.0 * PI;  // Stick to att scaling for 30 degrees

		///////////////////////////////

		eepromConfig.yawDirection       = 1.0f;		// TODO check this value

		eepromConfig.midCommand   = 3000.0f;
		eepromConfig.minCheck     = (float)(MINCOMMAND + 200);
		eepromConfig.maxCheck     = (float)(MAXCOMMAND - 200);
		eepromConfig.minThrottle  = (float)(MINCOMMAND + 200);
		eepromConfig.maxThrottle  = (float)(MAXCOMMAND);

		///////////////////////////////

		eepromConfig.PID[ROLL_RATE_PID].P                =  250.0f;
		eepromConfig.PID[ROLL_RATE_PID].I                =  100.0f;
		eepromConfig.PID[ROLL_RATE_PID].D                =    0.0f;
		eepromConfig.PID[ROLL_RATE_PID].Limit            = 1000.0f * eepromConfig.PID[ROLL_RATE_PID].P * PI / 180.0;
		eepromConfig.PID[ROLL_RATE_PID].integratorState  =    0.0f;
		eepromConfig.PID[ROLL_RATE_PID].filterState      =    0.0f;
		eepromConfig.PID[ROLL_RATE_PID].prevResetState   =   false;

		eepromConfig.PID[PITCH_RATE_PID].P               =  250.0f;
		eepromConfig.PID[PITCH_RATE_PID].I               =  100.0f;
		eepromConfig.PID[PITCH_RATE_PID].D               =    0.0f;
		eepromConfig.PID[PITCH_RATE_PID].Limit           = 1000.0f * eepromConfig.PID[PITCH_RATE_PID].P * PI / 180.0;
		eepromConfig.PID[PITCH_RATE_PID].integratorState =    0.0f;
		eepromConfig.PID[PITCH_RATE_PID].filterState     =    0.0f;
		eepromConfig.PID[PITCH_RATE_PID].prevResetState  =   false;

		eepromConfig.PID[YAW_RATE_PID].P                 =  350.0f;
		eepromConfig.PID[YAW_RATE_PID].I                 =  100.0f;
		eepromConfig.PID[YAW_RATE_PID].D                 =    0.0f;
		eepromConfig.PID[YAW_RATE_PID].Limit             =  1000.0f * eepromConfig.yawRateScaling * eepromConfig.PID[YAW_RATE_PID].P;
		eepromConfig.PID[YAW_RATE_PID].integratorState   =    0.0f;
		eepromConfig.PID[YAW_RATE_PID].filterState       =    0.0f;
		eepromConfig.PID[YAW_RATE_PID].prevResetState    =   false;

		eepromConfig.PID[ROLL_ATT_PID].P                 =    2.0f;
		eepromConfig.PID[ROLL_ATT_PID].I                 =    0.0f;
		eepromConfig.PID[ROLL_ATT_PID].D                 =    0.0f;
		eepromConfig.PID[ROLL_ATT_PID].Limit             = 1000.0f * eepromConfig.attitudeScaling * eepromConfig.PID[ROLL_ATT_PID].P;
		eepromConfig.PID[ROLL_ATT_PID].integratorState   =    0.0f;
		eepromConfig.PID[ROLL_ATT_PID].filterState       =    0.0f;
		eepromConfig.PID[ROLL_ATT_PID].prevResetState    =   false;

		eepromConfig.PID[PITCH_ATT_PID].P                =    2.0f;
		eepromConfig.PID[PITCH_ATT_PID].I                =    0.0f;
		eepromConfig.PID[PITCH_ATT_PID].D                =    0.0f;
		eepromConfig.PID[PITCH_ATT_PID].Limit            = 1000.0f * eepromConfig.attitudeScaling * eepromConfig.PID[PITCH_ATT_PID].P;
		eepromConfig.PID[PITCH_ATT_PID].integratorState  =    0.0f;
		eepromConfig.PID[PITCH_ATT_PID].filterState      =    0.0f;
		eepromConfig.PID[PITCH_ATT_PID].prevResetState   =   false;

		///////////////////////////////

		eepromConfig.activeTelemetry          = 5;

		writeEEPROM();
	}
}

///////////////////////////////////////////////////////////////////////////////
