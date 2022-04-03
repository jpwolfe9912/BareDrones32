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

/* Static Variables */
static uint8_t checkNewEEPROMConf = 29;

/** @brief CRC for the EEPROM
 *
 *  @return uint32_t CRC value.
 */
//uint32_t crc32bEEPROM(eepromConfig_t *e, int includeCRCAtEnd)
//{
//    return crc32B((uint32_t*)e, includeCRCAtEnd ? (uint32_t*)(e + 1) : e->CRCAtEnd);
//}
//
/////////////////////////////////////////////////////////////////////////////////
//
//enum { eepromConfigNUMWORD =  sizeof(eepromConfig_t)/sizeof(uint32_t) };
//
//void readEEPROM(void)
//{
//    eepromConfig_t *dst = &eepromConfig;
//
//    *dst = *(eepromConfig_t*)FLASH_WRITE_EEPROM_ADDR ;
//
//    if ( crcCheckVal != crc32bEEPROM(dst, true) )
//    {
//        evrPush(EVR_FlashCRCFail,0);
//        dst->CRCFlags |= CRC_HistoryBad;
//    }
//    else if ( dst->CRCFlags & CRC_HistoryBad )
//      evrPush(EVR_ConfigBadHistory,0);
//
//    accConfidenceDecay = 1.0f / sqrt(eepromConfig.accelCutoff);
//
//    eepromConfig.yawDirection = constrain(eepromConfig.yawDirection, -1.0f, 1.0f);
//}
//
/////////////////////////////////////////////////////////////////////////////////
//
//int writeEEPROM(void)
//{
//    FLASH_Status status;
//    int32_t i;
//
//    eepromConfig_t *src = &eepromConfig;
//    uint32_t       *dst = (uint32_t*)FLASH_WRITE_EEPROM_ADDR;
//
//    // there's no reason to write these values to EEPROM, they'll just be noise
//    zeroPIDstates();
//
//    if ( src->CRCFlags & CRC_HistoryBad )
//        evrPush(EVR_ConfigBadHistory,0);
//
//    src->CRCAtEnd[0] = crc32B( (uint32_t*)&src[0], src->CRCAtEnd);
//
//    FLASH_Unlock();
//
//    FLASH_ClearFlag(FLASH_FLAG_EOP    | FLASH_FLAG_OPERR  | FLASH_FLAG_WRPERR |
//                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
//
//    status = FLASH_EraseSector(FLASH_Sector_1, VoltageRange_3);
//
//    ///////////////////////////////////
//
//    i = -1;
//
//    while ( FLASH_COMPLETE == status && i++ < eepromConfigNUMWORD )
//        status = FLASH_ProgramWord((uint32_t)&dst[i], ((uint32_t*)src)[i]);
//
//    if ( FLASH_COMPLETE != status )
//        evrPush( -1 == i ? EVR_FlashEraseFail : EVR_FlashProgramFail, status);
//
//    ///////////////////////////////////
//
//    FLASH_Lock();
//
//    readEEPROM();
//
//    return status;
//}

/** @brief Sets all the default config values
 *
 *	@param bool eepromReset Whether or not you want to write config values to EEPROM
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

		eepromConfig.magBias[XAXIS] = 0.0f;      // Internal Mag Bias
		eepromConfig.magBias[YAXIS] = 0.0f;      // Internal Mag Bias
		eepromConfig.magBias[ZAXIS] = 0.0f;      // Internal Mag Bias

		eepromConfig.magBias[XAXIS + 3] = 0.0f;  // External Mag Bias
		eepromConfig.magBias[YAXIS + 3] = 0.0f;  // External Mag Bias
		eepromConfig.magBias[ZAXIS + 3] = 0.0f;  // External Mag Bias

		///////////////////////////////

		eepromConfig.accelCutoff = 0.25f;

		///////////////////////////////

		eepromConfig.KpAcc = 1.0f;    // proportional gain governs rate of convergence to accelerometer
		eepromConfig.KiAcc = 0.0f;    // integral gain governs rate of convergence of gyroscope biases
		eepromConfig.KpMag = 5.0f;    // proportional gain governs rate of convergence to magnetometer
		eepromConfig.KiMag = 0.0f;    // integral gain governs rate of convergence of gyroscope biases


		///////////////////////////////

		eepromConfig.dlpfSetting = BITS_DLPF_CFG_98HZ;

		///////////////////////////////////

		eepromConfig.sensorOrientation = 1;  // No rotation

		///////////////////////////////////

		eepromConfig.rollAndPitchRateScaling = 100.0 / 180000.0 * PI;  // Stick to rate scaling for 100 DPS
		eepromConfig.yawRateScaling          = 100.0 / 180000.0 * PI;  // Stick to rate scaling for 100 DPS

		eepromConfig.attitudeScaling         = 60.0  / 180000.0 * PI;  // Stick to att scaling for 60 degrees

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
		eepromConfig.PID[ROLL_RATE_PID].Limit            = 1000.0f * eepromConfig.rollAndPitchRateScaling * eepromConfig.PID[ROLL_RATE_PID].P ;
		eepromConfig.PID[ROLL_RATE_PID].integratorState  =    0.0f;
		eepromConfig.PID[ROLL_RATE_PID].filterState      =    0.0f;
		eepromConfig.PID[ROLL_RATE_PID].prevResetState   =   false;

		eepromConfig.PID[PITCH_RATE_PID].P               =  250.0f;
		eepromConfig.PID[PITCH_RATE_PID].I               =  100.0f;
		eepromConfig.PID[PITCH_RATE_PID].D               =    0.0f;
		eepromConfig.PID[PITCH_RATE_PID].Limit           = 1000.0f * eepromConfig.rollAndPitchRateScaling * eepromConfig.PID[PITCH_RATE_PID].P;
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

		eepromConfig.osdEnabled             =  true;
//		eepromConfig.defaultVideoStandard   =  PAL;
		eepromConfig.metricUnits            =  true;

		eepromConfig.osdDisplayAlt          =  true;
		eepromConfig.osdDisplayAltRow       =  15;
		eepromConfig.osdDisplayAltCol       =  1;
		eepromConfig.osdDisplayAltHoldState =  false;

		eepromConfig.osdDisplayAH           =  false; // DO NOT SET BOTH TO TRUE
		eepromConfig.osdDisplayAtt          =  true;

		eepromConfig.osdDisplayHdg          =  false;
		eepromConfig.osdDisplayHdgRow       =  1;
		eepromConfig.osdDisplayHdgCol       =  13;

		eepromConfig.osdDisplayHdgBar		=  true;
		eepromConfig.osdDisplayHdgBarRow	=  14;
		eepromConfig.osdDisplayHdgBarCol	=  10;

		eepromConfig.osdDisplayVoltage		=  true;
		eepromConfig.osdDisplayVoltageRow	=  1;
		eepromConfig.osdDisplayVoltageCol	=  1;

		eepromConfig.osdDisplayCurrent		=  true;
		eepromConfig.osdDisplayCurrentRow	=  1;
		eepromConfig.osdDisplayCurrentCol	=  8;

		eepromConfig.osdDisplayThrot		=  true;
		eepromConfig.osdDisplayThrotRow		=  15;
		eepromConfig.osdDisplayThrotCol		=  25;

		eepromConfig.osdDisplayTimer		=  true;
		eepromConfig.osdDisplayTimerRow		=  0;
		eepromConfig.osdDisplayTimerCol		=  23;

		eepromConfig.osdDisplayRSSI			=  true;
		eepromConfig.osdDisplayRSSIRow		=  1;
		eepromConfig.osdDisplayRSSICol		=  24;

		///////////////////////////////

		eepromConfig.activeTelemetry          = 5;

//		writeEEPROM();
	}
}

///////////////////////////////////////////////////////////////////////////////
