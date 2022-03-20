/** @file 		main.c
 *  @brief
 *  	This file runs all the other functions and carries out the scheduling of tasks.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

/* Includes */
#include "board.h"

/* Global Variables */
__attribute__((__section__(".eeprom"), used)) const int8_t eepromArray[16384];

eepromConfig_t	eepromConfig;

uint8_t			execUpCount = 0;

sensors_t		sensors;

uint16_t		timerValue;


int
main(void)
{
	uint32_t currentTime;

	systemReady = false;

	systemInit();

	systemReady = true;

	while (1)
	{
		if(frame_1000Hz)
		{
			frame_1000Hz = false;

		}

		if (frame_500Hz)
		{

			frame_500Hz = false;

			currentTime       = micros();
			deltaTime500Hz    = currentTime - previous500HzTime;
			previous500HzTime = currentTime;

			tim9Disable();
			timerValue = tim9GetCnt();
			tim9ResetCnt();
			tim9Enable();

			dt500Hz = (float)timerValue * 0.0000005f;  // For integrations in 500 Hz loop

			computeRotations500Hz();

			updateIMU(sensors.gyro500Hz[ROLL ], sensors.gyro500Hz[PITCH], sensors.gyro500Hz[YAW],
					sensors.accel500Hz[XAXIS], sensors.accel500Hz[YAXIS], sensors.accel500Hz[ZAXIS]);
			sensors.attitude500Hz[ROLL ] = getRollRadians();
			sensors.attitude500Hz[PITCH] = getPitchRadians();
			sensors.attitude500Hz[YAW  ] = getYawRadians();

			processCommands();
			computeAxisCommands(dt500Hz);
			mixTable();
			dshotWrite(motor_value);

			executionTime500Hz = micros() - currentTime;

		}


		///////////////////////////////

		if (frame_200Hz)
		{
			frame_200Hz = false;

			currentTime     = micros();
			deltaTime200Hz    = currentTime - previous200HzTime;
			previous200HzTime = currentTime;

			ibusProcess();

			executionTime200Hz = micros() - currentTime;
		}

		///////////////////////////////

		if (frame_100Hz)
		{

			frame_100Hz = false;

			currentTime       = micros();
			deltaTime100Hz    = currentTime - previous100HzTime;
			previous100HzTime = currentTime;

			if (armed == true)
			{
				if ( eepromConfig.activeTelemetry == 1 )
				{
					// Roll Loop
					printf("1,%9.4f,%1d,%9.4f,%9.4f,%9.4f,%9.4f,%9.4f,%9.4f\n",
							battVoltage,
							mode,
							rateCmd[ROLL],
							sensors.gyro500Hz[ROLL],
							ratePID[ROLL],
							attCmd[ROLL],
							sensors.attitude500Hz[ROLL],
							attPID[ROLL]);
				}

				if ( eepromConfig.activeTelemetry == 2 )
				{
					// Pitch Loop
					printf("2,%9.4f,%1d,%9.4f,%9.4f,%9.4f,%9.4f,%9.4f,%9.4f\n",
							battVoltage,
							mode,
							rateCmd[PITCH],
							sensors.gyro500Hz[PITCH],
							ratePID[PITCH],
							attCmd[PITCH],
							sensors.attitude500Hz[PITCH],
							attPID[PITCH]);
				}

				if ( eepromConfig.activeTelemetry == 3 )
				{
					// Sensors
					printf("3,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,%8.4f,\n",
							battVoltage,
							sensors.accel500Hz[XAXIS],
							sensors.accel500Hz[YAXIS],
							sensors.accel500Hz[ZAXIS],
							sensors.gyro500Hz[ROLL],
							sensors.gyro500Hz[PITCH],
							sensors.gyro500Hz[YAW],
							sensors.attitude500Hz[ROLL],
							sensors.attitude500Hz[PITCH],
							sensors.attitude500Hz[YAW]);

				}

				if ( eepromConfig.activeTelemetry == 4 )
				{
					printf("4,%9.4f,%u,%u,%u,%u,\n",
							battVoltage,
							motor_value[0],
							motor_value[1],
							motor_value[2],
							motor_value[3]);
				}

			}


			executionTime100Hz = micros() - currentTime;

		}
		if (frame_50Hz)
		{

			frame_50Hz = false;

			currentTime      = micros();
			deltaTime50Hz    = currentTime - previous50HzTime;
			previous50HzTime = currentTime;


			executionTime50Hz = micros() - currentTime;

		}

		///////////////////////////////

		if (frame_10Hz)
		{

			frame_10Hz = false;

			currentTime      = micros();
			deltaTime10Hz    = currentTime - previous10HzTime;
			previous10HzTime = currentTime;
			executionTime10Hz = micros() - currentTime;

		}

		///////////////////////////////



		///////////////////////////////

		if (frame_5Hz)
		{
			frame_5Hz = false;

			currentTime     = micros();
			deltaTime5Hz    = currentTime - previous5HzTime;
			previous5HzTime = currentTime;

			batMonRead();
			if(battEmpty)
				led4TOGGLE();

			executionTime5Hz = micros() - currentTime;
		}

		///////////////////////////////

		if (frame_1Hz)
		{
			frame_1Hz = false;

			currentTime     = micros();
			deltaTime1Hz    = currentTime - previous1HzTime;
			previous1HzTime = currentTime;

			if(armed)
				led1TOGGLE();
			if(systemReady)
				led2TOGGLE();
			if(rcActive)
				led3TOGGLE();
			if(battLow)
				led4TOGGLE();
			if(mode == FLIGHT)
				led5TOGGLE();
			else if(mode == ROVER)
				led5ON();

			executionTime1Hz = micros() - currentTime;
		}
	}
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	__disable_irq();
	while (1)
	{
	}
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
