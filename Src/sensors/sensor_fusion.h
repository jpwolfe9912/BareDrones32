/** @file 		sensor_fusion.h
 *  @brief
 *  	This file gives the function to create our RPY values.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 *  @bug
 */

#ifndef SRC_SENSORS_SENSOR_FUSION_H_
#define SRC_SENSORS_SENSOR_FUSION_H_

/* Defines */
#define sampleFreqDef   1000.0f          // sample frequency in Hz
#define betaDef         10.0f            // 2 * proportional gain

/* Function Prototypes */
void madgwickInit(void);
void updateIMU(float gx, float gy, float gz, float ax, float ay, float az);
void computeAngles(void);

/* Global Variables */
float beta;				// algorithm gain
float q0;
float q1;
float q2;
float q3;	// quaternion of sensor frame relative to auxiliary frame
float invSampleFreq;
float roll;
float pitch;
float yaw;
char anglesComputed;

/* Inline Functions */
static inline float
getRoll(void)
{
	if (!anglesComputed) computeAngles();
	return roll * 57.29578f;
}

static inline float
getPitch(void)
{
	if (!anglesComputed) computeAngles();
	return pitch * 57.29578f;
}

static inline float
getYaw(void)
{
	if (!anglesComputed) computeAngles();
	return yaw * 57.29578f + 180.0f;
}

static inline float
getRollRadians(void)
{
	if (!anglesComputed) computeAngles();
	return roll;
}

static inline float
getPitchRadians(void)
{
	if (!anglesComputed) computeAngles();
	return pitch;
}

static inline float
getYawRadians(void)
{
	if (!anglesComputed) computeAngles();
	return yaw;
}

#endif /* SRC_SENSORS_SENSOR_FUSION_H_ */
