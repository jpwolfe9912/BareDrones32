/** @file 		accel_calibration_mpu.c
 *  @brief
 *  	This file is used to calibrate the accelerometer.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		06 MAR 2022
 */

/* Includes */
#include "board.h"

/** @brief Calibrates the accelerometer.
 *
 *  @return Void.
 */
void
accelCalibrationMPU(void)
{
	float noseUp        = 0.0f;
	float noseDown      = 0.0f;
	float leftWingDown  = 0.0f;
	float rightWingDown = 0.0f;
	float upSideDown    = 0.0f;
	float rightSideUp   = 0.0f;

	int16_t index;

	accelCalibrating = true;

	printf("\nMPU6000 Accelerometer Calibration:\n\n");

	///////////////////////////////////

	printf("Place accelerometer right side up\n");
	printf("  Send a character when ready to proceed\n\n");

	if(serialWaitFor('y'));

	printf("  Gathering Data...\n\n");

	for (index = 0; index < 5000; index++)
	{
		readMPU6000();

		computeMPU6000TCBias();

		rightSideUp += (float)rawAccel[ZAXIS].value - accelTCBias[ZAXIS];
		delayMicroseconds(100);
	}

	rightSideUp /= 5000.0f;

	printf("Place accelerometer up side down\n");
	printf("  Send a character when ready to proceed\n\n");

	if(serialWaitFor('y'));

	printf("  Gathering Data...\n\n");

	for (index = 0; index < 5000; index++)
	{
		readMPU6000();

		computeMPU6000TCBias();

		upSideDown += (float)rawAccel[ZAXIS].value - accelTCBias[ZAXIS];
		delayMicroseconds(100);
	}

	upSideDown /= 5000.0f;

	///////////////////////////////////

	printf("Place accelerometer left edge down\n");
	printf("  Send a character when ready to proceed\n\n");

	if(serialWaitFor('y'));

	printf("  Gathering Data...\n\n");

	for (index = 0; index < 5000; index++)
	{
		readMPU6000();

		computeMPU6000TCBias();

		leftWingDown += (float)rawAccel[YAXIS].value - accelTCBias[YAXIS];
		delayMicroseconds(100);
	}

	leftWingDown /= 5000.0f;

	printf("Place accelerometer right edge down\n");
	printf("  Send a character when ready to proceed\n\n");

	if(serialWaitFor('y'));

	printf("  Gathering Data...\n\n");

	for (index = 0; index < 5000; index++)
	{
		readMPU6000();

		computeMPU6000TCBias();

		rightWingDown += (float)rawAccel[YAXIS].value - accelTCBias[YAXIS];
		delayMicroseconds(100);
	}

	rightWingDown /= 5000.0f;

	///////////////////////////////////

	printf("Place accelerometer rear edge down\n");
	printf("  Send a character when ready to proceed\n\n");

	if(serialWaitFor('y'));

	printf("  Gathering Data...\n\n");

	for (index = 0; index < 5000; index++)
	{
		readMPU6000();

		computeMPU6000TCBias();

		noseUp += (float)rawAccel[XAXIS].value - accelTCBias[XAXIS];
		delayMicroseconds(100);
	}

	noseUp /= 5000.0f;

	printf("Place accelerometer front edge down\n");
	printf("  Send a character when ready to proceed\n\n");

	if(serialWaitFor('y'));

	printf("  Gathering Data...\n\n");

	for (index = 0; index < 5000; index++)
	{
		readMPU6000();

		computeMPU6000TCBias();

		noseDown += (float)rawAccel[XAXIS].value - accelTCBias[XAXIS];
		delayMicroseconds(100);
	}

	noseDown /= 5000.0f;

	///////////////////////////////////

	eepromConfig.accelBiasMPU[ZAXIS]        = (rightSideUp + upSideDown) / 2.0f;
	eepromConfig.accelScaleFactorMPU[ZAXIS] = (2.0f * 9.8065f) / fabsf(rightSideUp - upSideDown);

	eepromConfig.accelBiasMPU[YAXIS]        = (leftWingDown + rightWingDown) / 2.0f;
	eepromConfig.accelScaleFactorMPU[YAXIS] = (2.0f * 9.8065f) / fabsf(leftWingDown - rightWingDown);

	eepromConfig.accelBiasMPU[XAXIS]        = (noseUp + noseDown) / 2.0f;
	eepromConfig.accelScaleFactorMPU[XAXIS] = (2.0f * 9.8065f) / fabsf(noseUp - noseDown);

	///////////////////////////////////

	accelCalibrating = false;
}

///////////////////////////////////////////////////////////////////////////////
