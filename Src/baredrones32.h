/** @file 		BareDrones32.h
 *  @brief
 *  	This file contains many of the global variables and settings for the drone.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

#ifndef __BAREDRONES32_H__
#define __BAREDRONES32_H__

#ifndef PI
#define PI  3.14159265358979f
#endif

#define TWO_PI (2.0f * PI)

#define D2R  (PI / 180.0f)

#define R2D  (180.0f / PI)

#define KNOTS2MPS 0.51444444f

#define EARTH_RADIUS 6371000f

#define SQR(x)  ((x) * (x))

#define CUBE(x)	((x) * (x) * (x))

///////////////////////////////////////////////////////////////////////////////

#define ROLL     0
#define PITCH    1
#define YAW      2
#define THROTTLE 3
#define AUX1     4
#define AUX2     5
#define AUX3     6
#define AUX4     7
#define AUX5     8
#define AUX6     9
#define AUX7     10
#define AUX8     11

#define XAXIS    0
#define YAXIS    1
#define ZAXIS    2

#define MINCOMMAND  2000
#define MIDCOMMAND  3000
#define MAXCOMMAND  4000

///////////////////////////////////////////////////////////////////////////////
// Misc Type Definitions
///////////////////////////////////////////////////////////////////////////////

typedef union {
	int16_t value;
	uint8_t bytes[2];
} int16andUint8_t;

typedef union {
	int32_t value;
	uint8_t bytes[4];
} int32andUint8_t;

typedef union {
	uint16_t value;
	uint8_t bytes[2];
} uint16andUint8_t;

typedef union {
	uint32_t value;
	uint8_t bytes[4];
} uint32andUint8_t;

///////////////////////////////////////

typedef volatile uint8_t semaphore_t;

///////////////////////////////////////////////////////////////////////////////
// Sensor Variables
///////////////////////////////////////////////////////////////////////////////

typedef struct sensors_t
{
	float	accel500Hz[3];
	float	accel100Hz[3];
	float	attitude500Hz[3];
	float	gyro500Hz[3];
	float	attDeg500Hz[3];
} sensors_t;

extern sensors_t sensors;

///////////////////////////////////////////////////////////////////////////////
// PID Definitions
///////////////////////////////////////////////////////////////////////////////

#define NUMBER_OF_PIDS		6

#define ROLL_RATE_PID		0
#define PITCH_RATE_PID		1
#define YAW_RATE_PID		2

#define ROLL_ATT_PID		3
#define PITCH_ATT_PID		4
#define HEADING_PID			5

///////////////////////////////////////////////////////////////////////////////
// Flight Modes
///////////////////////////////////////////////////////////////////////////////
typedef enum
{
	RATE,
	ANGLE
}flightModes_e;

///////////////////////////////////////////////////////////////////////////////
// MPU6000 DLPF Configurations
///////////////////////////////////////////////////////////////////////////////

enum { DLPF_256HZ, DLPF_188HZ, DLPF_98HZ, DLPF_42HZ };

///////////////////////////////////////////////////////////////////////////////
// EEPROM
///////////////////////////////////////////////////////////////////////////////

typedef struct eepromConfig_t
{
	///////////////////////////////////

	uint8_t version;

	float accelBiasMPU[3];          // Bias for MPU60x0 Accel
	float accelScaleFactorMPU[3];   // Scale factor for MPU60x0 Accel

	float accelTCBiasSlope[3];
	float accelTCBiasIntercept[3];

	float gyroTCBiasSlope[3];
	float gyroTCBiasIntercept[3];

	float accelCutoff;

	float KpAcc;

	float KiAcc;

	uint8_t dlpfSetting;

	uint8_t sensorOrientation;

	///////////////////////////////////

	double rateCoeffAlpha;
	float rateCoeffBravo;
	float yawRateScaling;

	float attitudeScaling;

	///////////////////////////////////

	float midCommand;
	float minCheck;
	float maxCheck;
	float minThrottle;
	float maxThrottle;

	///////////////////////////////////

	PIDdata_t PID[NUMBER_OF_PIDS];

	///////////////////////////////////

	uint16_t activeTelemetry;

	///////////////////////////////////

	float yawDirection;


} eepromConfig_t;

enum crcFlags { CRC_HistoryBad = 1 };

extern eepromConfig_t eepromConfig;


#endif /* __BAREDRONES32_H__ */
