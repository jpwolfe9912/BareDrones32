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
void computeRotations(void)
{
    arm_matrix_instance_f32 a;
    arm_matrix_instance_f32 b;
    arm_matrix_instance_f32 x;

    computeMPU6000TCBias();

    /* Orient raw accelerometer data */

    nonRotatedAccelData[XAXIS] = ((float)rawAccel[XAXIS].value - accelTCBias[XAXIS]) * ACCEL_SCALE_FACTOR;
    nonRotatedAccelData[YAXIS] = ((float)rawAccel[YAXIS].value - accelTCBias[YAXIS]) * ACCEL_SCALE_FACTOR;
    nonRotatedAccelData[ZAXIS] = ((float)rawAccel[ZAXIS].value - accelTCBias[ZAXIS]) * ACCEL_SCALE_FACTOR;

    arm_mat_init_f32(&a, 3, 3, (float *)mpuOrientationMatrix);

    arm_mat_init_f32(&b, 3, 1, (float *)nonRotatedAccelData);

    arm_mat_init_f32(&x, 3, 1, sensors.accel);

    arm_mat_mult_f32(&a, &b, &x);

    /* Orient raw gyro data */
    nonRotatedGyroData[XAXIS] = ((float)rawGyro[XAXIS].value - gyroRTBias[XAXIS] - gyroTCBias[XAXIS]) * GYRO_SCALE_FACTOR;
    nonRotatedGyroData[YAXIS] = ((float)rawGyro[YAXIS].value - gyroRTBias[YAXIS] - gyroTCBias[YAXIS]) * GYRO_SCALE_FACTOR;
    nonRotatedGyroData[ZAXIS] = ((float)rawGyro[ZAXIS].value - gyroRTBias[ZAXIS] - gyroTCBias[ZAXIS]) * GYRO_SCALE_FACTOR;

    arm_mat_init_f32(&a, 3, 3, (float *)mpuOrientationMatrix);

    arm_mat_init_f32(&b, 3, 1, (float *)nonRotatedGyroData);

    arm_mat_init_f32(&x, 3, 1, sensors.gyro);

    arm_mat_mult_f32(&a, &b, &x);
}