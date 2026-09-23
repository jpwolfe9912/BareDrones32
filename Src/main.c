/** @file 		main.c
 *  @brief
 *  	This file runs all the other functions and carries out the scheduling of tasks.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

 /* Includes */
#include <stdint.h>
#include <stdbool.h>
#include <cmsis_gcc.h>

#include "feature_config.h"
#include "drv_system.h"
#include "drv_usart3.h"
#include "scheduler.h"
#include "mpu6000.h"
#include "rotations.h"
#include "madgwick.h"
#include "process_commands.h"
#include "compute_axis_commands.h"
#include "mixer.h"
#include "motors.h"
#include "ibus.h"
#include "crsf.h"
#include "logging.h"
#include "w25q128.h"
#include "battery.h"
#include "drv_led.h"
#include "baredrones32.h"

#ifndef SANDBOX

sensors_t sensors;

int main(void)
{
    systemReady = false;

    systemInit();

    /* Add tasksb  */
    Tasks* execTasks[TOTAL_LOOPS] = { NULL };

#ifdef USE_MPU6000
    append(&execTasks[FRAME_8000HZ], readMPU6000);
#endif
#ifdef USE_MOTION_PROCESSING
    append(&execTasks[FRAME_8000HZ], computeRotations);
    append(&execTasks[FRAME_8000HZ], updateIMU);
    append(&execTasks[FRAME_8000HZ], updateAttitude);
    append(&execTasks[FRAME_8000HZ], processCommands);
    append(&execTasks[FRAME_8000HZ], computeAxisCommands);
    append(&execTasks[FRAME_8000HZ], mixTable);
#endif

#ifdef USE_DSHOT
    append(&execTasks[FRAME_8000HZ], motorUpdate);
#endif
#ifdef USE_IBUS
    append(&execTasks[FRAME_200HZ], ibusProcess);
#endif
#ifdef USE_CRSF
    append(&execTasks[FRAME_500HZ], crsfProcess);
#endif

#ifdef USE_LOGGING
#ifdef WIRED_LOGGING
    append(&execTasks[FRAME_100HZ], wiredLoggerUpdate);
#endif
#ifdef USE_W25Q128
    append(&execTasks[FRAME_8000HZ], w25q128Process);
    append(&execTasks[FRAME_1000HZ], flashLoggerProcess);
    append(&execTasks[FRAME_1000HZ], flashLoggerUpdate);
#endif
#endif

#ifdef USE_BATT_MON
    append(&execTasks[FRAME_5HZ], battMonRead);
#endif
#ifdef USE_LEDS
    // append(&execTasks[FRAME_1HZ], ledsSet);
#endif
    systemReady = true;
    while (1)
    {
        run(execTasks);
    }
}

#else
void testFunc(void);

sensors_t sensors;

int main(void)
{
    systemInit();

    // Tasks* execTasks[TOTAL_LOOPS] = { NULL };

    // append(&execTasks[FRAME_8000HZ], w25q128Process);
    // append(&execTasks[FRAME_8000HZ], testFunc);


    uint32_t test;
    w25q128ReadJedecId(&test);
    printf("Value: 0x%X\n", test);

    // uint8_t tx[768];
    // uint8_t rx[1000];

    // for (uint16_t i = 0; i < sizeof(tx); i++)
    //     tx[i] = i;

    // w25q128SectorErase(0);

    // w25q128Read(0, 0, rx, 1000);

    // w25q128PageProgramDMA(2, 200, tx, 600);
    // bool test_finished = false;

    // systemReady = true;

    while (1)
    {
        w25q128ReadJedecId(&test);
        printf("Value: 0x%X\n", test);
        delay(1);
        // run(execTasks);

        // if (w25q128IsReady() && !test_finished)
        // {
        //     w25q128Read(2, 190, rx, 1000);
        //     test_finished = true;
        // }
    }
}
volatile uint32_t var = 0;
void testFunc(void)
{
    var++;
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

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
     /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
