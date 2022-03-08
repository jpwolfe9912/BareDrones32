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

float   attCmd[3];

float   attPID[3];

float   ratePID[3];

float   rateCmd[3];

float   headingReference;

float   altitudeHoldReference;

float   throttleReference;

float   verticalVelocityCmd;

///////////////////////////////////////////////////////////////////////////////
// Compute Axis Commands
///////////////////////////////////////////////////////////////////////////////

void computeAxisCommands(float dt)
{
	float error;

	if (mode == FLIGHT)
	{
		attCmd[ROLL ]	= rxCommands[ROLL ] * eepromConfig.attitudeScaling;
		attCmd[PITCH]	= rxCommands[PITCH] * eepromConfig.attitudeScaling;
		rateCmd[YAW]	= rxCommands[YAW  ] * eepromConfig.yawRateScaling;

		error = standardRadianFormat(attCmd[ROLL] - sensors.attitude500Hz[ROLL]);
		attPID[ROLL ]  = updatePID(error, dt, pidReset, &eepromConfig.PID[ROLL_ATT_PID ]);
		rateCmd[ROLL ] = attPID[ROLL ];

		error = standardRadianFormat(attCmd[PITCH] + sensors.attitude500Hz[PITCH]);
		attPID[PITCH] = updatePID(error, dt, pidReset, &eepromConfig.PID[PITCH_ATT_PID]);
		rateCmd[PITCH] = attPID[PITCH];


		///////////////////////////////////

		error = rateCmd[ROLL] - sensors.gyro500Hz[ROLL];
		ratePID[ROLL] = updatePID(error, dt, pidReset, &eepromConfig.PID[ROLL_RATE_PID ]);

		error = rateCmd[PITCH] + sensors.gyro500Hz[PITCH];
		ratePID[PITCH] = updatePID(error, dt, pidReset, &eepromConfig.PID[PITCH_RATE_PID]);

		error = rateCmd[YAW] - sensors.gyro500Hz[YAW];
		ratePID[YAW] = updatePID(error, dt, pidReset, &eepromConfig.PID[YAW_RATE_PID  ]);

		///////////////////////////////////

		throttleCmd = rxCommands[THROTTLE];
	}
	else if(mode == ROVER)
	{
		speedCmd = rxCommands[PITCH];
		steerCmd = rxCommands[ROLL];
	}


}

///////////////////////////////////////////////////////////////////////////////
