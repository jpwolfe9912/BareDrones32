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

eepromConfig_t eepromConfig;

uint8_t execUpCount = 0;

sensors_t sensors;

uint16_t timerValue;
// this is a test
int main(void)
{
    systemReady = false;

    systemInit();

    /* Add tasks */
    Tasks *execTasks[8] = {NULL};

#ifdef USE_MPU6000
    append(&execTasks[FRAME_1000HZ], readMPU6000);
#endif
#ifdef USE_MOTION_PROCESSING
    append(&execTasks[FRAME_500HZ], computeRotations500Hz);
    append(&execTasks[FRAME_500HZ], updateIMU);
    append(&execTasks[FRAME_500HZ], updateAttitude);
    append(&execTasks[FRAME_500HZ], processCommands);
    append(&execTasks[FRAME_500HZ], computeAxisCommands);
    append(&execTasks[FRAME_500HZ], mixTable);
#endif

#ifdef USE_DSHOT
    append(&execTasks[FRAME_500HZ], dshotWrite);
#endif
#ifdef USE_IBUS
    append(&execTasks[FRAME_200HZ], ibusProcess);
#endif
#ifdef USE_LOGGING
    append(&execTasks[FRAME_100HZ], printLog);
#endif
#ifdef USE_BATT_MON
    append(&execTasks[FRAME_5HZ], battMonRead);
#endif
    append(&execTasks[FRAME_1HZ], ledsSet);
#ifdef USE_LEDS
    systemReady = true;
#endif
    while (1)
    {
        run(execTasks);
    }
}

#else
const uint8_t __attribute__((__section__(".eeprom"), used)) eepromArray[131072];

eepromConfig_t eepromConfig;

uint8_t execUpCount = 0;

sensors_t sensors;

uint16_t timerValue;

void test_func(void);

int main(void)
{
    systemInit();
    systemReady = true;

    printf("\nInitializing iBus Receiver\n");
    usart1BeginRx();
    lwrb_init(&Buffs.RxBuffer, (void *)Buffs.RxBuffer_Data, sizeof(Buffs.RxBuffer_Data));

    Tasks *execTasks[8] = {NULL};
    append(&execTasks[FRAME_100HZ], test_func);

    while (1)
    {
        run(execTasks);
    }
}

void test_func(void)
{
    uint8_t temp_buff[1024];

    lwrb_read(&Buffs.RxBuffer, temp_buff, lwrb_get_full(&Buffs.RxBuffer));
    for (int i = 0; i < sizeof(temp_buff); i++)
        printf("%u", temp_buff[i]);
    memcpy(temp_buff, '\0', sizeof(temp_buff));
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
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
