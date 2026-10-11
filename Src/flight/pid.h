/** @file 		pid.h
 *  @brief
 *  	This files initializes and updates the PID controller.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

#ifndef __PID_H__
#define __PID_H__

#include <stdint.h>
#include <stdbool.h>

/* Defines */
#define NUMBER_OF_PIDS		5

#define ROLL_RATE_PID		0
#define PITCH_RATE_PID		1
#define YAW_RATE_PID		2

#define ROLL_ATT_PID		3
#define PITCH_ATT_PID		4

 /* Global Structs */
typedef struct PIDconfig {
	float P;
	float I;
	float D;
	float limit;
} PIDconfig_t;

typedef struct PIDdata {
	float   integratorState;
	float   filterState;

	float pTerm;
	float iTerm;
	float dTerm;

	uint8_t prevResetState;
} PIDdata_t;

/* Global Variables */
extern PIDdata_t pidState[NUMBER_OF_PIDS];
extern uint8_t pidReset;

/* Function Prototypes */
void initPID(void);
float updatePID(float error, float deltaT, uint8_t reset, const PIDconfig_t* config, PIDdata_t* state);
void resetPID(void);

#endif /* __PID_H__ */
