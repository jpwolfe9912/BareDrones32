/**
 * @file flight_types.h
 * @author Jeremy Wolfe
 * @brief Defines and variables for tracking flight related parameters
 * @date 11 OCT 2026
 * 
 */

#ifndef __FLIGHT_TYPES_H__
#define __FLIGHT_TYPES_H__

#define ROLL     0
#define PITCH    1
#define YAW      2
#define THROTTLE 3

#define XAXIS    0
#define YAXIS    1
#define ZAXIS    2

#define MINCOMMAND  2000
#define MIDCOMMAND  3000
#define MAXCOMMAND  4000

typedef struct sensors_t
{
	float	accel[3];
	float	gyro[3];
	float	attitude[3];
	float	attDeg[3];
} sensors_t;

extern sensors_t sensors;

typedef enum
{
	RATE,
	ANGLE
}flightModes_e;

extern flightModes_e flightMode;

enum
{
	RC_AIL,
	RC_ELE,
	RC_THR,
	RC_RUD,
	RC_AUX1,
	RC_AUX2,
	RC_AUX3,
	RC_AUX4,
	RC_AUX5,
	RC_AUX6,
	RC_AUX7,
	RC_AUX8
};

#endif