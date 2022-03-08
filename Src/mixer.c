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

uint8_t numberMotor = 4;

uint16_t throttleCmd;

int16_t steerCmd, speedCmd;

int16_t motor_temp[4];

///////////////////////////////////////////////////////////////////////////////
// Pulse Motors
///////////////////////////////////////////////////////////////////////////////

//void pulseMotors(uint8_t quantity)
//{
//	uint8_t i;
//
//	for ( i = 0; i < quantity; i++ )
//	{
//		writeAllMotors( eepromConfig.minThrottle );
//		delay(250);
//		writeAllMotors( (float)MINCOMMAND );
//		delay(250);
//	}
//}

///////////////////////////////////////////////////////////////////////////////
// Mixer
///////////////////////////////////////////////////////////////////////////////

#define PIDMIXFLIGHT(X,Y,Z,T) (ratePID[ROLL] * (X) + ratePID[PITCH] * (Y) + eepromConfig.yawDirection * ratePID[YAW] * (Z) + throttleCmd * (T))
#define PIDMIXROVER(X,T) (steerCmd * (X) + speedCmd * (T))

void mixTable(void)
{
	uint8_t i;



	if((mode == FLIGHT) && (armed == true))
	{

		///////////////////////////////////

		motor_temp[0] = PIDMIXFLIGHT(  1.0f, -1.0f, -1.0f, 1.0f );      // Front Left  CW
		motor_temp[1] = PIDMIXFLIGHT( -1.0f, -1.0f,  1.0f, 1.0f );      // Front Right CCW
		motor_temp[2] = PIDMIXFLIGHT( -1.0f,  1.0f, -1.0f, 1.0f );      // Rear Right  CW
		motor_temp[3] = PIDMIXFLIGHT(  1.0f,  1.0f,  1.0f, 1.0f );      // Rear Left   CCW


		///////////////////////////////////



		float maxDeltaThrottle;
		float minDeltaThrottle;
		float deltaThrottle;

		maxDeltaThrottle = (float)MAXCOMMAND - rxCommands[THROTTLE];
		minDeltaThrottle = rxCommands[THROTTLE] - eepromConfig.minThrottle;
		deltaThrottle    = (minDeltaThrottle<maxDeltaThrottle) ? minDeltaThrottle : maxDeltaThrottle;

		for (i=0; i < numberMotor; i++)
		{
			motor_value[i] = constrain16(motor_temp[i], rxCommands[THROTTLE] - deltaThrottle, rxCommands[THROTTLE] + deltaThrottle - 1) - 1952;

			if ((rxCommands[THROTTLE]) < eepromConfig.minCheck)
				motor_value[i] = eepromConfig.minThrottle - 1952;

		}
	}
	else if((mode == ROVER) && (armed == true))
	{

		motor_temp[0] = PIDMIXROVER(  1.0f,  1.0f);	// front left
		motor_temp[1] = PIDMIXROVER( -1.0f,  1.0f);	// front right

		motor_value[0] = dshot3dFormat(motor_temp[0]);
		motor_value[1] = dshot3dFormat(motor_temp[1]);
		motor_value[2] = 0;
		motor_value[3] = 0;
	}
	else
	{
		motor_value[0] = 0;
		motor_value[1] = 0;
		motor_value[2] = 0;
		motor_value[3] = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
