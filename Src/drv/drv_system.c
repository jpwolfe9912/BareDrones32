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

semaphore_t frame_1000Hz = false;
semaphore_t frame_500Hz = false;
semaphore_t frame_200Hz = false;
semaphore_t frame_100Hz = false;
semaphore_t frame_50Hz = false;
semaphore_t frame_10Hz = false;
semaphore_t frame_5Hz = false;
semaphore_t frame_1Hz = false;

uint32_t deltaTime1000Hz, executionTime1000Hz, previous1000HzTime;
uint32_t deltaTime500Hz, executionTime500Hz, previous500HzTime;
uint32_t deltaTime200Hz, executionTime200Hz, previous200HzTime;
uint32_t deltaTime100Hz, executionTime100Hz, previous100HzTime;
uint32_t deltaTime50Hz, executionTime50Hz, previous50HzTime;
uint32_t deltaTime10Hz, executionTime10Hz, previous10HzTime;
uint32_t deltaTime5Hz, executionTime5Hz, previous5HzTime;
uint32_t deltaTime1Hz, executionTime1Hz, previous1HzTime;

float dt500Hz;

semaphore_t systemReady = false;

semaphore_t execUp = false;

volatile uint8_t loopMask = 0x00;
volatile bool loopsChecked;

/* Static Function Prototypes */
static void cycleCounterInit(void);

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
    uint32_t currentTime;

    sysTickCycleCounter = DWT->CYCCNT;
    sysTickUptime++;

    //    watchDogsTick();

    if ((systemReady == true) &&
        (accelCalibrating == false) &&
        (mpu6000Calibrating == false))
    {

        frameCounter++;
        if (frameCounter >= FRAME_COUNT)
            frameCounter = 0;

        if (!(frameCounter % COUNT_1000HZ))
            loopMask |= MASK_1000HZ;

        if (!(frameCounter % COUNT_500HZ))
            loopMask |= MASK_500HZ;

        if (!(frameCounter % COUNT_200HZ))
            loopMask |= MASK_200HZ;

        if (!(frameCounter % COUNT_100HZ))
            loopMask |= MASK_100HZ;

        if (!(frameCounter % COUNT_50HZ))
            loopMask |= MASK_50HZ;

        if (!(frameCounter % COUNT_10HZ))
            loopMask |= MASK_10HZ;

        if (!(frameCounter % COUNT_5HZ))
            loopMask |= MASK_5HZ;

        if (!(frameCounter % COUNT_1HZ))
            loopMask |= MASK_1HZ;

        loopsChecked = true;

        ///////////////////////////////////

        executionTime1000Hz = micros() - currentTime;

        ///////////////////////////////
    }
    // else if (!motor_initialized)
    //     dshotWrite(motor_value);
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
    } while (__STREXW(timeMs, &sysTickUptime));

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

/** @brief Delay in microseconds.
 *
 *  @return Void.
 */
void delayMicroseconds(uint32_t us)
{
    uint32_t elapsed = 0;
    uint32_t lastCount = DWT->CYCCNT;

    for (;;)
    {
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
void delay(int32_t ms)
{
    while (ms > 0)
    {
        delayMicroseconds(1000);
        ms--;
    }
}

/** @brief Initializes system.
 *
 *  @return Void.
 */
void systemInit(void)
{
    rcc216MHzInit();

    SysTick_Config(SystemCoreClock / 1000);

    dmaInit();

    cycleCounterInit();

    ledInit();

    /*		LOW LEVEL INITIALIZATION	*/
    serialInit();

    drawAutodrone();

    color(GREEN, YES);
    printf("\nBEGINNING INITIALIZATION\n");
    printf("----------------------------------\n");
    printf("----------------------------------\n");
    colorDefault();

#ifndef USE_EEPROM
    checkFirstTime(false);
    readEEPROM();
#endif
    adc1Ch8Init();

    dshotInit(DSHOT600);
    // motorInit();

    spi1Init();

    usart1Init();
    usart6Init();

    tim9Init();

    wormInit();

    /*		SENSOR INITIALIZATION		*/
    battMonInit();

    orientSensors();

    mpu6000Init();

    madgwickInit();

    while (!ibusInit())
        ;

    initPID();

    if (eepromChanged)
        writeEEPROM();

    motor_initialized = true;

    //    modeTransition();
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
