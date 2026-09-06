/** @file 		rotations.c
 *  @brief
 *  	This file computes the body fixed rotation matrices for the 100 and 500Hz loops.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		10 MAR 2022
 */

/* Includes */
#include "board.h"

/** @brief Computes the body frame for 500Hz loop.
 *
 *  @return Void.
 */
void computeRotations500Hz(void)
{
    arm_matrix_instance_f32 a;
    arm_matrix_instance_f32 b;
    arm_matrix_instance_f32 x;

    for (uint8_t index = 0; index < 3; index++)
    {
        accelSummedSamples500Hz[index] = accelSum500Hz[index];
        accelSum500Hz[index] = 0;

        gyroSummedSamples500Hz[index] = gyroSum500Hz[index];
        gyroSum500Hz[index] = 0;
    }

    computeMPU6000TCBias();

    nonRotatedAccelData[XAXIS] = ((float)accelSummedSamples500Hz[XAXIS] * 0.5f - accelTCBias[XAXIS]) * ACCEL_SCALE_FACTOR;
    nonRotatedAccelData[YAXIS] = ((float)accelSummedSamples500Hz[YAXIS] * 0.5f - accelTCBias[YAXIS]) * ACCEL_SCALE_FACTOR;
    nonRotatedAccelData[ZAXIS] = ((float)accelSummedSamples500Hz[ZAXIS] * 0.5f - accelTCBias[ZAXIS]) * ACCEL_SCALE_FACTOR;

    arm_mat_init_f32(&a, 3, 3, (float *)mpuOrientationMatrix);

    arm_mat_init_f32(&b, 3, 1, (float *)nonRotatedAccelData);

    arm_mat_init_f32(&x, 3, 1, sensors.accel500Hz);

    arm_mat_mult_f32(&a, &b, &x);

    nonRotatedGyroData[ROLL] = ((float)gyroSummedSamples500Hz[ROLL] * 0.5f - gyroRTBias[ROLL] - gyroTCBias[ROLL]) * GYRO_SCALE_FACTOR;
    nonRotatedGyroData[PITCH] = ((float)gyroSummedSamples500Hz[PITCH] * 0.5f - gyroRTBias[PITCH] - gyroTCBias[PITCH]) * GYRO_SCALE_FACTOR;
    nonRotatedGyroData[YAW] = ((float)gyroSummedSamples500Hz[YAW] * 0.5f - gyroRTBias[YAW] - gyroTCBias[YAW]) * GYRO_SCALE_FACTOR;

    arm_mat_init_f32(&a, 3, 3, (float *)mpuOrientationMatrix);

    arm_mat_init_f32(&b, 3, 1, (float *)nonRotatedGyroData);

    arm_mat_init_f32(&x, 3, 1, sensors.gyro500Hz);

    arm_mat_mult_f32(&a, &b, &x);
}