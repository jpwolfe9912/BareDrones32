/** @file 		logging.c
 *  @brief
 *  	This file logs data to the terminal if connected.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 APR 2022
 */

#include "logging.h"

#include "drv_printf.h"
#include "drv_usart6.h"
#include "feature_config.h"
#include "battery.h"
#include "process_commands.h"
#include "compute_axis_commands.h"
#include "receiver.h"
#include "motors.h"
#include "baredrones32.h"

uint8_t log_count = 0;
char *logData = (char *)0x20011000;

/* Static Functions */
#ifdef OPENLAGER
static void writeLog(char *pLog);
#endif

/** @brief Prints logging data to OpenLager based on which type of data you want.
 *
 *	@param logType The type of data you want.
 *  @return Void.
 */
void printLog(void)
{
    logs_t logType = 0;
#ifdef OPENLAGER
    memset(logData, '\0', LOG_SIZE);
    if (logType == 0)
    {
        sprintf(logData, "%.3f, %.3f, %.3f \n",
                sensors.attDeg[0],
                sensors.attDeg[1],
                sensors.attDeg[2]);
    }
    if (logType == 1)
    {
        sprintf(logData, "%d %.3f %d %.3f %.3f %.3f %.3f %.3f %.3f \n",
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
    if (logType == 2)
    {
        sprintf(logData, "%d %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n",
            logType,
            rxCommands[0],
            rxCommands[1],
            rxCommands[2],
            rxCommands[3],
            rxCommands[4],
            rxCommands[5],
            rxCommands[6],
            rxCommands[7]
        );
    }

    if (logType == 3)
    {
        sprintf(logData, "%d %f %d %u %u %u %u \r",
                logType,
                battVoltage,
                flightMode,
                motor_value[MOTOR1],
                motor_value[MOTOR2],
                motor_value[MOTOR3],
                motor_value[MOTOR4]);
    }
    writeLog(logData);
#endif
// #else
//     if (logType < 3)
//     {
//         printf("%d\t%.3f\t%d\t%.3f\t%.3f\t%.2f\t%.3f\t%.3f\t%.3f\n",
//                logType,
//                battVoltage,
//                flightMode,
//                rateCmd[logType],
//                sensors.gyro[logType],
//                ratePID[logType],
//                attCmd[logType],
//                (sensors.attitude[logType]*180.00/3.1415),
//                attPID[logType]);
//     }

//     if (logType == 3)
//     {
//         printf("%d %f %d %u %u %u %u \r",
//                logType,
//                battVoltage,
//             //    battCurrent,
//                flightMode,
//                motor_value[MOTOR1],
//                motor_value[MOTOR2],
//                motor_value[MOTOR3],
//                motor_value[MOTOR4]);
//     }
//     if (logType == 4)
//     {
//         // printf("%d %u, %u, %u, %u \r",
//         // logType,
//         // ibusChannels[0],
//         // ibusChannels[1],
//         // ibusChannels[2],
//         // ibusChannels[3]);
//         printf("%u %u\r",rawADC[0], rawADC[1]);
//     }
// #endif
}

#ifdef OPENLAGER
/** @brief Loops through the log.
 *
 *  @param ch The character to send.
 *  @return Void.
 */
static void
writeLog(char *pLog)
{
    // uint8_t lagerSize = strlen(pLog);
    // usart6Write(pLog, lagerSize);
    printf("%s", pLog);
}
#endif