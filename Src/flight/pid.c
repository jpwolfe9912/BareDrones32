/** @file 		pid.c
 *  @brief
 *  	This files initializes and updates the PID controller.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

 /* Includes */
#include "pid.h"

#include "drv_system.h"
#include "drv_usart3.h"
#include "drv_color.h"
#include "config.h"

/* Global Variables */
PIDdata_t pidState[NUMBER_OF_PIDS];
uint8_t pidReset = true;

/** @brief Initializes the PID states.
 *
 *  @return Void.
 */
void
initPID(void)
{
	printf("[INIT] PID................. ");
	uint8_t index;

	for (uint8_t i = 0; i < NUMBER_OF_PIDS; i++)
	{
		pidState[i].integratorState = 0.0f;
		pidState[i].filterState = 0.0f;
		pidState[i].pTerm = 0.0f;
		pidState[i].iTerm = 0.0f;
		pidState[i].dTerm = 0.0f;
		pidState[i].prevResetState = false;
	}

	printf("OK\n");
}

/**
 * @brief Updates the PID states.
 * 
 * @param error Commanded changed minus measured change.
 * @param deltaT Time between updates in sec.
 * @param reset If the PID states should be reset.
 * @param config Struct containing the parameters of the PID controller.
 * @param state Struct containing the current states of the PID controller.
 * @return float Output from the algorithm.
 */
float
updatePID(float error, float deltaT, uint8_t reset, const PIDconfig_t* config, PIDdata_t* state)
{
	float pidSum;
	float pidLimited;

	if ((reset == true) || (state->prevResetState == true))
	{
		state->integratorState = 0.0f;
		state->filterState = 0.0f;
	}

	state->pTerm = error * config->P;
	state->iTerm = state->integratorState;
	state->dTerm = ((error * config->D) - state->filterState) * 100.0f;

	pidSum = state->pTerm
		+ state->iTerm
		+ state->dTerm;

	if (pidSum > config->limit)
	{
		pidLimited = config->limit;		// if greater than max
	}
	else
	{
		pidLimited = -config->limit;

		if (!(pidSum < (-config->limit)))	// if less than min
		{
			pidLimited = pidSum;				// no change
		}
	}

	state->integratorState += ((error * config->I) + 100.0f * (pidLimited - pidSum)) * deltaT;

	state->filterState += deltaT * state->dTerm;

	if (reset == true)
		state->prevResetState = true;
	else
		state->prevResetState = false;

	return pidLimited;
}

/** @brief Reset the state of the PIDs.
 *
 *  @return Void.
 */
void
resetPID(void)
{
	for (uint8_t index = 0; index < NUMBER_OF_PIDS; index++) 
	{
		pidState[index].integratorState = 0.0f;
		pidState[index].filterState = 0.0f;
	}
}