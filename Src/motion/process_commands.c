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

flightModes_e flightMode = ANGLE;
semaphore_t armed = false;
uint8_t armingTimer    = 0;
uint8_t disarmingTimer = 0;

float rxCommands[8];

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
		rxCommands[ROLL]  = (ibusChannels[ROLL ] * 2) - MIDCOMMAND;	// Roll Range  -1000:1000
		rxCommands[PITCH] = (ibusChannels[PITCH] * 2) - MIDCOMMAND;	// Pitch Range -1000:1000
		rxCommands[YAW]   = (ibusChannels[YAW  ] * 2) - MIDCOMMAND;	// Yaw Range   -1000:1000

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
	if(rxCommands[AUX1] < MIDCOMMAND)
	{
		zeroPIDstates();
		armed = false;
		disarmingTimer = 0;

		// Check for gyro bias command ( low throttle, left yaw, aft pitch, right roll )
		if ( (rxCommands[YAW  ] < (eepromConfig.minCheck - MIDCOMMAND)) &&		//mincheck = 2200
				(rxCommands[ROLL ] > (eepromConfig.maxCheck - MIDCOMMAND)) &&	//maxcheck = 3800
				(rxCommands[PITCH] < (eepromConfig.minCheck - MIDCOMMAND)) )
		{
			delay(100);
			mpu6000Calibration();
		}

		if((rxCommands[YAW] < (eepromConfig.minCheck - MIDCOMMAND)) &&
				(rxCommands[ROLL] > (eepromConfig.maxCheck - MIDCOMMAND)) &&
				(rxCommands[PITCH] > (eepromConfig.maxCheck - MIDCOMMAND)))
		{
			delay(100);
			initPIDvalues();
		}
		if((rxCommands[YAW] > (eepromConfig.minCheck - MIDCOMMAND)) &&
				(rxCommands[ROLL ] < (eepromConfig.maxCheck - MIDCOMMAND)) &&	//maxcheck = 3800
				(rxCommands[PITCH] < (eepromConfig.minCheck - MIDCOMMAND)) )
		{
			delay(100);
			computeMPU6000RTData();
		}
	}

	/*		Check for arm switch and throttle low(<2200)	*/
	if((rxCommands[AUX1] > MIDCOMMAND) &&
	   (rxCommands[THROTTLE] < eepromConfig.minCheck) &&
	   (armed == false))
	{
		zeroPIDstates();
		armed = true;
	}

	/* Check for Flight Mode Change */
	if(rxCommands[AUX3] > MIDCOMMAND)
		flightMode = RATE;
	else
		flightMode = ANGLE;

	///////////////////////////////////

	// Check for armed true and throttle command > minThrottle

	if ((armed == true) && (rxCommands[THROTTLE] > eepromConfig.minThrottle))
		pidReset = false;
	else
		pidReset = true;
}

