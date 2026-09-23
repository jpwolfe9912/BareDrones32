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
}

/** @brief Updates the PID states.
 *
 *  @param error Commanded changed minus measured change.
 *  @param deltaT Time between updates in sec.
 *  @param reset If the PID states should be reset.
 *  @param *PIDparameters Struct containing the parameters of the PID controller.
 *  @return float Output from the algorithm.
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

/** @brief Allows the user to change the PID values.
 *
 *  @return Void.
 */
void
initPIDvalues(void)
{
	uint8_t ID;
	bool again = true;

	systemReady = false;
	delay(100);

	color(CYAN, YES);
	printf("\nWould you like to change the PID states?\n");
	colorDefault();
	delay(1);
	while (again) {
		if (usart3WaitFor('y')) {
			eepromChanged = true;

			printf("\nWhich PID would you like to change?\n");
			printf("Roll Rate PID : 0\n");
			printf("Pitch Rate PID: 1\n");
			printf("Yaw Rate PID  : 2\n");
			printf("Roll Att PID  : 3\n");
			printf("Pitch Att PID : 4\n");
			usart3Read8(&ID);
			ID -= 48;
			color(YELLOW, YES);
			printf("\nConfiguring state %u\n", ID);
			delay(1);
			color(BLUE, YES);
			printf("Current States:\n");
			delay(1);
			colorDefault();
			printf("P: (%1.2f)\nI: (%1.2f)\nD: (%1.2f)\nLimit: (%1.2f)\n",
				   eepromConfig.PID[ID].P,
				   eepromConfig.PID[ID].I,
				   eepromConfig.PID[ID].D,
				   eepromConfig.PID[ID].limit);

			usart3ReadPID(&eepromConfig.PID[ID].P,
						  &eepromConfig.PID[ID].I,
						  &eepromConfig.PID[ID].D);
			if (ID < 2)
				eepromConfig.PID[ID].limit = 1000.0f * eepromConfig.PID[ID].P * PI / 180.0;
			else if (ID == 2)
				eepromConfig.PID[ID].limit = 1000.0f * eepromConfig.yawRateScaling * eepromConfig.PID[ID].P;
			else
				eepromConfig.PID[ID].limit = 1000.0f * eepromConfig.attitudeScaling * eepromConfig.PID[ID].P;

			color(GREEN, YES);
			printf("\nNew States:\n");
			colorDefault();
			printf("P: (%1.2f)\nI: (%1.2f)\nD: (%1.2f)\nLimit: (%1.2f)\n",
				   eepromConfig.PID[ID].P,
				   eepromConfig.PID[ID].I,
				   eepromConfig.PID[ID].D,
				   eepromConfig.PID[ID].limit);

			printf("\nWould you like to configure another state?\n");
			delay(10);
			if (usart3WaitFor('y')) {
				again = true;
			}
			else {
				again = false;
			}
		}
		else
			again = false;
	}
	if (eepromChanged)
	{
		color(GREEN, YES);
		printf("\nFinished Configuring PIDs\n");
		colorDefault();
		writeEEPROM();
	}
	colorDefault();
	systemReady = true;
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