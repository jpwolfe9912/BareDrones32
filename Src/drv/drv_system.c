/*
 * drv_system.c
 *
 *  Created on: Dec 24, 2021
 *      Author: jeremywolfe
 */

#include "board.h"

///////////////////////////////////////////////////////////////////////////////

// Cycles per microsecond
static volatile uint32_t usTicks = 0;

///////////////////////////////////////////////////////////////////////////////

// Current uptime for 1kHz systick timer. will rollover after 49 days.
// Hopefully we won't care.
static volatile uint32_t sysTickUptime = 0;
static volatile uint32_t sysTickCycleCounter = 0;

///////////////////////////////////////////////////////////////////////////////
// Cycle Counter
///////////////////////////////////////////////////////////////////////////////

static void cycleCounterInit(void)
{
	usTicks = SystemCoreClock / 1000000;

	// enable DWT access
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	// enable the CPU cycle counter
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

///////////////////////////////////////
// Frame Timing Variables
///////////////////////////////////////

uint16_t frameCounter = 0;

semaphore_t frame_1000Hz= false;
semaphore_t frame_500Hz = false;
semaphore_t frame_100Hz = false;
semaphore_t frame_50Hz  = false;
semaphore_t frame_10Hz  = false;
semaphore_t frame_5Hz   = false;
semaphore_t frame_1Hz   = false;

uint32_t deltaTime1000Hz, executionTime1000Hz, previous1000HzTime;
uint32_t deltaTime500Hz,  executionTime500Hz,  previous500HzTime;
uint32_t deltaTime100Hz,  executionTime100Hz,  previous100HzTime;
uint32_t deltaTime50Hz,   executionTime50Hz,   previous50HzTime;
uint32_t deltaTime10Hz,   executionTime10Hz,   previous10HzTime;
uint32_t deltaTime5Hz,    executionTime5Hz,    previous5HzTime;
uint32_t deltaTime1Hz,    executionTime1Hz,    previous1HzTime;

float dt500Hz, dt100Hz;

semaphore_t systemReady = false;

semaphore_t execUp = false;

uint8_t whoami;

uint16_t motor_value[4] = {0, 0, 0, 0};

#ifdef _DTIMING
#define LA2_ENABLE       GPIO_SetBits(GPIOC,   GPIO_Pin_2)
#define LA2_DISABLE      GPIO_ResetBits(GPIOC, GPIO_Pin_2)
#endif

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
	uint8_t index;
	uint32_t currentTime;

	sysTickCycleCounter = DWT->CYCCNT;
	sysTickUptime++;

//    watchDogsTick();

	if ((systemReady         == true)  &&
		(accelCalibrating    == false) &&
		(mpu6000Calibrating  == false))
	{

		frameCounter++;
		if (frameCounter > FRAME_COUNT)
			frameCounter = 1;

		///////////////////////////////
		frame_1000Hz = true;

		accelSum500Hz[XAXIS] += rawAccel[XAXIS].value;
		accelSum500Hz[YAXIS] += rawAccel[YAXIS].value;
		accelSum500Hz[ZAXIS] += rawAccel[ZAXIS].value;

		accelSum100Hz[XAXIS] += rawAccel[XAXIS].value;
		accelSum100Hz[YAXIS] += rawAccel[YAXIS].value;
		accelSum100Hz[ZAXIS] += rawAccel[ZAXIS].value;

		gyroSum500Hz[ROLL ] += rawGyro[ROLL ].value;
		gyroSum500Hz[PITCH] += rawGyro[PITCH].value;
		gyroSum500Hz[YAW  ] += rawGyro[YAW  ].value;

		///////////////////////////////

		if ((frameCounter % COUNT_500HZ) == 0)
		{
			frame_500Hz = true;

			for (index = 0; index < 3; index++)
			{
				accelSummedSamples500Hz[index] = accelSum500Hz[index];
				accelSum500Hz[index] = 0;

				gyroSummedSamples500Hz[index] = gyroSum500Hz[index];
				gyroSum500Hz[index] = 0;
			}
		}

		///////////////////////////////

		if ((frameCounter % COUNT_100HZ) == 0)
		{
			frame_100Hz = true;

			for (index = 0; index < 3; index++)
			{
				accelSummedSamples100Hz[index] = accelSum100Hz[index];
				accelSum100Hz[index] = 0;
			}

//			if (!newTemperatureReading)
//			{
//				readTemperatureRequestPressure();
//				newTemperatureReading = true;
//			}
//			else
//			{
//				readPressureRequestTemperature();
//				newPressureReading = true;
//			}
//
//			sdCardCountDown();
		}

		///////////////////////////////

		if ((frameCounter % COUNT_50HZ) == 0)
			frame_50Hz = true;

		///////////////////////////////

		if ((frameCounter % COUNT_10HZ) == 0)
			frame_10Hz = true;

		///////////////////////////////

		if ((frameCounter % COUNT_5HZ) == 0)
			frame_5Hz = true;

		///////////////////////////////

		if ((frameCounter % COUNT_1HZ) == 0)
			frame_1Hz = true;

		///////////////////////////////////

		executionTime1000Hz = micros() - currentTime;

		///////////////////////////////

#ifdef _DTIMING
		//            LA2_DISABLE;
#endif
	}
}

/** @brief Gets system time in microseconds.
 *
 *		This can be called from within IRQ Handlers, so uses LDREX/STREX.
 *		If a higher priority IRQ or DMA or anything happens the STREX will fail
 *		and restart the loop. Otherwise the same number that was read is harmlessly
 *		written back.
 *
 *  @return uint32_t Time in microseconds.
 */
uint32_t
micros(void)
{
	register uint32_t oldCycle, cycle, timeMs;

	do
	{
		timeMs = __LDREXW(&sysTickUptime);
		cycle = DWT->CYCCNT;
		oldCycle = sysTickCycleCounter;
	}
	while ( __STREXW( timeMs , &sysTickUptime ) );

	return (timeMs * 1000) + (cycle - oldCycle) / usTicks;
}

/** @brief Gets system time in milliseconds.
 *
 *  @return uint32_t Time in milliseconds.
 */
uint32_t
millis(void)
{
	return sysTickUptime;
}

//void checkResetType()
//{
//    uint32_t rst = RCC->CSR;
//
//    evrPush(( rst & (RCC_CSR_PORRSTF | RCC_CSR_PADRSTF | RCC_CSR_SFTRSTF) ) ? EVR_NormalReset : EVR_AbnormalReset , rst >> 24 );
//
//    RCC_ClearFlag();
//}

/** @brief Initializes system.
 *
 *  @return Void.
 */
void
systemInit(void)
{
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	// diables SysTick

	SystemClock_Config();
	dmaInit();
	cycleCounterInit();
	/*		LOW LEVEL INITIALIZATION	*/
	serialInit();
	printf("\nBEGINNING AUTODRONE INITIALIZATION\n");
	printf("----------------------------------\n");
	printf("----------------------------------\n");

	spi1Init();

	usart1Init();

//	MX_USB_OTG_FS_PCD_Init();

	/*		SENSOR INITIALIZATION		*/
	while(!mpu6000Init());

	madgwickInit();

	while(!ibusInit());

	dshotInit(DSHOT600);
	motorInit();

	SysTick_Config(216000);
}

/** @brief Initializes system clock.
 *
 *  @return Void.
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 216;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Activate the Over-Drive mode
	 */
	if (HAL_PWREx_EnableOverDrive() != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART3
			|RCC_PERIPHCLK_USART6|RCC_PERIPHCLK_UART5
			|RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C2
			|RCC_PERIPHCLK_CLK48;
	PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
	PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
	PeriphClkInitStruct.Uart5ClockSelection = RCC_UART5CLKSOURCE_PCLK1;
	PeriphClkInitStruct.Usart6ClockSelection = RCC_USART6CLKSOURCE_PCLK2;
	PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
	PeriphClkInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
	PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
}

/** @brief Delay in microseconds.
 *
 *  @return Void.
 */
void delayMicroseconds(uint32_t us)
{
	uint32_t elapsed = 0;
	uint32_t lastCount = DWT->CYCCNT;

	for (;;) {
		register uint32_t current_count = DWT->CYCCNT;
		uint32_t elapsed_us;

		// measure the time elapsed since the last time we checked
		elapsed += current_count - lastCount;
		lastCount = current_count;

		// convert to microseconds
		elapsed_us = elapsed / usTicks;
		if (elapsed_us >= us)
			break;

		// reduce the delay by the elapsed time
		us -= elapsed_us;

		// keep fractional microseconds for the next iteration
		elapsed %= usTicks;
	}
}

/** @brief Delay in milliseconds.
 *
 *  @return Void.
 */
void delay(uint32_t ms)
{
	while (ms--)
		delayMicroseconds(1000);
}
