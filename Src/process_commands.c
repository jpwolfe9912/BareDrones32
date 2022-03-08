/** @file 		process_commands.c
 *  @brief
 *  	This file processes the receiver inputs to ensure they are in the correct
 *  	ranges for further computations.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

/* Includes */
#include "board.h"

/* Global Variables */
uint8_t  commandInDetent[3]         = { true, true, true };
uint8_t  previousCommandInDetent[3] = { true, true, true };

uint8_t mode;
semaphore_t armed = false;
uint8_t armingTimer    = 0;
uint8_t disarmingTimer = 0;

int16_t rxCommands[8];

/** @brief Processes receiver commands.
 *
 *  @return Void.
 */
void
processCommands(void)
{
	uint8_t channel;
	uint8_t channelsToRead = 8;

	if ( rcActive == true )
	{

		/* Makes RPY from -1000 to 1000 */
		rxCommands[ROLL]  = (ibusChannels[ROLL] * 2)  - MIDCOMMAND;	// Roll Range  -1000:1000
		rxCommands[PITCH] = (ibusChannels[PITCH] * 2) - MIDCOMMAND;	// Pitch Range -1000:1000
		rxCommands[YAW]   = (ibusChannels[YAW] * 2)   - MIDCOMMAND;	// Yaw Range   -1000:1000

		/* Makes all other channels from 2000 to 4000 */
		for (channel = 3; channel < channelsToRead; channel++)
			rxCommands[channel] = ibusChannels[channel] * 2;
	}

	// Set past command in detent values
	for (channel = 0; channel < 3; channel++)
		previousCommandInDetent[channel] = commandInDetent[channel];

	// Apply deadbands and set detent discretes'
	for (channel = 0; channel < 3; channel++)
	{
		/* RPY is within deadband */
		if ((rxCommands[channel] <= DEADBAND) && (rxCommands[channel] >= -DEADBAND))
		{
			rxCommands[channel] = 0;			// set command to 0
			commandInDetent[channel] = true;	// command IS within detent
		}
		else
		{
			commandInDetent[channel] = false;
			if (rxCommands[channel] > 0)
			{
				rxCommands[channel] = (rxCommands[channel] - DEADBAND) * DEADBAND_SLOPE;
			}
			else
			{
				rxCommands[channel] = (rxCommands[channel] + DEADBAND) * DEADBAND_SLOPE;
			}
		}
	}

	///////////////////////////////////

	/*		Check for disarm switch	*/
	if(rxCommands[AUX1] < MIDCOMMAND){
		zeroPIDstates();
		armed = false;
		disarmingTimer = 0;

		// Check for gyro bias command ( low throttle, left yaw, aft pitch, right roll )
		if ( (rxCommands[YAW  ] < (eepromConfig.minCheck - MIDCOMMAND)) &&		//mincheck = 2200
				(rxCommands[ROLL ] > (eepromConfig.maxCheck - MIDCOMMAND)) &&	//maxcheck = 3800
				(rxCommands[PITCH] < (eepromConfig.minCheck - MIDCOMMAND)) )
		{
			computeMPU6000RTData();
//			pulseMotors(3);
		}
	}

	/*		Check for arm switch and throttle low(<2200)	*/
	if((rxCommands[AUX1] > MIDCOMMAND) &&
			(rxCommands[THROTTLE] < eepromConfig.minCheck) &&
			(armed == false)){
		zeroPIDstates();
		armed = true;
	}


	///////////////////////////////////

	// Check for armed true and throttle command > minThrottle

	if ((armed == true) && (rxCommands[THROTTLE] > eepromConfig.minThrottle))
		pidReset = false;
	else
		pidReset = true;

	///////////////////////////////////

	// Check AUX1 for rate, attitude, or GPS mode (3 Position Switch) NOT COMPLETE YET....

	if ((rxCommands[AUX2] > MIDCOMMAND) &&
			(mode == ROVER) &&
			(armed == false))
	{
		mode = FLIGHT;
		setPIDstates(ROLL_ATT_PID,  0.0f);
		setPIDstates(PITCH_ATT_PID, 0.0f);
		setPIDstates(HEADING_PID,   0.0f);
	}
	else if ((rxCommands[AUX2] <= MIDCOMMAND) &&
			(mode == FLIGHT) &&
			(armed == false))
	{
		mode = ROVER;
	}
}

