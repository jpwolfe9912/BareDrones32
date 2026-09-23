/** @file 		logging.c
 *  @brief
 *  	This file logs data to the terminal if connected.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 APR 2022
 */

#include "logging.h"

#include "drv_usart3.h"
#include "drv_system.h"
#include "drv_usart6.h"
#include "drv_adc.h"
#include "drv_led.h"
#include "scheduler.h"
#include "battery.h"
#include "process_commands.h"
#include "compute_axis_commands.h"
#include "receiver.h"
#include "motors.h"
#include "mixer.h"
#include "w25q128.h"
#include "baredrones32.h"

#ifdef WIRED_LOGGING
#define LOG_QUEUE_SIZE 8

#define LOG_HEADER 0xDEAD
#define LOG_FOOTER 0xBEEF

static wiredLogRecord_t logQueue[LOG_QUEUE_SIZE];

static volatile uint8_t write_index = 0;
static volatile uint8_t read_index = 0;
static volatile uint8_t queued_records = 0;

static uint16_t wired_sequence = 0;
static volatile uint32_t dropped_records = 0;
#endif

#ifdef USE_W25Q128
#define FLASH_LOG_BUFFER_SIZE       1024
#define FLASH_RECORDS_PER_BUFFER    FLASH_LOG_BUFFER_SIZE / sizeof(flashLogRecord_t)        // 13
#define FLASH_LOG_WRITE_SIZE        FLASH_RECORDS_PER_BUFFER * sizeof(flashLogRecord_t)     // 13*78=1014

static uint8_t flash_buffer_A[FLASH_LOG_BUFFER_SIZE];
static uint8_t flash_buffer_B[FLASH_LOG_BUFFER_SIZE];

static uint8_t* flash_active_buffer = flash_buffer_A;
static uint8_t* flash_pending_buffer = NULL;

static uint16_t flash_record_count = 0;

static uint32_t flash_sequence = 0;
static uint32_t flash_address = 0;

volatile bool flash_update_active = true;
static uint8_t test_read_buffer[1014];

static uint32_t flash_dropped_records = 0;

static bool flash_write_active = false;

static bool flash_logger_store_record(const flashLogRecord_t* record);
static void flash_logger_buffer_full(void);
#endif



#ifdef WIRED_LOGGING
/* Static Functions */
static void loggerTransmit(void);

void
wiredLoggerUpdate(void)
{
    if (queued_records >= LOG_QUEUE_SIZE)
    {
        dropped_records++;
        return;
    }

    wiredLogRecord_t* record = &logQueue[write_index];

    record->header = LOG_HEADER;
    record->sequence = wired_sequence++;
    record->timeUs = micros();

    record->flightMode = flightMode;
    record->armed = armed;
    record->reserved = 0;

    record->battVoltage = battVoltage;

    record->throttleCmd = throttleCmd - 2000;

    record->motorOutput[0] = motor_value[0];
    record->motorOutput[1] = motor_value[1];
    record->motorOutput[2] = motor_value[2];
    record->motorOutput[3] = motor_value[3];

    for (uint8_t axis = 0; axis < 3; axis++)
    {
        record->rateCmd[axis] = rateCmd[axis];
        record->gyro[axis] = sensors.gyro[axis];
        record->ratePID[axis] = ratePID[axis];

        record->attCmd[axis] = attCmd[axis];
        record->attitude[axis] = sensors.attitude[axis];
        record->attPID[axis] = attPID[axis];
    }

    record->footer = LOG_FOOTER;

    writeIndex++;

    if (write_index >= LOG_QUEUE_SIZE)
        write_index = 0;

    queued_records++;

    loggerTransmit();
}

static void
loggerTransmit(void)
{
    if (queued_records == 0)
        return;
    usart3Write((uint8_t*)&logQueue[read_index], sizeof(wiredLogRecord_t));
}

void
loggerComplete(void)
{
    if (queued_records > 0)
    {
        read_index++;

        if (read_index >= LOG_QUEUE_SIZE)
            read_index = 0;

        queued_records--;
    }

    loggerTransmit();
}
#endif

#ifdef USE_W25Q128
void
flashLoggerProcess(void)
{
    if (!flash_write_active)
    {
        if (flash_pending_buffer == NULL)
            return;

        uint32_t page = flash_address / W25Q128_PAGE_SIZE;
        uint16_t offset = flash_address % W25Q128_PAGE_SIZE;

        W25Q128_Status_e status = w25q128PageProgramDMA(page, offset, flash_pending_buffer, FLASH_LOG_WRITE_SIZE);

        if (status == W25Q128_SUCCESS)
            flash_write_active = true;

        return;
    }

    if (w25q128IsIdle())
    {
        flash_address += FLASH_LOG_WRITE_SIZE;

        flash_pending_buffer = NULL;
        flash_write_active = false;
    }
}


void
flashLoggerUpdate(void)
{
    if (flash_update_active)
    {
        flashLogRecord_t record;

        record.sequence = flash_sequence++;
        record.timeUs = micros();

        for (uint8_t axis = 0; axis < 3; axis++)
        {
            record.rateCmd[axis] = rateCmd[axis];
            record.gyro[axis] = sensors.gyro[axis];
            record.rateP[axis] = pidState[axis].pTerm;
            record.rateI[axis] = pidState[axis].iTerm;
            record.rateD[axis] = pidState[axis].dTerm;
        }
        record.throttleCmd = throttleCmd - 2000;
        for (uint8_t i = 0; i < MOTOR_COUNT; i++)
            record.motor[i] = motor_value[i];

        if (!flash_logger_store_record(&record))
            flash_dropped_records++;
    }
    else
    {
        w25q128Read(0, 0, test_read_buffer, sizeof(test_read_buffer));
        flashLogRecord_t record; 
        memcpy(&record, test_read_buffer, sizeof(test_read_buffer));
    }
    if (flash_sequence >= 130)
        flash_update_active = false;

}

static bool
flash_logger_store_record(const flashLogRecord_t* record)
{
    if (flash_record_count >= FLASH_RECORDS_PER_BUFFER)
        return false;

    memcpy(&flash_active_buffer[flash_record_count * sizeof(flashLogRecord_t)], record, sizeof(flashLogRecord_t));

    flash_record_count++;

    if (flash_record_count >= FLASH_RECORDS_PER_BUFFER)
        flash_logger_buffer_full();

    return true;
}

/** @brief Switches buffer when one becomes full.
 *
 *  @return Void.
 */
static void
flash_logger_buffer_full(void)
{
    if (flash_pending_buffer != NULL)       // if the other buffer is being used, can't switch
        return;

    flash_pending_buffer = flash_active_buffer;

    if (flash_active_buffer == flash_buffer_A)
        flash_active_buffer = flash_buffer_B;
    else
        flash_active_buffer = flash_buffer_A;

    flash_record_count = 0;
}
#endif