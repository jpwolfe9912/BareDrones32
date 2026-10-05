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
#include "lwrb.h"
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
static volatile uint32_t wired_dropped_records = 0;
#endif

#ifdef USE_W25Q128
/* Macros */
#define FLASH_LOG_BUFFER_SIZE       1024U
#define FLASH_RECORDS_PER_BUFFER    (FLASH_LOG_BUFFER_SIZE / sizeof(flashLogRecord_t))        // 13
#define FLASH_LOG_WRITE_SIZE        (FLASH_RECORDS_PER_BUFFER * sizeof(flashLogRecord_t))     // 13*78=1014

#define FLASH_DUMP_CHUNK_SIZE       FLASH_LOG_WRITE_SIZE

#define FLASH_SCAN_RECORDS      64U
#define FLASH_SCAN_SIZE         (FLASH_SCAN_RECORDS * sizeof(flashLogRecord_t))

/**************************************************** */
/* Static Variables for Collecting Data*/
static uint8_t flash_scan_buffer[FLASH_SCAN_SIZE];

static uint8_t flash_buffer_A[FLASH_LOG_BUFFER_SIZE];
static uint8_t flash_buffer_B[FLASH_LOG_BUFFER_SIZE];

static uint8_t* flash_active_buffer = flash_buffer_A;
static uint8_t* flash_pending_buffer = NULL;
static uint16_t flash_pending_length = 0;

static uint16_t flash_record_count = 0;

static uint32_t flash_sequence = 0;
static uint32_t flash_address = 0;

static uint32_t flash_dropped_records = 0;

static bool flash_write_active = false;     // Set true when the SPI program finishes. \
                                               Only set false when we read the status register to be not busy on the chip

/* Static Variables for Dumping Data */
static uint8_t flash_dump_buffer[FLASH_DUMP_CHUNK_SIZE];
static uint32_t flash_dump_position = 0;
static uint32_t flash_dump_length = 0;
static volatile bool flash_dump_active = false;      // whether a dump is currently in progress
static volatile bool flash_dump_tx_complete = false; // whether an individual chunk of the dump has been sent. Changed by TX callback
static bool flash_dump_header_sent = false;

static uint8_t flash_dump_header[10];

/* Static Functions */
static void flash_logger_find_end(void);
static bool flash_logger_store_record(const flashLogRecord_t* record);
static void flash_logger_buffer_full(void);

#endif

void
loggerInit(void)
{
#ifdef USE_W25Q128
    uint32_t dummy = 0;
    do
    {
        w25q128ReadJedecId(&dummy);
    } while (dummy != BAREDRONES_JEDEC_ID);

    flash_logger_find_end();
#endif
#ifdef WIRED_LOGGING
    usart3RegisterCallback(loggerComplete);
#else
    usart3RegisterCallback(NULL);
#endif
}



#ifdef WIRED_LOGGING
/* Static Functions */
static void logger_transmit(void);

void
wiredLoggerUpdate(void)
{
#ifdef USE_W25Q128
    if (flash_dump_active)
        return;
#endif

    if (queued_records >= LOG_QUEUE_SIZE)
    {
        wired_dropped_records++;
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

    write_index++;

    if (write_index >= LOG_QUEUE_SIZE)
        write_index = 0;

    queued_records++;

    logger_transmit();
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

    logger_transmit();
}

static void
logger_transmit(void)
{
    if (queued_records == 0)
        return;
    usart3Tx((uint8_t*)&logQueue[read_index], sizeof(wiredLogRecord_t));
}
#endif

#ifdef USE_W25Q128
static void
flash_logger_find_end(void)
{
    flashLogRecord_t record;
    uint32_t scan_address = 0;

    flash_address = 0;
    flash_sequence = 0;

    while (scan_address + FLASH_SCAN_SIZE <= W25Q128_CHIP_SIZE)
    {
        uint32_t page = scan_address / W25Q128_PAGE_SIZE;
        uint16_t offset = scan_address % W25Q128_PAGE_SIZE;

        w25q128Read(page, offset, flash_scan_buffer, FLASH_SCAN_SIZE);

        for (uint16_t i = 0; i < FLASH_SCAN_RECORDS; i++)
        {
            memcpy(&record, &flash_scan_buffer[i * sizeof(flashLogRecord_t)], sizeof(flashLogRecord_t));

            if ((record.sequence == 0xFFFFFFFF) && (record.timeUs == 0xFFFFFFFF))
            {
                flash_address = scan_address + (i * sizeof(flashLogRecord_t));
                printf("Last sector: %u\n", (uint32_t)(scan_address / W25Q128_SECTOR_SIZE));
                return;
            }
            flash_sequence = record.sequence + 1;
        }

        scan_address += FLASH_SCAN_SIZE;
    }

    /*
     * Handle whatever remains at the end of the chip if its
     * capacity isn't evenly divisible by FLASH_SCAN_SIZE.
     */
    while (scan_address + sizeof(flashLogRecord_t) <= W25Q128_CHIP_SIZE)
    {
        uint32_t page = scan_address / W25Q128_PAGE_SIZE;
        uint16_t offset = scan_address % W25Q128_PAGE_SIZE;

        w25q128Read(
            page,
            offset,
            (uint8_t*)&record,
            sizeof(record)
        );

        if (record.sequence == 0xFFFFFFFF &&
            record.timeUs == 0xFFFFFFFF)
        {
            flash_address = scan_address;
            return;
        }

        flash_sequence = record.sequence + 1;
        scan_address += sizeof(flashLogRecord_t);
    }

    /* We made it to the end without finding erased flash */
    flash_address = scan_address;
}


/** @brief Starts the flash transfer process if data is ready
 *
 *  @return Void.
 */
void
flashLoggerProcess(void)
{
    /* Flush remaining records after disarm */
    if (!armed && (flash_record_count > 0) && (flash_pending_buffer == NULL) && !flash_write_active)
    {
        flash_pending_buffer = flash_active_buffer;
        flash_pending_length =
            flash_record_count * sizeof(flashLogRecord_t);

        if (flash_active_buffer == flash_buffer_A)
            flash_active_buffer = flash_buffer_B;
        else
            flash_active_buffer = flash_buffer_A;

        flash_record_count = 0;
    }

    if (!flash_write_active)
    {
        if (flash_pending_buffer == NULL)
            return;

        uint32_t page = flash_address / W25Q128_PAGE_SIZE;
        uint16_t offset = flash_address % W25Q128_PAGE_SIZE;

        W25Q128_Status_e status = w25q128PageProgramDMA(page, offset, flash_pending_buffer, flash_pending_length);

        if (status == W25Q128_SUCCESS)
            flash_write_active = true;

        return;
    }

    if (w25q128IsIdle())
    {
        flash_address += flash_pending_length;

        flash_pending_buffer = NULL;
        flash_write_active = false;
    }
}

/** @brief Updates the buffers with new data.
 *
 *  @return Void.
 */
void
flashLoggerUpdate(void)
{
    /* Only record logs if: armed, not dumping */
    if (!armed || flash_dump_active)
        return;

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

    if (!flash_logger_store_record(&record))  // store the record into one of the two buffers
    {
        flash_dropped_records++;
        return;
    }
}

/* Flash Logger Static Functions */
/** @brief Stores the log record into a buffer.
 *
 *  @param record. Record to be stored.
 *  @return Bool. Successful or not
 */
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
    flash_pending_length = FLASH_LOG_WRITE_SIZE;

    if (flash_active_buffer == flash_buffer_A)
        flash_active_buffer = flash_buffer_B;
    else
        flash_active_buffer = flash_buffer_A;

    flash_record_count = 0;
}

void
flashLoggerErase(void)
{
    if (armed)
        return;

    if (flash_dump_active)
        return;

    if (flash_write_active)
        return;

    if (!w25q128IsIdle())
        return;

    uint16_t last_sector = (flash_address + W25Q128_SECTOR_SIZE - 1) / W25Q128_SECTOR_SIZE;
    w25q128SequentialSectorErase(0, last_sector);

    flash_address = 0;
    flash_sequence = 0;

    flash_record_count = 0;
    flash_pending_buffer = NULL;
    flash_pending_length = 0;
    flash_write_active = false;

    flash_active_buffer = flash_buffer_A;

    return;
}

/* Logger Dump Functions */
/** @brief Processes the data to be dumped.
 *
 *  @return Void.
 */
void
flashLoggerDumpProcess(void)
{
    if (!flash_dump_tx_complete)    // should skip after flash_logger_start_dump() called
        return;

    if (!flash_dump_header_sent)    // true on second run through. Should be false and skip on subsequent runs
    {
        uint32_t record_count = flash_dump_length / sizeof(flashLogRecord_t);

        flash_dump_header[0] = 'B';
        flash_dump_header[1] = 'L';
        flash_dump_header[2] = 'O';
        flash_dump_header[3] = 'G';

        flash_dump_header[4] = (uint8_t)(record_count);
        flash_dump_header[5] = (uint8_t)(record_count >> 8);
        flash_dump_header[6] = (uint8_t)(record_count >> 16);
        flash_dump_header[7] = (uint8_t)(record_count >> 24);

        flash_dump_header[8] =
            (uint8_t)sizeof(flashLogRecord_t);
        flash_dump_header[9] =
            (uint8_t)(sizeof(flashLogRecord_t) >> 8);

        if (usart3Tx(flash_dump_header, sizeof(flash_dump_header))) // send just the header
        {
            flash_dump_tx_complete = false;    // will be set true when the callback runs in the DMA IRQ
            flash_dump_header_sent = true;
        }
        return;     // return and come back later
    }

    if (flash_dump_position >= flash_dump_length) // dump is done!
    {
        flash_dump_active = false;

#ifdef WIRED_LOGGING
        usart3RegisterCallback(loggerComplete);
#endif
        return;
    }

    uint32_t remaining = flash_dump_length - flash_dump_position;

    uint16_t chunk_size = remaining > FLASH_DUMP_CHUNK_SIZE ? FLASH_DUMP_CHUNK_SIZE : (uint16_t)remaining;

    uint32_t page = flash_dump_position / W25Q128_PAGE_SIZE;
    uint16_t offset = flash_dump_position % W25Q128_PAGE_SIZE;

    w25q128Read(page, offset, flash_dump_buffer, chunk_size);   // should hit all this on the 3rd run through
    if (usart3Tx(flash_dump_buffer, chunk_size))                // send chunk
    {
        flash_dump_tx_complete = false;
        flash_dump_position += chunk_size;
    }
}

/** @brief Starts the data dump process by setting appropriate varibles.
 *
 *  @return Bool. Successful or not
 */
void
flashLoggerStartDump(void)
{
    /* All the conditions under which we shouldn't start a dump */
    if (armed)
        return;

    if (flash_dump_active)
        return;

    if (flash_write_active)
        return;

    if (flash_pending_buffer != NULL)
        return;

    if (!w25q128IsIdle())
        return;

    if (usart3TxBusy())
        return;

    flash_dump_position = 0;
    flash_dump_length = flash_address;

    flash_dump_tx_complete = true;
    flash_dump_header_sent = false;

    flash_dump_active = true;       // we aren't adding data to the buffer while dumping it

    usart3RegisterCallback(flashLoggerDumpComplete);
    return;
}

/** @brief Callback for when a chunk of the data dump is complete.
 *
 *  @return Void.
 */
void
flashLoggerDumpComplete(void)
{
    if (flash_dump_active)
        flash_dump_tx_complete = true;
}

#endif