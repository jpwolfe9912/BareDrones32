/** @file 		drv_system.c
 *  @brief
 *  	This file contains all the basic functions to run the timing
 *  	of the system as well as initialize the system.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

#include "drv_system.h"

#include "stm32f7xx.h"

#include "feature_config.h"
#include "scheduler.h"
#include "drv_rcc.h"
#include "drv_dma.h"
#include "drv_led.h"
#include "drv_color.h"
#include "config.h"
#include "drv_dshot_burst.h"
#include "motors.h"
#include "drv_spi1.h"
#include "drv_spi2.h"
#include "drv_usart1.h"
#include "drv_usart2.h"
#include "drv_usart3.h"
#include "drv_usart6.h"
#include "drv_tim.h"
#include "drv_adc.h"
#include "battery.h"
#include "orientation.h"
#include "mpu6000.h"
#include "madgwick.h"
#include "receiver.h"
#include "ibus.h"
#include "crsf.h"
#include "w25q128.h"
#include "pid.h"

 /* Static Variables */
static volatile uint32_t cyclesPerUs = 0;
static volatile uint32_t sysTickUptimeUs = 0;
static volatile uint32_t sysTickCycleCounter = 0;

/* Global Variables */
uint16_t frameCounter = 0;

float dt8000Hz;

semaphore_t systemReady = false;

semaphore_t execUp = false;

volatile uint16_t loopMask = 0x0000;
volatile bool loopsChecked;

/* Static Function Prototypes */
static void cycleCounterInit(void);

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
    sysTickCycleCounter = DWT->CYCCNT;
    sysTickUptimeUs += SYSTICK_PERIOD_US;

    if (systemReady)
    {

        frameCounter++;
        if (frameCounter >= FRAME_COUNT)
            frameCounter = 0;
        if (!(frameCounter % COUNT_8000HZ))
            loopMask |= MASK_8000HZ;

        if (!(frameCounter % COUNT_4000HZ))
            loopMask |= MASK_4000HZ;

        if (!(frameCounter % COUNT_2000HZ))
            loopMask |= MASK_2000HZ;

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
    register uint32_t oldCycle, cycle, timeUs;

    do
    {
        timeUs = __LDREXW(&sysTickUptimeUs);
        cycle = DWT->CYCCNT;
        oldCycle = sysTickCycleCounter;
    } while (__STREXW(timeUs, &sysTickUptimeUs));

    return timeUs + (cycle - oldCycle) / cyclesPerUs;
}

/** @brief Gets system time in milliseconds.
 *
 *  @return uint32_t Time in milliseconds.
 */
uint32_t
millis(void)
{
    return micros() / 1000U;
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
        elapsed_us = elapsed / cyclesPerUs;
        if (elapsed_us >= us)
            break;

        // reduce the delay by the elapsed time
        us -= elapsed_us;

        // keep fractional microseconds for the next iteration
        elapsed %= cyclesPerUs;
    }
}

/** @brief Delay in milliseconds.
 *
 *  @return Void.
 */
void delay(int32_t ms)
{
    while (ms--)
        delayMicroseconds(1000);
}

/** @brief Initializes system.
 *
 *  @return Void.
 */
void systemInit(void)
{
    rcc216MHzInit();

    cycleCounterInit();

    SysTick_Config(SystemCoreClock / FRAME_COUNT);

    dmaInit();

#ifdef USE_LEDS
    ledInit();
#endif
    /*		LOW LEVEL INITIALIZATION	*/
    usart3Init(WIREDLOGGING_BAUDRATE);

#ifdef USE_W25Q128
    spi2Init();
    w25q128Init();
    w25q128SectorErase(0);
    w25q128SectorErase(1);
    w25q128SectorErase(2);
#endif

#ifdef OPENLAGER
    usart6Init(OPENLAGER_BAUDRATE);
#endif
#ifdef DRAW_AUTODRONE
    drawAutodrone();
#endif

    color(GREEN, YES);
    printf("\nBEGINNING INITIALIZATION\n");
    printf("----------------------------------\n");
    printf("----------------------------------\n");
    colorDefault();

#ifdef USE_EEPROM
    checkFirstTime(false);
    readEEPROM();
#endif
#ifdef USE_DSHOT
    dshotInit(DSHOT600);
    motorInit();
#endif

#ifdef USE_MPU6000
    spi1Init();
    mpu6000Init();
#endif

#ifdef USE_USART1_DRIVER
    usart1Init();
#endif
#ifdef USE_IBUS
    while (!ibusInit());
    usart2Init(IBUS_BAUDRATE);
#endif

#ifdef USE_CRSF
    usart2Init(CRSF_BAUDRATE);
    while (!crsfInit());
#endif

    tim9Init();

    /*		SENSOR INITIALIZATION		*/
#ifdef USE_BATT_MON
    adc1Init();
    battMonInit();
#endif

    orientSensors();

#ifdef USE_MOTION_PROCESSING
    madgwickInit();
    initPID();
#endif

#ifdef USE_EEPROM
    if (eepromChanged)
        writeEEPROM();
#endif
}

/** @brief Initializes the cycle counter so we can use delay
 *  and getTime functions
 *
 *  @return Void.
 */
static void
cycleCounterInit(void)
{
    cyclesPerUs = SystemCoreClock / 1000000;

    // enable DWT access
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    // enable the CPU cycle counter
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void
pauseSysTick(void)
{
    SysTick->CTRL &= ~(SysTick_CTRL_CLKSOURCE_Msk |
                       SysTick_CTRL_TICKINT_Msk |
                       SysTick_CTRL_ENABLE_Msk);
}