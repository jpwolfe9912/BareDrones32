/** @file 		mpu6000_calibration.c
 *  @brief
 *  	This file is used to calibrate IMU.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		06 MAR 2022
 */

/* Includes */
#include "board.h"

/** @brief Calibrates the IMU.
 *
 *  @return Void.
 */
void
mpu6000Calibration(void)
{
    uint16_t sampleRate      = 100;
    uint16_t numberOfSamples = 2000;

    float accelBias1[3]       = { 0.0f, 0.0f, 0.0f };
    float gyroBias1[3]        = { 0.0f, 0.0f, 0.0f };
    float mpu6000Temperature1 = 0.0f;

    float accelBias2[3]       = { 0.0f, 0.0f, 0.0f };
    float gyroBias2[3]        = { 0.0f, 0.0f, 0.0f };
    float mpu6000Temperature2 = 0.0f;

    eepromConfig.accelScaleFactorMPU[XAXIS] = 0.004788403f;  // (1/8192) * 9.8065  (8192 LSB = 1 G)
    eepromConfig.accelScaleFactorMPU[YAXIS] = 0.004788403f;  // (1/8192) * 9.8065  (8192 LSB = 1 G)
    eepromConfig.accelScaleFactorMPU[ZAXIS] = 0.004788403f;  // (1/8192) * 9.8065  (8192 LSB = 1 G)

    uint16_t index;

    mpu6000Calibrating = true;

    printf("\nMPU6000 Calibration:\n");

    ///////////////////////////////////
    // Get samples at temperature1
    ///////////////////////////////////

    printf("\nBegin 1st MPU6000 Measurements...\n");
    for (index = 0; index < numberOfSamples; index++)
    {
        readMPU6000();

        rawAccel[ZAXIS].value = rawAccel[ZAXIS].value - 2048;

        accelBias1[XAXIS]    += rawAccel[XAXIS].value;
        accelBias1[YAXIS]    += rawAccel[YAXIS].value;
        accelBias1[ZAXIS]    += rawAccel[ZAXIS].value;
        gyroBias1[ROLL ]     += rawGyro[ROLL ].value;
        gyroBias1[PITCH]     += rawGyro[PITCH].value;
        gyroBias1[YAW  ]     += rawGyro[YAW  ].value;
        mpu6000Temperature1  += (float) (rawMPU6000Temperature.value) / 340.0f + 35.0f;

        delayMicroseconds(sampleRate);
    }

    accelBias1[XAXIS]   /= (float) numberOfSamples;
    accelBias1[YAXIS]   /= (float) numberOfSamples;
    accelBias1[ZAXIS]   /= (float) numberOfSamples;
    gyroBias1[ROLL ]    /= (float) numberOfSamples;
    gyroBias1[PITCH]    /= (float) numberOfSamples;
    gyroBias1[YAW  ]    /= (float) numberOfSamples;
    mpu6000Temperature1 /= (float) numberOfSamples;

    printf("\nGyro Temperature Reading: %6.2f", mpu6000Temperature1);

    printf("\n\nEnd 1st MPU6000 Measurements\n");

    ///////////////////////////////////
    // Time delay for temperature
    // Stabilization
    ///////////////////////////////////

    printf("\nWaiting for 10 seconds for MPU6000 temp to rise...\n");
    delay(3000);    // Number of mSec in 10 minutes

    ///////////////////////////////////
    // Get samples at temperature2
    ///////////////////////////////////

    printf("\nBegin 2nd MPU6000 Measurements...\n");
    for (index = 0; index < numberOfSamples; index++)
    {
        readMPU6000();

        rawAccel[ZAXIS].value = rawAccel[ZAXIS].value - 2048;

        accelBias2[XAXIS]    += rawAccel[XAXIS].value;
        accelBias2[YAXIS]    += rawAccel[YAXIS].value;
        accelBias2[ZAXIS]    += rawAccel[ZAXIS].value;
        gyroBias2[ROLL ]     += rawGyro[ROLL ].value;
        gyroBias2[PITCH]     += rawGyro[PITCH].value;
        gyroBias2[YAW  ]     += rawGyro[YAW  ].value;
        mpu6000Temperature2  += (float)(rawMPU6000Temperature.value) / 340.0f + 35.0f;

        delayMicroseconds(sampleRate);
    }

    accelBias2[XAXIS]   /= (float) numberOfSamples;
    accelBias2[YAXIS]   /= (float) numberOfSamples;
    accelBias2[ZAXIS]   /= (float) numberOfSamples;
    gyroBias2[ROLL ]    /= (float) numberOfSamples;
    gyroBias2[PITCH]    /= (float) numberOfSamples;
    gyroBias2[YAW  ]    /= (float) numberOfSamples;
    mpu6000Temperature2 /= (float) numberOfSamples;

    printf("\nGyro Temperature Reading: %6.2f", mpu6000Temperature2);

    printf("\n\nEnd 2nd MPU6000 Measurements\n");

    ///////////////////////////////////

    eepromConfig.accelTCBiasSlope[XAXIS]     = (accelBias2[XAXIS] - accelBias1[XAXIS]) / (mpu6000Temperature2 - mpu6000Temperature1);
    eepromConfig.accelTCBiasSlope[YAXIS]     = (accelBias2[YAXIS] - accelBias1[YAXIS]) / (mpu6000Temperature2 - mpu6000Temperature1);
    eepromConfig.accelTCBiasSlope[ZAXIS]     = (accelBias2[ZAXIS] - accelBias1[ZAXIS]) / (mpu6000Temperature2 - mpu6000Temperature1);

    eepromConfig.accelTCBiasIntercept[XAXIS] = accelBias2[XAXIS] - eepromConfig.accelTCBiasSlope[XAXIS] * mpu6000Temperature2;
    eepromConfig.accelTCBiasIntercept[YAXIS] = accelBias2[YAXIS] - eepromConfig.accelTCBiasSlope[YAXIS] * mpu6000Temperature2;
    eepromConfig.accelTCBiasIntercept[ZAXIS] = accelBias2[ZAXIS] - eepromConfig.accelTCBiasSlope[ZAXIS] * mpu6000Temperature2;

    eepromConfig.gyroTCBiasSlope[ROLL ]      = (gyroBias2[ROLL ] - gyroBias1[ROLL ]) / (mpu6000Temperature2 - mpu6000Temperature1);
    eepromConfig.gyroTCBiasSlope[PITCH]      = (gyroBias2[PITCH] - gyroBias1[PITCH]) / (mpu6000Temperature2 - mpu6000Temperature1);
    eepromConfig.gyroTCBiasSlope[YAW  ]      = (gyroBias2[YAW  ] - gyroBias1[YAW  ]) / (mpu6000Temperature2 - mpu6000Temperature1);

    eepromConfig.gyroTCBiasIntercept[ROLL ]  = gyroBias2[ROLL ] - eepromConfig.gyroTCBiasSlope[ROLL ] * mpu6000Temperature2;
    eepromConfig.gyroTCBiasIntercept[PITCH]  = gyroBias2[PITCH] - eepromConfig.gyroTCBiasSlope[PITCH] * mpu6000Temperature2;
    eepromConfig.gyroTCBiasIntercept[YAW  ]  = gyroBias2[YAW  ] - eepromConfig.gyroTCBiasSlope[YAW  ] * mpu6000Temperature2;

    ///////////////////////////////////

    printf("\nMPU6000 Calibration Complete.\n");

    mpu6000Calibrating = false;
}

///////////////////////////////////////////////////////////////////////////////
