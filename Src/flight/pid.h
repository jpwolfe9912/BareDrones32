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

#include "baredrones32.h"

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
extern PIDdata_t pidState[5];
extern uint8_t pidReset;

/* Function Prototypes */
void initPID(void);
float updatePID(float error, float deltaT, uint8_t reset, const PIDconfig_t* config, PIDdata_t* state);
void initPIDvalues(void);
void resetPID(void);

#endif /* __PID_H__ */
