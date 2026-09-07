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
#include "drv_printf.h"
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
#include "battery.h"
#include "drv_led.h"
#include "baredrones32.h"

#ifndef SANDBOX

sensors_t sensors;

int main(void)
{
    systemReady = false;

    systemInit();

    /* Add tasks */
    Tasks* execTasks[TOTAL_LOOPS] = { NULL };

#ifdef USE_MPU6000
    append(&execTasks[FRAME_8000HZ], readMPU6000);
#endif
#ifdef USE_MOTION_PROCESSING
    append(&execTasks[FRAME_8000HZ], computeRotations500Hz); // okay
    append(&execTasks[FRAME_8000HZ], updateIMU);
    append(&execTasks[FRAME_8000HZ], updateAttitude);
    append(&execTasks[FRAME_8000HZ], processCommands); // bad
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
    append(&execTasks[FRAME_50HZ], printLog);
#endif
#ifdef USE_BATT_MON
    append(&execTasks[FRAME_5HZ], battMonRead);
#endif
#ifdef USE_LEDS
    append(&execTasks[FRAME_1HZ], ledsSet);
#endif
    systemReady = true;
    while (1)
    {
        run(execTasks);
    }
}

#else
const uint8_t __attribute__((__section__(".eeprom"), used)) eepromArray[131072];


// void test_func(void);

int main(void)
{
    systemInit();
    usart2BeginRx();
    systemReady = true;
    char c = ' ';
    while (1)
    {
        c = USART2->RDR;
        printf("Received: %c\r\n", c);
        delay(10);
    }
}

// void test_func(void)
// {
//     uint8_t temp_buff[1024];

//     lwrb_read(&Buffs.RxBuffer, temp_buff, lwrb_get_full(&Buffs.RxBuffer));
//     for (int i = 0; i < sizeof(temp_buff); i++)
//         printf("%u", temp_buff[i]);
//     memcpy(temp_buff, '\0', sizeof(temp_buff));
// }

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
