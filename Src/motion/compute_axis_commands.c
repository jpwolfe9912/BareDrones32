/** @file 		compute_axis_commands.c
 *  @brief
 *  	This file creates the inputs to the PID controller and sends values to the mixer.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

/* Includes */
#include "board.h"

/* Global Variables */
float   attCmd[3];
float   attPID[3];
float   ratePID[3];
float   rateCmd[3];

/** @brief Computes the commands that get sent to the mixer.
 *
 *  @param dt The PID loop period in sec.
 *  @return Void.
 */
void
computeAxisCommands(float dt)
{
	float error;

	if (mode == FLIGHT)
	{
#ifndef BAD_PID
		attCmd[ROLL ]	= rxCommands[ROLL ] * eepromConfig.attitudeScaling;
		attCmd[PITCH]	= rxCommands[PITCH] * eepromConfig.attitudeScaling;
		rateCmd[YAW ]	= rxCommands[YAW  ] * eepromConfig.yawRateScaling;

		error = standardRadianFormat(attCmd[ROLL] - sensors.attitude500Hz[ROLL]);
		attPID[ROLL ]  = updatePID(error, dt, pidReset, &eepromConfig.PID[ROLL_ATT_PID ]);
		rateCmd[ROLL ] = attPID[ROLL ];

		error = standardRadianFormat(attCmd[PITCH] - sensors.attitude500Hz[PITCH]);
		attPID[PITCH] = updatePID(error, dt, pidReset, &eepromConfig.PID[PITCH_ATT_PID]);
		rateCmd[PITCH] = attPID[PITCH];

		///////////////////////////////////

		error = rateCmd[ROLL] - sensors.gyro500Hz[ROLL];
		ratePID[ROLL] = updatePID(error, dt, pidReset, &eepromConfig.PID[ROLL_RATE_PID ]);

		error = rateCmd[PITCH] - sensors.gyro500Hz[PITCH];
		ratePID[PITCH] = updatePID(error, dt, pidReset, &eepromConfig.PID[PITCH_RATE_PID]);

		error = rateCmd[YAW] + sensors.gyro500Hz[YAW];
		ratePID[YAW] = updatePID(error, dt, pidReset, &eepromConfig.PID[YAW_RATE_PID  ]);

		///////////////////////////////////

		throttleCmd = rxCommands[THROTTLE];
#else
		attCmd[ROLL ]	= rxCommands[ROLL ] * eepromConfig.attitudeScaling;
		attCmd[PITCH]	= rxCommands[PITCH] * eepromConfig.attitudeScaling;
		rateCmd[YAW ]	= rxCommands[YAW  ] * eepromConfig.yawRateScaling;

		attPID[ROLL ] = updatePID(&attPIDdata, sensors.attitude500Hz[ROLL], attCmd[ROLL ]);
		rateCmd[ROLL ] = attPID[ROLL ];

		attPID[PITCH] = updatePID(&attPIDdata, sensors.attitude500Hz[PITCH], attCmd[PITCH]);
		rateCmd[PITCH] = attPID[PITCH];

		///////////////////////////////////

		ratePID[ROLL ] = updatePID(&ratePIDdata, sensors.gyro500Hz[ROLL ], rateCmd[ROLL ]);

		ratePID[PITCH] = updatePID(&ratePIDdata, sensors.gyro500Hz[PITCH], rateCmd[PITCH]);

		ratePID[YAW  ] = updatePID(&ratePIDdata, sensors.gyro500Hz[YAW  ], rateCmd[YAW  ]);

		///////////////////////////////////

		throttleCmd = rxCommands[THROTTLE];
#endif
	}
	else if(mode == ROVER)
	{
		speedCmd = rxCommands[PITCH];
		steerCmd = rxCommands[ROLL];
	}


}

///////////////////////////////////////////////////////////////////////////////
