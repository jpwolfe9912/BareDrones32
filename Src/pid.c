/** @file 		pid.c
 *  @brief
 *  	This files initializes and updates the PID controller.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

/* Includes */
#include "board.h"

/* Global Variables */
uint8_t pidReset = true;
#ifdef BAD_PID
fastPID attPIDdata;
fastPID ratePIDdata;
#endif


/** @brief Initializes the PID states.
 *
 *  @return Void.
 */
void
initPID(void)
{
	uint8_t index;

	for (index = 0; index < NUMBER_OF_PIDS; index++)
	{
		eepromConfig.PID[index].integratorState = 0.0f;
		eepromConfig.PID[index].filterState     = 0.0f;
		eepromConfig.PID[index].prevResetState  = false;
	}
#ifdef BAD_PID
	attPIDdata.Kp = 2.0;
	attPIDdata.Ki = 0.0;
	attPIDdata.Kd = 0.0;
	attPIDdata.integral_limit = 10000.0;
	attPIDdata.output_limit = 50000.0;

	ratePIDdata.Kp = 100.0;
	ratePIDdata.Ki = 0.0;
	ratePIDdata.Kd = 50.0;
	ratePIDdata.integral_limit = 10000.0;
	ratePIDdata.output_limit = 50000.0;
#endif
}

#ifndef BAD_PID
/** @brief Updates the PID states.
 *
 *  @param error Commanded changed minus measured change.
 *  @param deltaT Time between updates in sec.
 *  @param reset If the PID states should be reset.
 *  @param *PIDparameters Struct containing the parameters of the PID controller.
 *  @return float Output from the algorithm.
 */
float
updatePID(float error, float deltaT, uint8_t reset, struct PIDdata *PIDparameters)
{
    float dTerm;
    float pidSum;
    float pidLimited;

    if ((reset == true) || (PIDparameters->prevResetState == true))
    {
        PIDparameters->integratorState = 0.0f;
        PIDparameters->filterState     = 0.0f;
    }

    dTerm = ((error * PIDparameters->D) - PIDparameters->filterState) * 100.0f;

    pidSum = (error * PIDparameters->P) + PIDparameters->integratorState + dTerm;

    if (pidSum > PIDparameters->Limit)
    {
        pidLimited = PIDparameters->Limit;
    }
    else
    {
        pidLimited = -PIDparameters->Limit;

        if (!(pidSum < (-PIDparameters->Limit)))
        {
            pidLimited = pidSum;
        }
    }

    PIDparameters->integratorState += ((error * PIDparameters->I) + 100.0f * (pidLimited - pidSum)) * deltaT;

    PIDparameters->filterState += deltaT * dTerm;

    if (reset == true)
        PIDparameters->prevResetState = true;
    else
        PIDparameters->prevResetState = false;

    return pidLimited;
}

#else
float
updatePID(struct fastPID *pidnow, float actual, float target)
{

	pidnow->error = standardRadianFormat(actual - target);
	pidnow->integral = pidnow->integral + pidnow->error*pidnow->Ki + pidnow->last_error*pidnow->Ki;
	if(pidnow->integral > pidnow->integral_limit){
		pidnow->integral = pidnow->integral_limit;
	}
	if(pidnow->integral < -pidnow->integral_limit){
		pidnow->integral = -pidnow->integral_limit;
	}

	pidnow->derivative = pidnow->Kd * (pidnow->error - pidnow->last_error);
	pidnow->last_error = pidnow->error;

	pidnow->pid_output = pidnow->error*pidnow->Kp + pidnow->integral + pidnow->derivative;


	if (pidnow->pid_output>pidnow->output_limit){
		pidnow->pid_output = pidnow->output_limit;
	}if(pidnow->pid_output <-pidnow->output_limit){
		pidnow->pid_output = -pidnow->output_limit;
	}
	return pidnow->pid_output;

}
#endif

/** @brief Allows the user to change the PID values.
 *
 *  @return Void.
 */
void
initPIDvalues(void)
{
	uint8_t ID;
	bool again = true;

	color(CYAN, YES);
	printf("\nWould you like to change the PID states?\n");
	colorDefault();
	delay(1);
	while(again){
		if(serialWaitFor('y')){
			eepromChanged = true;

			printf("\nWhich PID would you like to change?\n");
			printf("Roll Rate PID : 0\n");
			printf("Pitch Rate PID: 1\n");
			printf("Yaw Rate PID  : 2\n");
			printf("Roll Att PID  : 3\n");
			printf("Pitch Att PID : 4\n");
			serialRead8(&ID);

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
					eepromConfig.PID[ID].Limit);

			serialReadPID(&eepromConfig.PID[ID].P,
					&eepromConfig.PID[ID].I,
					&eepromConfig.PID[ID].D);
			if(ID < 2)
				eepromConfig.PID[ID].Limit	= 1000.0f * eepromConfig.PID[ID].P * PI / 180.0;
			else if(ID == 2)
				eepromConfig.PID[ID].Limit	= 1000.0f * eepromConfig.yawRateScaling * eepromConfig.PID[ID].P;
			else
				eepromConfig.PID[ID].Limit	= 1000.0f * eepromConfig.attitudeScaling * eepromConfig.PID[ID].P;

			color(GREEN, YES);
			printf("\nNew States:\n");
			colorDefault();
			printf("P: (%1.2f)\nI: (%1.2f)\nD: (%1.2f)\nLimit: (%1.2f)\n",
					eepromConfig.PID[ID].P,
					eepromConfig.PID[ID].I,
					eepromConfig.PID[ID].D,
					eepromConfig.PID[ID].Limit);

			printf("\nWould you like to configure another state?\n");
			delay(1);
			if(serialWaitFor('y')){
				again = true;
			}
			else{
				again = false;
			}
		}
		else
			again = false;
	}
	color(GREEN, YES);
	printf("\nFinished Configuring PIDs\n");
	colorDefault();
	writeEEPROM();
}

/** @brief Set the state of the PIDs.
 *
 *	@param IDPid Which PID state you want to change.
 *	@param value The value to write.
 *  @return Void.
 */
void
setPIDstates(uint8_t IDPid, float value)
{
	eepromConfig.PID[IDPid].integratorState = value;
	eepromConfig.PID[IDPid].filterState     = value;
}

/** @brief Set all states to 0.
 *
 *  @return Void.
 */
void
zeroPIDstates(void)
{
	uint8_t index;

	for (index = 0; index < NUMBER_OF_PIDS; index++)
		setPIDstates(index, 0.0f);
}
