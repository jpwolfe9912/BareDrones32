/** @file 		drv_system.c
 *  @brief
 *  	This file contains all the basic functions to run the timing
 *  	of the system as well as initialize the system.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

#include "board.h"

/* Static Variables */
static volatile uint32_t usTicks = 0;
static volatile uint32_t sysTickUptime = 0;
static volatile uint32_t sysTickCycleCounter = 0;

/* Global Variables */
uint16_t frameCounter = 0;

semaphore_t frame_1000Hz= false;
semaphore_t frame_500Hz = false;
semaphore_t frame_200Hz = false;
semaphore_t frame_100Hz = false;
semaphore_t frame_50Hz  = false;
semaphore_t frame_10Hz  = false;
semaphore_t frame_5Hz   = false;
semaphore_t frame_1Hz   = false;

uint32_t deltaTime1000Hz, executionTime1000Hz, previous1000HzTime;
uint32_t deltaTime500Hz,  executionTime500Hz,  previous500HzTime;
uint32_t deltaTime200Hz,  executionTime200Hz,  previous200HzTime;
uint32_t deltaTime100Hz,  executionTime100Hz,  previous100HzTime;
uint32_t deltaTime50Hz,   executionTime50Hz,   previous50HzTime;
uint32_t deltaTime10Hz,   executionTime10Hz,   previous10HzTime;
uint32_t deltaTime5Hz,    executionTime5Hz,    previous5HzTime;
uint32_t deltaTime1Hz,    executionTime1Hz,    previous1HzTime;

float dt500Hz;

semaphore_t systemReady = false;

semaphore_t execUp = false;

/* Static Function Prototypes */
static void cycleCounterInit(void);

/**
 * @brief This function handles System tick timer.
 */
void
SysTick_Handler(void)
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

		currentTime = micros();
		deltaTime1000Hz = currentTime - previous1000HzTime;
		previous1000HzTime = currentTime;

		readMPU6000();

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

		if ((frameCounter % COUNT_200HZ) == 0)
			frame_200Hz = true;

		///////////////////////////////

		if ((frameCounter % COUNT_100HZ) == 0)
		{
			frame_100Hz = true;


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
	}
	else if(!motor_initialized)
		dshotWrite(motor_value);

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


/** @brief Initializes system.
 *
 *  @return Void.
 */
void
systemInit(void)
{
	rcc216MHzInit();

	SysTick_Config(SystemCoreClock / 1000);

	dmaInit();
	cycleCounterInit();

	ledInit();

	checkFirstTime(true);
	/*		LOW LEVEL INITIALIZATION	*/
	serialInit();

	color(CYAN, YES);
	printf("\nBEGINNING AUTODRONE INITIALIZATION\n");
	printf("----------------------------------\n");
	printf("----------------------------------\n");
	colorDefault();

	adc1Ch8Init();

	dshotInit(DSHOT600);
	motorInit();

	spi1Init();

	usart1Init();

	tim9Init();

	wormInit();

	/*		SENSOR INITIALIZATION		*/
	battMonInit();

	orientSensors();
	while(!mpu6000Init());

	madgwickInit();

	while(!ibusInit());

	initPID();
	initPIDvalues();

	motor_initialized = 1;

}

/** @brief Initializes the cycle counter so we can use delay
 *  and getTime functions
 *
 *  @return Void.
 */
static void
cycleCounterInit(void)
{
	usTicks = SystemCoreClock / 1000000;

	// enable DWT access
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	// enable the CPU cycle counter
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}
