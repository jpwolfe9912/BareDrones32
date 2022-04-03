/** @file 		madgwick.h
 *  @brief
 *  	This file gives the function to create our RPY values.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

#ifndef __SENSOR_FUSION_H__
#define __SENSOR_FUSION_H__

/* Defines */
#define sampleFreqDef   1000.0f          // sample frequency in Hz
#define betaDef         10.0f            // 2 * proportional gain

/* Function Prototypes */
void madgwickInit(void);
void updateIMU(float gx, float gy, float gz, float ax, float ay, float az);
void computeAngles(void);

/* Global Variables */
extern float beta;				// algorithm gain
extern float q0;
extern float q1;
extern float q2;
extern float q3;	// quaternion of sensor frame relative to auxiliary frame
extern float invSampleFreq;
extern float roll;
extern float pitch;
extern float yaw;
extern char anglesComputed;

/* Inline Functions */

/** @brief Computes roll angle in deg.
 *
 * 	@return float Roll.
 */
static inline float
getRoll(void)
{
	if (!anglesComputed) computeAngles();
	return roll * 57.29578f;
}

/** @brief Computes pitch angle in deg.
 *
 * 	@return float Pitch.
 */
static inline float
getPitch(void)
{
	if (!anglesComputed) computeAngles();
	return pitch * 57.29578f;
}

/** @brief Computes yaw angle in deg.
 *
 * 	@return float Yaw.
 */
static inline float
getYaw(void)
{
	if (!anglesComputed) computeAngles();
	return yaw * 57.29578f;
}

/** @brief Computes roll angle in rad.
 *
 * 	@return float Roll.
 */
static inline float
getRollRadians(void)
{
	if (!anglesComputed) computeAngles();
	return roll;
}

/** @brief Computes pitch angle in rad.
 *
 * 	@return float Pitch.
 */
static inline float
getPitchRadians(void)
{
	if (!anglesComputed) computeAngles();
	return pitch;
}

/** @brief Computes yaw angle in rad.
 *
 * 	@return float Yaw.
 */
static inline float
getYawRadians(void)
{
	if (!anglesComputed) computeAngles();
	return yaw;
}

#endif /* __SENSOR_FUSION_H__ */
