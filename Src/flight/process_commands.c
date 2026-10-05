/** @file 		process_commands.c
 *  @brief
 *  	This file processes the receiver inputs to ensure they are in the correct
 *  	ranges for further computations.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

 /* Includes */
#include "process_commands.h"

#include "drv_system.h"
#include "drv_usart3.h"
#include "receiver.h"
#include "mpu6000.h"
#include "mpu6000_calibration.h"
#include "config.h"
#include "pid.h"
#include "logging.h"

/* Global Variables */
flightModes_e flightMode = ANGLE;
volatile bool armed = false;
float rcCommands[RC_CHANNELS_MAX];

/* Static Variables */
static bool imu_calibration_latched = false;
static bool pid_config_latched = false;
static bool compute_rt_data_latched = false;


/** @brief Processes receiver commands.
 *
 *  @return Void.
 */
void
processRcCommands(void)
{
	/* Package raw data from receiever */
	if (rcData.connected == true)
	{
		/* Makes RPY from 1000:2000 to -1000:1000 */
		rcCommands[ROLL] = (rcData.channels[RC_AIL] * 2) - MIDCOMMAND;	// Roll Range  -1000:1000
		rcCommands[PITCH] = (rcData.channels[RC_ELE] * 2) - MIDCOMMAND;	// Pitch Range -1000:1000
		rcCommands[YAW] = (rcData.channels[RC_RUD] * 2) - MIDCOMMAND;	// Yaw Range   -1000:1000
		rcCommands[THROTTLE] = (rcData.channels[RC_THR]) * 2;				// Throttle Range 2000:4000

		/* Makes all other channels from 2000 to 4000 */
		for (uint8_t channel = 4; channel < RC_CHANNELS_MAX; channel++)
			rcCommands[channel] = rcData.channels[channel] * 2;
	}

	/* Apply deadbands */
	for (uint8_t channel = 0; channel < 3; channel++)
	{
		/* RPY is within deadband */
		if ((rcCommands[channel] <= DEADBAND) && (rcCommands[channel] >= -DEADBAND))
		{
			rcCommands[channel] = 0;			// set command to 0
		}
		else
		{
			if (rcCommands[channel] > 0)
				rcCommands[channel] = (rcCommands[channel] - DEADBAND) * DEADBAND_SLOPE;

			else
				rcCommands[channel] = (rcCommands[channel] + DEADBAND) * DEADBAND_SLOPE;
		}
	}

	/* Auxillary switch handling */

	/*		Check for disarm switch	*/
	if (rcCommands[RC_AUX1] < MIDCOMMAND)
	{
		resetPID();
		armed = false;	// disarm the quad

		/* Calibrate IMU (low throttle, left yaw, aft pitch, right roll) */
		bool imuCalibrationCommand = ((rcCommands[YAW] < (eepromConfig.minCheck - MIDCOMMAND)) &&	//mincheck = 2200
									  (rcCommands[ROLL] > (eepromConfig.maxCheck - MIDCOMMAND)) &&	//maxcheck = 3800
									  (rcCommands[PITCH] < (eepromConfig.minCheck - MIDCOMMAND)));
		if (imuCalibrationCommand)
		{
			if (!imu_calibration_latched)
			{
				imu_calibration_latched = true;
				mpu6000Calibration();
			}
		}
		else
			imu_calibration_latched = false;

		/* Set PID values (low throttle, left yaw, right roll, forward pitch) */
		bool pidConfigCommand = ((rcCommands[YAW] < (eepromConfig.minCheck - MIDCOMMAND)) &&
								 (rcCommands[ROLL] > (eepromConfig.maxCheck - MIDCOMMAND)) &&
								 (rcCommands[PITCH] > (eepromConfig.maxCheck - MIDCOMMAND)));
		if (pidConfigCommand)
		{
			if (!pid_config_latched)
			{
				pid_config_latched = true;
				initPIDvalues();
			}
		}
		else
			pid_config_latched = false;

		/* Compute MPU6000 RT data (low throttle, right yaw, left roll, aft stick) */
		bool computeRTData =  ((rcCommands[YAW] > (eepromConfig.maxCheck - MIDCOMMAND)) &&
									(rcCommands[ROLL] < (eepromConfig.minCheck - MIDCOMMAND)) &&	//maxcheck = 3800
									(rcCommands[PITCH] < (eepromConfig.minCheck - MIDCOMMAND)));
		if (computeRTData)
		{	
			if(!compute_rt_data_latched)
			{
				compute_rt_data_latched = true;
				computeMPU6000RTData();
			}
			else
				compute_rt_data_latched = false;
		}
	}

	/*		Check for arm switch and throttle low(<2200)	*/
	if ((rcCommands[RC_AUX1] > MIDCOMMAND) &&
		(rcCommands[THROTTLE] < eepromConfig.minCheck) &&
		(!armed))
	{
		resetPID();
		armed = true;
	}

	/* Check for Flight Mode Change */
	if (rcCommands[RC_AUX2] > MIDCOMMAND)
		flightMode = ANGLE;
	else
		flightMode = RATE;

	///////////////////////////////////

	// Check for armed true and throttle command > minThrottle

	if ((armed == true) && (rcCommands[THROTTLE] > eepromConfig.minThrottle))
		pidReset = false;
	else
		pidReset = true;

}

/** @brief Processes serial commands.
 *
 *  @return Void.
 */
void
processSerialCommands(void)
{
#ifdef USE_W25Q128
	/* Check for Flash Requests */
	char command[10] = { 0 };
	size_t avail = lwrb_get_full(&Buff_3.RxBuffer);

	if (avail == sizeof("dump"))
	{
		lwrb_read(&Buff_3.RxBuffer, command, avail);
		if (!strcmp(command, "dump\r"))
		{
			memset(command, 0, sizeof(command));
			flashLoggerStartDump();
		}
		else return;
	}
	else if (avail == sizeof("erase"))
	{
		lwrb_read(&Buff_3.RxBuffer, command, avail);
		if (!strcmp(command, "erase\r"))
		{
			memset(command, 0, sizeof(command));
			flashLoggerErase();
		}
		else return;
	}
	else
	{
		lwrb_skip(&Buff_3.RxBuffer, avail);
		return;
	}
#endif
}