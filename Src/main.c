/** @file 		main.c
 *  @brief
 *  	This file runs all the other functions and carries out the scheduling of tasks.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

/* Includes */
#include "board.h"

#ifndef SANDBOX
/* Global Variables */
const uint8_t __attribute__((__section__(".eeprom"), used)) eepromArray[131072];

eepromConfig_t	eepromConfig;

uint8_t			execUpCount = 0;

sensors_t		sensors;

uint16_t		timerValue;

/* Test Code */

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

			currentTime = micros();
			deltaTime1000Hz = currentTime = previous1000HzTime;
			previous1000HzTime = currentTime;

			executionTime1000Hz = micros() - currentTime;

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

			sensors.attDeg500Hz[ROLL ] 	= sensors.attitude500Hz[ROLL ] * 57.29578f;
			sensors.attDeg500Hz[PITCH] 	= sensors.attitude500Hz[PITCH] * 57.29578f;
			sensors.attDeg500Hz[YAW  ] 	= sensors.attitude500Hz[YAW  ] * 57.29578f;

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
				printLog(PITCH);

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

			battMonRead();
			if(battEmpty)
				led4TOGGLE();
			else
				led4OFF();

			executionTime5Hz = micros() - currentTime;
		}

		///////////////////////////////

		if (frame_1Hz)
		{
			frame_1Hz = false;

			currentTime     = micros();
			deltaTime1Hz    = currentTime - previous1HzTime;
			previous1HzTime = currentTime;

			if(armed)				// 1 - Armed
				led1ON();
			else
				led1OFF();
			if(systemReady)			// 2 - System Initialized
				led2ON();
			else
				led2OFF();
			if(rcActive)			// 3 - Receiver Connected
				led3ON();
			else
				led3OFF();
			if(battLow)				// 4 - Battery Low
				led4TOGGLE();
			else
				led4OFF();
			if(mode == FLIGHT)		// 5(toggle) - Flight Mode
				led5TOGGLE();
			else if(mode == ROVER)	// 5(on) - Rover Mode
				led5ON();
			else
				led5OFF();

			executionTime1Hz = micros() - currentTime;
		}
	}
}

#else

int main(void)
{
	rcc216MHzInit();


	cycleCounterInit();

	/*		LOW LEVEL INITIALIZATION	*/
	serialInit();

	color(RED, YES);
	printf("\n    __       _     _     _______      _____     ");
	delay(100);
	printf("_______     _______       _____      __    _     _______  ");
	delay(100);
	color(PURPLE, YES);
	printf("\n   /  \\     | |   | |   |_______|    /  _  \\    ");
	delay(100);
	printf("|  __  \\    |   _  \\     /  _  \\    |  \\  | |   |   ____| ");
	delay(100);
	color(BLUE, YES);
	printf("\n  /    \\    | |   | |      | |      /  / \\  \\   ");
	delay(100);
	printf("|  | \\  |   |  | \\  |   /  / \\  \\   |   \\ | |   |  |      ");
	delay(100);
	color(CYAN, YES);
	printf("\n /  /\\  \\   | |   | |      | |      |  | |  |   ");
	delay(100);
	printf("|  | |  |   |  |_/  /   |  | |  |   |    \\| |   |  |____  ");
	delay(100);
	color(CYAN, YES);
	printf("\n/  /__\\  \\  | |   | |      | |      |  | |  |   ");
	delay(100);
	printf("|  | |  |   |      /    |  | |  |   | |\\    |   |   ____| ");
	delay(100);
	color(BLUE, YES);
	printf("\n|   __   |  | |   | |      | |      |  | |  |   ");
	delay(100);
	printf("|  | |  |   |  |\\  \\    |  | |  |   | | \\   |   |  |      ");
	delay(100);
	color(PURPLE, YES);
	printf("\n|  |  |  |  \\ \\___/ /      | |      \\  \\_/  /   ");
	delay(100);
	printf("|  |_/  /   |  | \\  \\   \\  \\_/  /   | |  \\  |   |  |____  ");
	delay(100);
	color(RED, YES);
	printf("\n|__|  |__|   \\_____/       | |       \\_____/    ");
	delay(100);
	printf("|______/    |__| |__|    \\_____/    |_|   \\_|   |_______| ");
	delay(100);
	colorDefault();

	color(GREEN, YES	);
	printf("\nBEGINNING INITIALIZATION\n");
	printf("----------------------------------\n");
	printf("----------------------------------\n");
	colorDefault();

	while(1);
}

#endif

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
