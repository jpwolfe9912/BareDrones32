/** @file 		rotations.c
 *  @brief
 *  	This file computes the body fixed rotation matrices for the 100 and 500Hz loops.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		10 MAR 2022
 */

/* Includes */
#include "rotations.h"

#include "arm_math.h"
#include "mpu6000.h"
#include "orientation.h"
#include "baredrones32.h"

/** @brief Computes the body frame for 500Hz loop.
 *
 *  @return Void.
 */
void computeRotations500Hz(void)
{
    arm_matrix_instance_f32 a;
    arm_matrix_instance_f32 b;
    arm_matrix_instance_f32 x;

    // for (uint8_t index = 0; index < 3; index++)
    // {
    //     accelSummedSamples500Hz[index] = accelSum500Hz[index];
    //     accelSum500Hz[index] = 0;

    //     gyroSummedSamples500Hz[index] = gyroSum500Hz[index];
    //     gyroSum500Hz[index] = 0;
    // }

    computeMPU6000TCBias();

    /* Orient raw accelerometer data */

    nonRotatedAccelData[XAXIS] = ((float)rawAccel[XAXIS].value - accelTCBias[XAXIS]) * ACCEL_SCALE_FACTOR;
    nonRotatedAccelData[YAXIS] = ((float)rawAccel[YAXIS].value - accelTCBias[YAXIS]) * ACCEL_SCALE_FACTOR;
    nonRotatedAccelData[ZAXIS] = ((float)rawAccel[ZAXIS].value - accelTCBias[ZAXIS]) * ACCEL_SCALE_FACTOR;

    // nonRotatedAccelData[XAXIS] = ((float)accelSummedSamples500Hz[XAXIS] * 0.5f - accelTCBias[XAXIS]) * ACCEL_SCALE_FACTOR;
    // nonRotatedAccelData[YAXIS] = ((float)accelSummedSamples500Hz[YAXIS] * 0.5f - accelTCBias[YAXIS]) * ACCEL_SCALE_FACTOR;
    // nonRotatedAccelData[ZAXIS] = ((float)accelSummedSamples500Hz[ZAXIS] * 0.5f - accelTCBias[ZAXIS]) * ACCEL_SCALE_FACTOR;

    arm_mat_init_f32(&a, 3, 3, (float *)mpuOrientationMatrix);

    arm_mat_init_f32(&b, 3, 1, (float *)nonRotatedAccelData);

    arm_mat_init_f32(&x, 3, 1, sensors.accel);

    arm_mat_mult_f32(&a, &b, &x);

    /* Orient raw gyro data */
    nonRotatedGyroData[XAXIS] = ((float)rawGyro[XAXIS].value - gyroRTBias[XAXIS] - gyroTCBias[XAXIS]) * GYRO_SCALE_FACTOR;
    nonRotatedGyroData[YAXIS] = ((float)rawGyro[YAXIS].value - gyroRTBias[YAXIS] - gyroTCBias[YAXIS]) * GYRO_SCALE_FACTOR;
    nonRotatedGyroData[ZAXIS] = ((float)rawGyro[ZAXIS].value - gyroRTBias[ZAXIS] - gyroTCBias[ZAXIS]) * GYRO_SCALE_FACTOR;

    // nonRotatedGyroData[ROLL ] = ((float)gyroSummedSamples500Hz[ROLL ] * 0.5f - gyroRTBias[ROLL ] - gyroTCBias[ROLL ]) * GYRO_SCALE_FACTOR;
    // nonRotatedGyroData[PITCH] = ((float)gyroSummedSamples500Hz[PITCH] * 0.5f - gyroRTBias[PITCH] - gyroTCBias[PITCH]) * GYRO_SCALE_FACTOR;
    // nonRotatedGyroData[YAW  ] = ((float)gyroSummedSamples500Hz[YAW  ] * 0.5f - gyroRTBias[YAW  ] - gyroTCBias[YAW. ]) * GYRO_SCALE_FACTOR;

    arm_mat_init_f32(&a, 3, 3, (float *)mpuOrientationMatrix);

    arm_mat_init_f32(&b, 3, 1, (float *)nonRotatedGyroData);

    arm_mat_init_f32(&x, 3, 1, sensors.gyro);

    arm_mat_mult_f32(&a, &b, &x);
}