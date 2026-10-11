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

#include "stm32f7xx.h"
#include "utilities.h"
#include "drv_system.h"
#include "drv_usart3.h"
#include "receiver.h"
#include "mpu6000.h"
#include "mpu6000_calibration.h"
#include "flight_types.h"
#include "config.h"
#include "pid.h"
#include "logging.h"

/* Global Variables */
volatile bool armed = false;
float rcCommands[RC_CHANNELS_MAX];
flightModes_e flightMode = ANGLE;

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
				// initPIDvalues();		//TODO: Has the same functionality as sending "pid" console command
			}
		}
		else
			pid_config_latched = false;

		/* Compute MPU6000 RT data (low throttle, right yaw, left roll, aft stick) */
		bool computeRTData = ((rcCommands[YAW] > (eepromConfig.maxCheck - MIDCOMMAND)) &&
							  (rcCommands[ROLL] < (eepromConfig.minCheck - MIDCOMMAND)) &&	//maxcheck = 3800
							  (rcCommands[PITCH] < (eepromConfig.minCheck - MIDCOMMAND)));
		if (computeRTData)
		{
			if (!compute_rt_data_latched)
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
	char command[64];

	if (!usart3ReadLine(command, sizeof(command)))
		return;

	//* Flash command processor
#ifdef USE_W25Q128
	if (!strcmp(command, "dump"))
	{
		flashLoggerStartDump();
		return;
	}

	if (!strcmp(command, "erase"))
	{
		flashLoggerErase();
		return;
	}
#endif

	//* PID command processor
	uint8_t ID;
	float P;
	float I;
	float D;

	if (sscanf(command, "pid %u %f %f %f", &ID, &P, &I, &D) == 4)
	{
		if (ID >= 5)
		{
			printf("Invalid PID ID\n");
			return;
		}

		eepromConfig.PID[ID].P = P;
		eepromConfig.PID[ID].I = I;
		eepromConfig.PID[ID].D = D;

		if (ID < 2)
			eepromConfig.PID[ID].limit = 1000.0f * P * PI / 180.0f;
		else if (ID == 2)
			eepromConfig.PID[ID].limit = 1000.0f * eepromConfig.yawRateScaling * P;
		else
			eepromConfig.PID[ID].limit = 1000.0f * eepromConfig.attitudeScaling * P;

		printf("PID %u updated:\t P=%.2f\t I=%.2f\t D=%.2f\t Limit=%.2f\n",
			   ID,
			   eepromConfig.PID[ID].P,
			   eepromConfig.PID[ID].I,
			   eepromConfig.PID[ID].D,
			   eepromConfig.PID[ID].limit);

		return;
	}

	if (!strcmp(command, "pid"))
	{
		for (uint8_t i = 0; i < NUMBER_OF_PIDS; i++)
		{
			printf("PID %u:\t P=%.2f\t I=%.2f\t D=%.2f\t Limit=%.2f\n",
				   i,
				   eepromConfig.PID[i].P,
				   eepromConfig.PID[i].I,
				   eepromConfig.PID[i].D,
				   eepromConfig.PID[i].limit);
		}
		return;
	}

	//* Rate command processor
	float maxRate;
	float coeff_A;
	float coeff_B;

	if (!strcmp(command, "rate"))
	{
		maxRate = (eepromConfig.rateCoeffAlpha * 1000000000.0f) +
			(eepromConfig.rateCoeffBravo * 1000.0f);

		printf("Rate: A=%.9g B=%.9g Max=%.0f\n",
			   eepromConfig.rateCoeffAlpha,
			   eepromConfig.rateCoeffBravo,
			   maxRate);
		return;
	}

	if (sscanf(command, "rate %f, %f", &coeff_A, &coeff_B) == 2)
	{
		eepromConfig.rateCoeffAlpha = coeff_A;
		eepromConfig.rateCoeffBravo = coeff_B;

		maxRate = (eepromConfig.rateCoeffAlpha * 1000000000.0f) +
			(eepromConfig.rateCoeffBravo * 1000.0f);

		printf("Rate updated: A=%.9g B=%.9g Max=%.0f\n",
			   eepromConfig.rateCoeffAlpha,
			   eepromConfig.rateCoeffBravo,
			   maxRate);
		return;
	}

	//* Wired logging command processor
#ifdef WIRED_LOGGING
	if (!strcmp(command, "start log"))
	{
		wiredLoggerStart();
		return;
	}

	if (!strcmp(command, "stop log"))
	{
		wiredLoggerStop();
		return;
	}
#endif


	//* Write eeprom command processor
	if (!strcmp(command, "write"))
	{
		writeEEPROM();
		printf("EEPROM written\n");
		return;
	}

	//* Restart system command processor
	if (!strcmp(command, "restart"))
	{
		printf("Restarting...\n");
		delay(100);
		/*
		 * Make sure the response actually finishes transmitting before reset,
		 * if desired.
		 */

		NVIC_SystemReset();
	}
}