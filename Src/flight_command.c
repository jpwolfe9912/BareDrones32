///*
//  October 2012
//
//  aq32Plus Rev -
//
//  Copyright (c) 2012 John Ihlein.  All rights reserved.
//
//  Open Source STM32 Based Multicopter Controller Software
//
//  Includes code and/or ideas from:
//
//  1)AeroQuad
//  2)BaseFlight
//  3)CH Robotics
//  4)MultiWii
//  5)S.O.H. Madgwick
//  6)UAVX
//
//  Designed to run on the AQ32 Flight Control Board
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program. If not, see <http://www.gnu.org/licenses/>.
// */
//
/////////////////////////////////////////////////////////////////////////////////
//
//#include "board.h"
//
/////////////////////////////////////////////////////////////////////////////////
//// Process Pilot Commands Defines and Variables
/////////////////////////////////////////////////////////////////////////////////
//
//float    ibusChannels[12] = { 0.0f, 0.0f, 0.0f, 2000.0f, 2000.0f, 2000.0f, 2000.0f, 2000.0f, 2000.0f, 2000.0f, 2000.0f, 2000.0f };
//
//uint8_t  commandInDetent[3]         = { true, true, true };
//uint8_t  previousCommandInDetent[3] = { true, true, true };
//
//
//extern uint16_t ibusChannels[RC_CHANNELS];
/////////////////////////////////////////////////////////////////////////////////
//// Flight Mode Defines and Variables
/////////////////////////////////////////////////////////////////////////////////
//
//uint8_t flightMode = RATE;
//
//uint8_t headingHoldEngaged     = false;
//
/////////////////////////////////////////////////////////////////////////////////
//// Arm State Variables
/////////////////////////////////////////////////////////////////////////////////
//
//semaphore_t armed          = false;
//uint8_t     armingTimer    = 0;
//uint8_t     disarmingTimer = 0;
//
/////////////////////////////////////////////////////////////////////////////////
//// Vertical Mode State Variables
/////////////////////////////////////////////////////////////////////////////////
//
//uint8_t  verticalModeState = ALT_DISENGAGED_THROTTLE_ACTIVE;
//
//uint16_t previousAUX2State = MINCOMMAND;
//uint16_t previousAUX4State = MINCOMMAND;
//
//uint8_t  vertRefCmdInDetent         = true;
//uint8_t  previousVertRefCmdInDetent = true;
//
//float    verticalReferenceCommand;
//
/////////////////////////////////////////////////////////////////////////////////
//// Read Flight Commands
/////////////////////////////////////////////////////////////////////////////////
//
//void processFlightCommands(void)
//{
//	uint8_t channel;
//	uint8_t channelsToRead = 8;
//
//	float hdgDelta, simpleX, simpleY;
//
//	if ( rcActive == true )
//	{
////		// Read receiver commands
////		if (eepromConfig.receiverType == PPM)
////			channelsToRead = eepromConfig.ppmChannels;
////
////		for (channel = 0; channel < channelsToRead; channel++)
////		{
////			if (eepromConfig.receiverType == SPEKTRUM)
////				ibusChannels[channel] = spektrumRead(eepromConfig.rcMap[channel]);
////			else if (eepromConfig.receiverType == SBUS)
////				ibusChannels[channel] = sBusRead(eepromConfig.rcMap[channel]);
////			else
////				ibusChannels[channel] = rxRead(eepromConfig.rcMap[channel]);
////		}
//
////		ibusChannels[ROLL]  -= eepromConfig.midCommand;  // Roll Range  -1000:1000
////		ibusChannels[PITCH] -= eepromConfig.midCommand;  // Pitch Range -1000:1000
////		ibusChannels[YAW]   -= eepromConfig.midCommand;  // Yaw Range   -1000:1000
//
//		ibusChannels[ROLL]  = (ibusChannels[ROLL] * 2) - 2000;	// Roll Range  -1000:1000
//		ibusChannels[PITCH] = (ibusChannels[PITCH] * 2) - 2000;	// Pitch Range -1000:1000
//		ibusChannels[YAW]   = (ibusChannels[YAW] * 2) - 2000;	// Yaw Range   -1000:1000
//
////		for (channel = 3; channel < channelsToRead; channel++)
////			ibusChannels[channel] -= eepromConfig.midCommand - MIDCOMMAND;  // Range 2000:4000
//		for (channel = 3; channel < channelsToRead; channel++)
//			ibusChannels[channel] *= 2;
//	}
//
//	// Set past command in detent values
//	for (channel = 0; channel < 3; channel++)
//		previousCommandInDetent[channel] = commandInDetent[channel];
//
//	// Apply deadbands and set detent discretes'
//	for (channel = 0; channel < 3; channel++)
//	{
//		if ((ibusChannels[channel] <= DEADBAND) && (ibusChannels[channel] >= -DEADBAND))
//		{
//			ibusChannels[channel] = 0;
//			commandInDetent[channel] = true;
//		}
//		else
//		{
//			commandInDetent[channel] = false;
//			if (ibusChannels[channel] > 0)
//			{
//				ibusChannels[channel] = (ibusChannels[channel] - DEADBAND) * DEADBAND_SLOPE;
//			}
//			else
//			{
//				ibusChannels[channel] = (ibusChannels[channel] + DEADBAND) * DEADBAND_SLOPE;
//			}
//		}
//	}
//
//	///////////////////////////////////
//
//	/*		Check for disarm switch	*/
//	if(ibusChannels[AUX1] < MIDCOMMAND){
//		zeroPIDstates();
//		armed = false;
//		disarmingTimer = 0;
//	}
//	/*		Check for arm switch and throttle low	*/
//	if((ibusChannels[AUX1] > MIDCOMMAND) && (ibusChannels[THROTTLE] < eepromConfig.minCheck) && (armed == false)){
//		zeroPIDstates();
//		armed = true;
//	}
//
//	// Check for low throttle
//	if ( ibusChannels[THROTTLE] < eepromConfig.minCheck )
//	{
//		// Check for disarm command ( low throttle, left yaw )
//		if ( (ibusChannels[YAW] < (eepromConfig.minCheck - MIDCOMMAND)) && (armed == true) )
//		{
//			disarmingTimer++;
//
//			if (disarmingTimer > eepromConfig.disarmCount)
//			{
//				zeroPIDstates();
//				armed = false;
//				disarmingTimer = 0;
//			}
//		}
//		else
//		{
//			disarmingTimer = 0;
//		}
//
//		// Check for gyro bias command ( low throttle, left yaw, aft pitch, right roll )
//		if ( (ibusChannels[YAW  ] < (eepromConfig.minCheck - MIDCOMMAND)) &&
//				(ibusChannels[ROLL ] > (eepromConfig.maxCheck - MIDCOMMAND)) &&
//				(ibusChannels[PITCH] < (eepromConfig.minCheck - MIDCOMMAND)) )
//		{
//			computeMPU6000RTData();
//			pulseMotors(3);
//		}
//
//		// Check for arm command ( low throttle, right yaw)
//		if ((ibusChannels[YAW] > (eepromConfig.maxCheck - MIDCOMMAND) ) && (armed == false) && (execUp == true))
//		{
//			armingTimer++;
//
//			if (armingTimer > eepromConfig.armCount)
//			{
//				zeroPIDstates();
//				armed = true;
//				armingTimer = 0;
//			}
//		}
//		else
//		{
//			armingTimer = 0;
//		}
//	}
//
//	///////////////////////////////////
//
//	// Check for armed true and throttle command > minThrottle
//
//	if ((armed == true) && (ibusChannels[THROTTLE] > eepromConfig.minThrottle))
//		pidReset = false;
//	else
//		pidReset = true;
//
//	///////////////////////////////////
//
//	// Check AUX1 for rate, attitude, or GPS mode (3 Position Switch) NOT COMPLETE YET....
//
//	if ((ibusChannels[AUX1] > MIDCOMMAND) && (flightMode == RATE))
//	{
//		flightMode = ATTITUDE;
//		setPIDstates(ROLL_ATT_PID,  0.0f);
//		setPIDstates(PITCH_ATT_PID, 0.0f);
//		setPIDstates(HEADING_PID,   0.0f);
//	}
//	else if ((ibusChannels[AUX1] <= MIDCOMMAND) && (flightMode == ATTITUDE))
//	{
//		flightMode = RATE;
//		setPIDstates(ROLL_RATE_PID,  0.0f);
//		setPIDstates(PITCH_RATE_PID, 0.0f);
//		setPIDstates(YAW_RATE_PID,   0.0f);
//	}
//
//	///////////////////////////////////
//
//	// Check yaw in detent and flight mode to determine hdg hold engaged state
//
//	if ((commandInDetent[YAW] == true) && (flightMode == ATTITUDE) && (headingHoldEngaged == false))
//	{
//		headingHoldEngaged = true;
//		setPIDstates(HEADING_PID,  0.0f);
//		setPIDstates(YAW_RATE_PID, 0.0f);
//		headingReference = heading.mag;
//	}
//
//	if (((commandInDetent[YAW] == false) || (flightMode != ATTITUDE)) && (headingHoldEngaged == true))
//	{
//		headingHoldEngaged = false;
//	}
//
//	///////////////////////////////////
//
//	// Simple Mode Command Processing
//
//	if (ibusChannels[AUX3] > MIDCOMMAND)
//	{
//		hdgDelta = sensors.attitude500Hz[YAW] - homeData.magHeading;
//
//		hdgDelta = standardRadianFormat(hdgDelta);
//
//		simpleX = cosf(hdgDelta) * ibusChannels[PITCH] + sinf(hdgDelta) * ibusChannels[ROLL ];
//
//		simpleY = cosf(hdgDelta) * ibusChannels[ROLL ] - sinf(hdgDelta) * ibusChannels[PITCH];
//
//		ibusChannels[ROLL ] = simpleY;
//
//		ibusChannels[PITCH] = simpleX;
//	}
//
//	///////////////////////////////////
//
//	// Vertical Mode Command Processing
//
//	verticalReferenceCommand = ibusChannels[THROTTLE] - eepromConfig.midCommand;
//
//	// Set past altitude reference in detent value
//	previousVertRefCmdInDetent = vertRefCmdInDetent;
//
//	// Apply deadband and set detent discrete'
//	if ((verticalReferenceCommand <= ALT_DEADBAND) && (verticalReferenceCommand >= -ALT_DEADBAND))
//	{
//		verticalReferenceCommand = 0;
//		vertRefCmdInDetent = true;
//	}
//	else
//	{
//		vertRefCmdInDetent = false;
//		if (verticalReferenceCommand > 0)
//		{
//			verticalReferenceCommand = (verticalReferenceCommand - ALT_DEADBAND) * ALT_DEADBAND_SLOPE;
//		}
//		else
//		{
//			verticalReferenceCommand = (verticalReferenceCommand + ALT_DEADBAND) * ALT_DEADBAND_SLOPE;
//		}
//	}
//
//	///////////////////////////////////
//
//	// Vertical Mode State Machine
//
//	switch (verticalModeState)
//	{
//	case ALT_DISENGAGED_THROTTLE_ACTIVE:
//		if ((ibusChannels[AUX2] > MIDCOMMAND) && (previousAUX2State <= MIDCOMMAND))  // AUX2 Rising edge detection
//		{
//			verticalModeState = ALT_HOLD_FIXED_AT_ENGAGEMENT_ALT;
//			setPIDstates(HDOT_PID,        0.0f);
//			setPIDstates(H_PID,           0.0f);
//			altitudeHoldReference = hEstimate;
//			throttleReference     = ibusChannels[THROTTLE];
//		}
//
//		break;
//
//		///////////////////////////////
//
//	case ALT_HOLD_FIXED_AT_ENGAGEMENT_ALT:
//		if ((vertRefCmdInDetent == true) || eepromConfig.verticalVelocityHoldOnly)
//			verticalModeState = ALT_HOLD_AT_REFERENCE_ALTITUDE;
//
//		if ((ibusChannels[AUX2] <= MIDCOMMAND) && (previousAUX2State > MIDCOMMAND))  // AUX2 Falling edge detection
//			verticalModeState = ALT_DISENGAGED_THROTTLE_INACTIVE;
//
//		if ((ibusChannels[AUX4] > MIDCOMMAND) && (previousAUX4State <= MIDCOMMAND))  // AUX4 Rising edge detection
//			verticalModeState = ALT_DISENGAGED_THROTTLE_ACTIVE;
//
//		break;
//
//		///////////////////////////////
//
//	case ALT_HOLD_AT_REFERENCE_ALTITUDE:
//		if ((vertRefCmdInDetent == false) || eepromConfig.verticalVelocityHoldOnly)
//			verticalModeState = VERTICAL_VELOCITY_HOLD_AT_REFERENCE_VELOCITY;
//
//		if ((ibusChannels[AUX2] <= MIDCOMMAND) && (previousAUX2State > MIDCOMMAND))  // AUX2 Falling edge detection
//			verticalModeState = ALT_DISENGAGED_THROTTLE_INACTIVE;
//
//		if ((ibusChannels[AUX4] > MIDCOMMAND) && (previousAUX4State <= MIDCOMMAND))  // AUX4 Rising edge detection
//			verticalModeState = ALT_DISENGAGED_THROTTLE_ACTIVE;
//
//		break;
//
//		///////////////////////////////
//
//	case VERTICAL_VELOCITY_HOLD_AT_REFERENCE_VELOCITY:
//		if ((vertRefCmdInDetent == true) && !eepromConfig.verticalVelocityHoldOnly)
//		{
//			verticalModeState = ALT_HOLD_AT_REFERENCE_ALTITUDE;
//			altitudeHoldReference = hEstimate;
//		}
//
//		if ((ibusChannels[AUX2] <= MIDCOMMAND) && (previousAUX2State > MIDCOMMAND))  // AUX2 Falling edge detection
//		{
//			verticalModeState = ALT_DISENGAGED_THROTTLE_INACTIVE;
//			altitudeHoldReference = hEstimate;
//		}
//
//
//		if ((ibusChannels[AUX4] > MIDCOMMAND) && (previousAUX4State <= MIDCOMMAND))  // AUX4 Rising edge detection
//			verticalModeState = ALT_DISENGAGED_THROTTLE_ACTIVE;
//
//		break;
//
//		///////////////////////////////
//
//	case ALT_DISENGAGED_THROTTLE_INACTIVE:
//		if (((ibusChannels[THROTTLE] < throttleCmd + THROTTLE_WINDOW) && (ibusChannels[THROTTLE] > throttleCmd - THROTTLE_WINDOW)) ||
//				eepromConfig.verticalVelocityHoldOnly)
//			verticalModeState = ALT_DISENGAGED_THROTTLE_ACTIVE;
//
//		if ((ibusChannels[AUX2] > MIDCOMMAND) && (previousAUX2State <= MIDCOMMAND))  // AUX2 Rising edge detection
//			verticalModeState = ALT_HOLD_FIXED_AT_ENGAGEMENT_ALT;
//
//		if ((ibusChannels[AUX4] > MIDCOMMAND) && (previousAUX4State <= MIDCOMMAND))  // AUX4 Rising edge detection
//			verticalModeState = ALT_DISENGAGED_THROTTLE_ACTIVE;
//
//		break;
//	}
//
//	previousAUX2State = ibusChannels[AUX2];
//	previousAUX4State = ibusChannels[AUX4];
//
//	///////////////////////////////////
//}
//
/////////////////////////////////////////////////////////////////////////////////
//
//
//
//
