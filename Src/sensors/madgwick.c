/** @file 		madgwick.c
 *  @brief
 *  	This file gives the function to create our RPY values.
 *	Source for this function exists at:
 *		https://github.com/arduino-libraries/MadgwickAHRS
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

/* Defines */
#include "board.h"

/* Global Variables */
float beta; // algorithm gain
float q0;
float q1;
float q2;
float q3; // quaternion of sensor frame relative to auxiliary frame
float invSampleFreq;
float roll;
float pitch;
float yaw;
char anglesComputed;

/* Static Function Prototypes */
static float invSqrt(float x);

/** @brief Initializes quaternion and other variables.
 *
 * 	@return Void.
 */
void madgwickInit(void)
{
    printf("\nInitializing Madgwick Sensor Fusion\n");

    beta = betaDef;
    q0 = 1.0f;
    q1 = 0.0f;
    q2 = 0.0f;
    q3 = 0.0f;
    invSampleFreq = 1.0f / sampleFreqDef;
    anglesComputed = 0;
}

/** @brief Creates quaternion from gyro and accel values.
 *
 *  @return Void.
 */
void updateIMU(void)
{
    float gx = sensors.gyro500Hz[ROLL];
    float gy = sensors.gyro500Hz[PITCH];
    float gz = sensors.gyro500Hz[YAW];
    float ax = sensors.accel500Hz[XAXIS];
    float ay = sensors.accel500Hz[YAXIS];
    float az = sensors.accel500Hz[ZAXIS];

    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2, _8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

    // Rate of change of quaternion from gyroscope
    qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
    qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
    qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
    qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
    {

        // Normalise accelerometer measurement
        recipNorm = invSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // Auxiliary variables to avoid repeated arithmetic
        _2q0 = 2.0f * q0;
        _2q1 = 2.0f * q1;
        _2q2 = 2.0f * q2;
        _2q3 = 2.0f * q3;
        _4q0 = 4.0f * q0;
        _4q1 = 4.0f * q1;
        _4q2 = 4.0f * q2;
        _8q1 = 8.0f * q1;
        _8q2 = 8.0f * q2;
        q0q0 = q0 * q0;
        q1q1 = q1 * q1;
        q2q2 = q2 * q2;
        q3q3 = q3 * q3;

        // Gradient decent algorithm corrective step
        s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
        s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
        s2 = 4.0f * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
        s3 = 4.0f * q1q1 * q3 - _2q1 * ax + 4.0f * q2q2 * q3 - _2q2 * ay;
        recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;

        // Apply feedback step
        qDot1 -= beta * s0;
        qDot2 -= beta * s1;
        qDot3 -= beta * s2;
        qDot4 -= beta * s3;
    }

    // Integrate rate of change of quaternion to yield quaternion
    q0 += qDot1 * invSampleFreq;
    q1 += qDot2 * invSampleFreq;
    q2 += qDot3 * invSampleFreq;
    q3 += qDot4 * invSampleFreq;

    // Normalise quaternion
    recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= recipNorm;
    q1 *= recipNorm;
    q2 *= recipNorm;
    q3 *= recipNorm;
    anglesComputed = 0;
}

/** @brief Fast inverse square-root.
 *
 * 		See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
 *
 *	@param float x Input value.
 * 	@return float Inverse square-root.
 */
float invSqrt(float x)
{
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y = y * (1.5f - (halfx * y * y));
    y = y * (1.5f - (halfx * y * y));
    return y;
}

/** @brief Computes angles from quaternion
 *
 * 	@return Void.
 */
void computeAngles(void)
{
    roll = atan2f(q0 * q1 + q2 * q3, 0.5f - q1 * q1 - q2 * q2);
    pitch = asinf(-2.0f * (q1 * q3 - q0 * q2));
    yaw = atan2f(q1 * q2 + q0 * q3, 0.5f - q2 * q2 - q3 * q3);
    anglesComputed = 1;
}

void updateAttitude(void)
{
    sensors.attitude500Hz[ROLL] = getRollRadians();
    sensors.attitude500Hz[PITCH] = getPitchRadians();
    sensors.attitude500Hz[YAW] = getYawRadians();
}