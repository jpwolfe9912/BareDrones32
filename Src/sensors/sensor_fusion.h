/*
 * sensor_fusion.h
 *
 *  Created on: Feb 7, 2022
 *      Author: jeremywolfe
 */

#ifndef SRC_SENSORS_SENSOR_FUSION_H_
#define SRC_SENSORS_SENSOR_FUSION_H_

#define sampleFreqDef   500.0f          // sample frequency in Hz
#define betaDef         0.1f            // 2 * proportional gain

void madgwickInit(void);
void updateIMU(float gx, float gy, float gz, float ax, float ay, float az);
void computeAngles(void);

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

static inline float getRoll() {
	if (!anglesComputed) computeAngles();
	return roll * 57.29578f;
}
static inline float getPitch() {
	if (!anglesComputed) computeAngles();
	return pitch * 57.29578f;
}
static inline float getYaw() {
	if (!anglesComputed) computeAngles();
	return yaw * 57.29578f + 180.0f;
}
static inline float getRollRadians() {
	if (!anglesComputed) computeAngles();
	return roll;
}
static inline float getPitchRadians() {
	if (!anglesComputed) computeAngles();
	return pitch;
}
static inline float getYawRadians() {
	if (!anglesComputed) computeAngles();
	return yaw;
}

#endif /* SRC_SENSORS_SENSOR_FUSION_H_ */
