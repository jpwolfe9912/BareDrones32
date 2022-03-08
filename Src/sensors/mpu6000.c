/** @file 		mpu6000.c
 *  @brief
 *  	This file contains all the functions to initalize and read data from the MPU-6000.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		06 MAR 2022
 *  @bug
 */

/* Includes */
#include "board.h"

/* Global Variables */
float accelOneG = 9.8065;

uint8_t rawData[16];

// Accel
int32_t accelSum100Hz[3] = { 0, 0, 0 };

int32_t accelSum500Hz[3] = { 0, 0, 0 };

int32_t accelSummedSamples100Hz[3];

int32_t accelSummedSamples500Hz[3];

float accelTCBias[3] = { 0.0f, 0.0f, 0.0f };

int16andUint8_t rawAccel[3];

float nonRotatedAccelData[3];

// Gyro

float gyroRTBias[3];

int32_t gyroSum500Hz[3] = { 0, 0, 0 };

int32_t gyroSummedSamples500Hz[3];

float gyroTCBias[3];

int16andUint8_t rawGyro[3];

float nonRotatedGyroData[3];

// Calibration

uint8_t accelCalibrating = false;

uint8_t mpu6000Calibrating = false;

// Temperature

float   mpu6000Temperature;

int16andUint8_t rawMPU6000Temperature;

uint8_t whoami[3];

/** @brief MPU-6000 initialization sequence
 *
 *  @return bool True if initialization was successful.
 *  			 False if unsuccessful.
 */
bool
mpu6000Init(void)
{
    ///////////////////////////////////
	printf("\nInitializing MPU-6000\n");
	SPI1->CR1	&= ~SPI_CR1_BR;
	SPI1->CR1 	|= SPI_BR_PRESCALER_128;		// BR < 1MHz for init

	SPI1_DISABLE;

	spiWriteOneByte(0x00, 0x00);
	delayMicroseconds(100);

	spiWriteOneByte(MPU6000_PWR_MGMT_1, BIT_H_RESET);	// reset device configuration
	delay(150);		// 100ms delay after reset

	spiWriteOneByte(MPU6000_SIGNAL_PATH_RESET, 0x07);
	delay(150);

	spiWriteOneByte(MPU6000_PWR_MGMT_1, MPU_CLK_SEL_PLLGYROZ);
	delayMicroseconds(100);

	spiReadOneByte(MPU6000_WHOAMI, whoami);
	delayMicroseconds(100);
	if(whoami[2] != 0x68){
		color(RED, YES);
		printf("\nFailed to read device ID. Would you like to retry?\n");
		color(WHITE, NO);
		if(serialWaitFor('y')){
			return false;
		}
	}
	else{
		color(GREEN, YES);
		printf("\nDevice ID recognized as 0x%X\n", whoami[2]);
		color(WHITE, NO);
	}
	delayMicroseconds(100);

	spiWriteOneByte(MPU6000_USER_CTRL, BIT_I2C_IF_DIS);	// disable I2C interface
	delayMicroseconds(100);

	spiWriteOneByte(MPU6000_PWR_MGMT_2, 0x00);
	delayMicroseconds(100);

	spiWriteOneByte(MPU6000_SMPLRT_DIV, 0x00);
	delayMicroseconds(100);

	spiWriteOneByte(MPU6000_GYRO_CONFIG, BITS_FS_2000DPS);
	delayMicroseconds(100);

	spiWriteOneByte(MPU6000_ACCEL_CONFIG, BITS_FS_16G);;
	delayMicroseconds(100);

	spiWriteOneByte(MPU6000_INT_PIN_CFG, 0x10);
	delayMicroseconds(100);

	///////////////////////////////////
	SPI1->CR1	&= ~SPI_CR1_BR;
	SPI1->CR1 	|= SPI_BR_PRESCALER_16;		// sensor BR < 20MHz

	delay(100);

#ifdef STLINK
	printf("\nDo you want to calibrate the MPU6000? \'y\' or \'n\'\n");
	if(serialWaitFor('y')){
		mpu6000Calibration();
	}

	printf("\nDo you want to calibrate the accelerometer? \'y\' or \'n\'\n");
	if(serialWaitFor('y')){
		accelCalibrationMPU();
	}
#endif

	computeMPU6000RTData();
	return true;
}

/** @brief Reads MPU-6000 raw data and packs accel, gyro, temp data into variables
 *
 *  @return Void.
 */
void
readMPU6000(void)
{
	spiReadBytes(MPU6000_ACCEL_XOUT_H, rawData, 16);

    rawAccel[XAXIS].bytes[1]		= 	rawData[2];
    rawAccel[XAXIS].bytes[0]		= 	rawData[3];
    rawAccel[YAXIS].bytes[1]		= 	rawData[4];
    rawAccel[YAXIS].bytes[0]		= 	rawData[5];
    rawAccel[ZAXIS].bytes[1]		= 	rawData[6];
    rawAccel[ZAXIS].bytes[0]		= 	rawData[7];

    rawMPU6000Temperature.bytes[1]	=	rawData[8];
    rawMPU6000Temperature.bytes[0]	= 	rawData[9];

    rawGyro[ROLL ].bytes[1]			= 	rawData[10];
    rawGyro[ROLL ].bytes[0]			= 	rawData[11];
    rawGyro[PITCH].bytes[1]			= 	rawData[12];
    rawGyro[PITCH].bytes[0]			= 	rawData[13];
    rawGyro[YAW  ].bytes[1]			= 	rawData[14];
    rawGyro[YAW  ].bytes[0]			= 	rawData[15];
}

/** @brief Computes IMU runtime data to find gyro bias.
 *
 *  @return Void.
 */
void
computeMPU6000RTData(void)
{
    uint8_t  axis;
    uint16_t samples;

    float accelSum[3]    = { 0.0f, 0.0f, 0.0f };
    float gyroSum[3]     = { 0.0f, 0.0f, 0.0f };

    mpu6000Calibrating = true;

    for (samples = 0; samples < 5000; samples++)
    {
        readMPU6000();

        computeMPU6000TCBias();

       	accelSum[XAXIS] += ((float)rawAccel[XAXIS].value - eepromConfig.accelBiasMPU[XAXIS] - accelTCBias[XAXIS]) * eepromConfig.accelScaleFactorMPU[XAXIS];
       	accelSum[YAXIS] += ((float)rawAccel[YAXIS].value - eepromConfig.accelBiasMPU[YAXIS] - accelTCBias[YAXIS]) * eepromConfig.accelScaleFactorMPU[YAXIS];
       	accelSum[ZAXIS] += ((float)rawAccel[ZAXIS].value - eepromConfig.accelBiasMPU[ZAXIS] - accelTCBias[ZAXIS]) * eepromConfig.accelScaleFactorMPU[ZAXIS];

        gyroSum[ROLL ]  += (float)rawGyro[ROLL ].value  - gyroTCBias[ROLL ];
        gyroSum[PITCH]  += (float)rawGyro[PITCH].value  - gyroTCBias[PITCH];
        gyroSum[YAW  ]  += (float)rawGyro[YAW  ].value  - gyroTCBias[YAW  ];

        delayMicroseconds(100);
    }

    for (axis = 0; axis < 3; axis++)
    {
        accelSum[axis]   = accelSum[axis] / 5000.0f;
        gyroRTBias[axis] = gyroSum[axis]  / 5000.0f;
    }

    accelOneG = sqrt(SQR(accelSum[XAXIS]) + SQR(accelSum[YAXIS]) + SQR(accelSum[ZAXIS]));

    mpu6000Calibrating = false;
}

/** @brief Calculates the IMU temperature bias.
 *
 *  @return Void.
 */
void
computeMPU6000TCBias(void)
{
    mpu6000Temperature = (float) (rawMPU6000Temperature.value) / 340.0f + 35.0f;

    accelTCBias[XAXIS] = eepromConfig.accelTCBiasSlope[XAXIS] * mpu6000Temperature + eepromConfig.accelTCBiasIntercept[XAXIS];
    accelTCBias[YAXIS] = eepromConfig.accelTCBiasSlope[YAXIS] * mpu6000Temperature + eepromConfig.accelTCBiasIntercept[YAXIS];
    accelTCBias[ZAXIS] = eepromConfig.accelTCBiasSlope[ZAXIS] * mpu6000Temperature + eepromConfig.accelTCBiasIntercept[ZAXIS];

    gyroTCBias[ROLL ]  = eepromConfig.gyroTCBiasSlope[ROLL ]  * mpu6000Temperature + eepromConfig.gyroTCBiasIntercept[ROLL ];
    gyroTCBias[PITCH]  = eepromConfig.gyroTCBiasSlope[PITCH]  * mpu6000Temperature + eepromConfig.gyroTCBiasIntercept[PITCH];
    gyroTCBias[YAW  ]  = eepromConfig.gyroTCBiasSlope[YAW  ]  * mpu6000Temperature + eepromConfig.gyroTCBiasIntercept[YAW  ];
}
