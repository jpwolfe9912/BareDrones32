/*
  October 2012

  aq32Plus Rev -

  Copyright (c) 2012 John Ihlein.  All rights reserved.

  Open Source STM32 Based Multicopter Controller Software

  Includes code and/or ideas from:

  1)AeroQuad
  2)BaseFlight
  3)CH Robotics
  4)MultiWii
  5)S.O.H. Madgwick
  6)UAVX

  Designed to run on the AQ32 Flight Control Board

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

///////////////////////////////////////////////////////////////////////////////

#include "board.h"

///////////////////////////////////////////////////////////////////////////////
// Process Pilot Commands Defines and Variables
///////////////////////////////////////////////////////////////////////////////

uint8_t  commandInDetent[3]         = { true, true, true };
uint8_t  previousCommandInDetent[3] = { true, true, true };


extern uint16_t ibusChannels[RC_CHANNELS];
///////////////////////////////////////////////////////////////////////////////
// Flight Mode Defines and Variables
///////////////////////////////////////////////////////////////////////////////

uint8_t mode = FLIGHT;

uint8_t headingHoldEngaged     = false;

///////////////////////////////////////////////////////////////////////////////
// Arm State Variables
///////////////////////////////////////////////////////////////////////////////

semaphore_t armed          = false;
uint8_t     armingTimer    = 0;
uint8_t     disarmingTimer = 0;

///////////////////////////////////////////////////////////////////////////////
// Vertical Mode State Variables
///////////////////////////////////////////////////////////////////////////////

uint8_t  verticalModeState = ALT_DISENGAGED_THROTTLE_ACTIVE;

uint16_t previousAUX2State = MINCOMMAND;
uint16_t previousAUX4State = MINCOMMAND;

uint8_t  vertRefCmdInDetent         = true;
uint8_t  previousVertRefCmdInDetent = true;

float    verticalReferenceCommand;

///////////////////////////////////////////////////////////////////////////////
// Read Flight Commands
///////////////////////////////////////////////////////////////////////////////
int16_t rxCommands[8];

void processFlightCommands(void)
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

	///////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////




