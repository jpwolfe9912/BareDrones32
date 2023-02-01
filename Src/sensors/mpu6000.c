/** @file 		mpu6000.c
 *  @brief
 *  	This file contains all the functions to initalize and read data from the MPU-6000.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		06 MAR 2022
 */

/* Includes */
#include "board.h"

/* Global Variables */
float accelOneG = 9.8065;

uint8_t rawData[15];

// Accel
int32_t accelSum500Hz[3] = {0, 0, 0};

int32_t accelSummedSamples500Hz[3];

float accelTCBias[3] = {0.0f, 0.0f, 0.0f};

int16andUint8_t rawAccel[3];

float nonRotatedAccelData[3];

// Gyro

float gyroRTBias[3];

int32_t gyroSum500Hz[3] = {0, 0, 0};

int32_t gyroSummedSamples500Hz[3];

float gyroTCBias[3];

int16andUint8_t rawGyro[3];

float nonRotatedGyroData[3];

// Calibration

uint8_t accelCalibrating = false;

uint8_t mpu6000Calibrating = false;

// Temperature

float mpu6000Temperature;

int16andUint8_t rawMPU6000Temperature;

uint8_t whoami[2];

bool mpu6000Initialized = false;

uint8_t devIdReadAttempts = DEV_READ_ATTEMPTS;

/** @brief MPU-6000 initialization sequence
 *
 *  @return bool True if initialization was successful.
 *  			 False if unsuccessful.
 */
bool mpu6000Init(void)
{
    ///////////////////////////////////
    printf("\nInitializing MPU-6000\n");

    SPI1_DISABLE;

    spi1WriteOneByte(0x00, 0x00);
    delay(100);

    spi1WriteOneByte(MPU6000_PWR_MGMT_1, BIT_H_RESET); // reset device configuration
    delay(150);                                        // 100ms delay after reset

    spi1WriteOneByte(MPU6000_SIGNAL_PATH_RESET, 0x07);
    delay(150);

    spi1WriteOneByte(MPU6000_PWR_MGMT_1, MPU_CLK_SEL_PLLGYROZ);
    delay(10);

    /* Read Device ID */
    while (devIdReadAttempts--)
    {
        spi1ReadOneByte(MPU6000_WHOAMI, whoami);
        delayMicroseconds(15);
        if (whoami[1] == 0x68)
        {
            color(GREEN, YES);
            printf("\nDevice ID recognized as 0x%X\n", whoami[1]);
            mpu6000Initialized = true;
            color(WHITE, NO);
            break;
        }
        else if ((whoami[1] != 0x68) && (devIdReadAttempts == 0))
        {
            color(RED, YES);
            printf("\nFailed to read device ID. Would you like to retry?\n");
            color(WHITE, NO);
            if (serialWaitFor('y'))
            {
                devIdReadAttempts = DEV_READ_ATTEMPTS;
            }
            else
            {
                mpu6000Initialized = false;
                return mpu6000Initialized;
            }
        }
    }
    delayMicroseconds(100);

    spi1WriteOneByte(MPU6000_USER_CTRL, BIT_I2C_IF_DIS); // disable I2C interface
    delayMicroseconds(100);

    spi1WriteOneByte(MPU6000_PWR_MGMT_2, 0x00);
    delayMicroseconds(100);

    spi1WriteOneByte(MPU6000_SMPLRT_DIV, 0x00);
    delayMicroseconds(100);

    spi1WriteOneByte(MPU6000_GYRO_CONFIG, BITS_FS_2000DPS);
    delayMicroseconds(100);

    spi1WriteOneByte(MPU6000_ACCEL_CONFIG, BITS_FS_16G);

    delayMicroseconds(100);

    spi1WriteOneByte(MPU6000_CONFIG, 0x03);
    delayMicroseconds(100);

    spi1WriteOneByte(MPU6000_INT_PIN_CFG, 0x10);
    delayMicroseconds(100);

    ///////////////////////////////////
    SPI1->CR1 &= ~SPI_CR1_BR;
    SPI1->CR1 |= SPI_BR_PRESCALER_8; // sensor BR < 20MHz

    delay(100);

    computeMPU6000RTData();
    return mpu6000Initialized;
}

/** @brief Reads MPU-6000 raw data and packs accel, gyro, temp data into variables
 *
 *  @return Void.
 */
void readMPU6000(void)
{
    spi1ReadBytes(MPU6000_ACCEL_XOUT_H, rawData, 15);

    rawAccel[XAXIS].bytes[1] = rawData[1];
    rawAccel[XAXIS].bytes[0] = rawData[2];
    rawAccel[YAXIS].bytes[1] = rawData[3];
    rawAccel[YAXIS].bytes[0] = rawData[4];
    rawAccel[ZAXIS].bytes[1] = rawData[5];
    rawAccel[ZAXIS].bytes[0] = rawData[6];

    rawMPU6000Temperature.bytes[1] = rawData[7];
    rawMPU6000Temperature.bytes[0] = rawData[8];

    rawGyro[ROLL].bytes[1] = rawData[9];
    rawGyro[ROLL].bytes[0] = rawData[10];
    rawGyro[PITCH].bytes[1] = rawData[11];
    rawGyro[PITCH].bytes[0] = rawData[12];
    rawGyro[YAW].bytes[1] = rawData[13];
    rawGyro[YAW].bytes[0] = rawData[14];

    accelSum500Hz[XAXIS] += rawAccel[XAXIS].value;
    accelSum500Hz[YAXIS] += rawAccel[YAXIS].value;
    accelSum500Hz[ZAXIS] += rawAccel[ZAXIS].value;

    gyroSum500Hz[ROLL] += rawGyro[ROLL].value;
    gyroSum500Hz[PITCH] += rawGyro[PITCH].value;
    gyroSum500Hz[YAW] += rawGyro[YAW].value;
}

/** @brief Computes IMU runtime data to find gyro bias.
 *
 *  @return Void.
 */
void computeMPU6000RTData(void)
{
    uint8_t axis;
    uint16_t samples;

    float accelSum[3] = {0.0f, 0.0f, 0.0f};
    float gyroSum[3] = {0.0f, 0.0f, 0.0f};

    mpu6000Calibrating = true;

    printf("\nComputing Gyro Runtime Data...\n");

    for (samples = 0; samples < 5000; samples++)
    {
        readMPU6000();

        computeMPU6000TCBias();

        accelSum[XAXIS] += ((float)rawAccel[XAXIS].value - eepromConfig.accelBiasMPU[XAXIS] - accelTCBias[XAXIS]) * eepromConfig.accelScaleFactorMPU[XAXIS];
        accelSum[YAXIS] += ((float)rawAccel[YAXIS].value - eepromConfig.accelBiasMPU[YAXIS] - accelTCBias[YAXIS]) * eepromConfig.accelScaleFactorMPU[YAXIS];
        accelSum[ZAXIS] += ((float)rawAccel[ZAXIS].value - eepromConfig.accelBiasMPU[ZAXIS] - accelTCBias[ZAXIS]) * eepromConfig.accelScaleFactorMPU[ZAXIS];

        gyroSum[ROLL] += (float)rawGyro[ROLL].value - gyroTCBias[ROLL];
        gyroSum[PITCH] += (float)rawGyro[PITCH].value - gyroTCBias[PITCH];
        gyroSum[YAW] += (float)rawGyro[YAW].value - gyroTCBias[YAW];

        delayMicroseconds(100);
    }

    for (axis = 0; axis < 3; axis++)
    {
        accelSum[axis] = accelSum[axis] / 5000.0f;
        gyroRTBias[axis] = gyroSum[axis] / 5000.0f;
    }

    accelOneG = sqrt(SQR(accelSum[XAXIS]) + SQR(accelSum[YAXIS]) + SQR(accelSum[ZAXIS]));

    mpu6000Calibrating = false;
}

/** @brief Calculates the IMU temperature bias.
 *
 *  @return Void.
 */
void computeMPU6000TCBias(void)
{
    mpu6000Temperature = (float)(rawMPU6000Temperature.value) / 340.0f + 35.0f;

    accelTCBias[XAXIS] = eepromConfig.accelTCBiasSlope[XAXIS] * mpu6000Temperature + eepromConfig.accelTCBiasIntercept[XAXIS];
    accelTCBias[YAXIS] = eepromConfig.accelTCBiasSlope[YAXIS] * mpu6000Temperature + eepromConfig.accelTCBiasIntercept[YAXIS];
    accelTCBias[ZAXIS] = eepromConfig.accelTCBiasSlope[ZAXIS] * mpu6000Temperature + eepromConfig.accelTCBiasIntercept[ZAXIS];

    gyroTCBias[ROLL] = eepromConfig.gyroTCBiasSlope[ROLL] * mpu6000Temperature + eepromConfig.gyroTCBiasIntercept[ROLL];
    gyroTCBias[PITCH] = eepromConfig.gyroTCBiasSlope[PITCH] * mpu6000Temperature + eepromConfig.gyroTCBiasIntercept[PITCH];
    gyroTCBias[YAW] = eepromConfig.gyroTCBiasSlope[YAW] * mpu6000Temperature + eepromConfig.gyroTCBiasIntercept[YAW];
}
