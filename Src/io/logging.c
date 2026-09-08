/** @file 		logging.c
 *  @brief
 *  	This file logs data to the terminal if connected.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 APR 2022
 */

#include "logging.h"

#include "drv_printf.h"
#include "drv_system.h"
#include "drv_usart6.h"
#include "drv_adc.h"
#include "feature_config.h"
#include "scheduler.h"
#include "battery.h"
#include "process_commands.h"
#include "compute_axis_commands.h"
#include "receiver.h"
#include "motors.h"
#include "baredrones32.h"

uint8_t log_count = 0;
char* logData = (char*)0x20011000;

/* Static Functions */
#ifdef OPENLAGER
static void writeLog(char* pLog);
#endif

/** @brief Prints logging data to OpenLager based on which type of data you want.
 *
 *	@param logType The type of data you want.
 *  @return Void.
 */
void printLog(void)
{
    logs_t logType = MOTORS_LOG;
#ifdef OPENLAGER
    memset(logData, '\0', LOG_SIZE);
    if (logType < 3)
    {
        sprintf(logData, "%lu %d %.3f %d %.3f %.3f %.3f %.3f %.3f %.3f \n",
                micros(),
                logType,
                battVoltage,
                flightMode,
                rateCmd[logType],           // commanded roll rate in rad/s
                sensors.gyro[logType],      // actual gyro roll rate in rad/s
                ratePID[logType],           // PID output from previous values
                attCmd[logType],            // commanded roll angle in rad
                sensors.attitude[logType],  // actual roll angle in rad
                attPID[logType]);           // PID output from previous values
    }

    if (logType == MOTORS_LOG)
    {
        sprintf(logData, "%d %f %d %u %u %u %u \n",
                logType,
                battVoltage,
                flightMode,
                motor_value[MOTOR1],
                motor_value[MOTOR2],
                motor_value[MOTOR3],
                motor_value[MOTOR4]);
    }
    if (logType == SCHEDULER_LOG)
    {
        sprintf(logData, "%u %u %u %u %u %u %u %u %u %u %u %u \n",
                logType,
                deltaTime[FRAME_8000HZ],
                deltaTime[FRAME_4000HZ],
                deltaTime[FRAME_2000HZ],
                deltaTime[FRAME_1000HZ],
                deltaTime[FRAME_500HZ],
                deltaTime[FRAME_200HZ],
                deltaTime[FRAME_100HZ],
                deltaTime[FRAME_50HZ],
                deltaTime[FRAME_10HZ],
                deltaTime[FRAME_5HZ],
                deltaTime[FRAME_1HZ]);
    }
    if (logType == NO_LOG)
    {
        logData = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\r\n";
    }
    writeLog(logData);
#else
    if (logType < 3)
    {
        printf("%lu\t%d\t%.3f\t%d\t%.3f\t%.3f\t%.2f\t%.3f\t%.3f\t%.3f\n",
                micros(),    
                logType,
                battVoltage,
                flightMode,
                rateCmd[logType],
                sensors.gyro[logType],
                ratePID[logType],
                attCmd[logType],
                sensors.attitude[logType],
                attPID[logType]);
    }

    if (logType == 3)
    {
        printf("%d %f %d %u %u %u %u \r",
               logType,
               battVoltage,
               //    battCurrent,
               flightMode,
               motor_value[MOTOR1],
               motor_value[MOTOR2],
               motor_value[MOTOR3],
               motor_value[MOTOR4]);
    }
    if (logType == BATT_LOG)
    {
        printf("%u %u\r", rawADC[0], rawADC[1]);
    }
#endif
}

#ifdef OPENLAGER
/** @brief Loops through the log.
 *
 *  @param ch The character to send.
 *  @return Void.
 */
static void
writeLog(char* pLog)
{
#ifdef OPENLAGER
    // static uint8_t testData[] = {
    //     'A', 'B', 'C', 'D',
    //     0x7D, 0x00, 0x00, 0x00,   // 125
    //     0xFA, 0x00, 0x00, 0x00,   // 250
    //     '\r', '\n'
    // };

    // usart6Write(testData, sizeof(testData));
    // uint8_t lagerSize = sizeof(deltaTime);
    // usart6Write((uint8_t*)deltaTime, lagerSize);

    uint8_t lagerSize = strlen(pLog);
    usart6Write(pLog, lagerSize);
#else
    printf("%s", pLog);
#endif
}
#endif