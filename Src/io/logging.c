/** @file 		logging.c
 *  @brief
 *  	This file logs data to the terminal if connected.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 APR 2022
 */

#include "board.h"

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
    logs_t logType = 2;
#ifdef OPENLAGER
    memset(logData, '\0', LOG_SIZE);
    if (logType == 0)
    {
        sprintf(logData, "%.3f, %.3f, %.3f \n",
                sensors.attitude500Hz[0],
                sensors.attitude500Hz[1],
                sensors.attitude500Hz[2]);
    }
    if (logType == 1)
    {
        sprintf(logData, "%d %.3f %d %.3f %.3f %.3f %.3f %.3f %.3f \n",
                logType,
                battVoltage,
                flightMode,
                rateCmd[logType],
                sensors.gyro500Hz[logType],
                ratePID[logType],
                attCmd[logType],
                sensors.attitude500Hz[logType],
                attPID[logType]);
    }
    if (logType == 2)
    {
        sprintf(logData, "%d %d %d %d %d %d %d %d %d\n",
            logType,
            rcData.channels[0],
            rcData.channels[1],
            rcData.channels[2],
            rcData.channels[3],
            rcData.channels[4],
            rcData.channels[5],
            rcData.channels[6],
            rcData.channels[7]
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

#else
    if (logType < 3)
    {
        printf("%d\t%.3f\t%d\t%.3f\t%.3f\t%.2f\t%.3f\t%.3f\t%.3f\n",
               logType,
               battVoltage,
               flightMode,
               rateCmd[logType],
               sensors.gyro500Hz[logType],
               ratePID[logType],
               attCmd[logType],
               (sensors.attitude500Hz[logType]*180.00/3.1415),
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
    if (logType == 4)
    {
        // printf("%d %u, %u, %u, %u \r",
        // logType,
        // ibusChannels[0],
        // ibusChannels[1],
        // ibusChannels[2],
        // ibusChannels[3]);
        printf("%u %u\r",rawADC[0], rawADC[1]);
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
writeLog(char *pLog)
{
    // uint8_t lagerSize = strlen(pLog);
    // usart6Write(pLog, lagerSize);
    printf("%s", pLog);
}
#endif