/** @file 		crsf.h
 *  @brief
 *  	This file contains all the functions for crossfire compatibility.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

#include "crsf.h"

#include "lwrb.h"
#include "baredrones32.h"
#include "drv_system.h"
#include "drv_usart3.h"
#include "drv_usart2.h"
#include "drv_color.h"
#include "receiver.h"

static crsfFrame_t crsfFrame;
static crsfFrame_t crsfChannelDataFrame;
static uint16_t crsfChannelData[CRSF_MAX_CHANNEL];

/*
 * CRSF protocol
 *
 * CRSF protocol uses a single wire half duplex uart connection.
 * The master sends one frame every 4ms and the slave replies between two frames from the master.
 *
 * 420000 baud
 * not inverted
 * 8 Bit
 * 1 Stop bit
 * Big endian
 * 420000 bit/s = 46667 byte/s (including stop bit) = 21.43us per byte
 * Max frame size is 64 bytes
 * A 64 byte frame plus 1 sync byte can be transmitted in 1393 microseconds.
 *
 * CRSF_TIME_NEEDED_PER_FRAME_US is set conservatively at 1500 microseconds
 *
 * Every frame has the structure:
 * <Device address><Frame length><Type><Payload><CRC>
 *
 * Device address: (uint8_t)
 * Frame length:   length in  bytes including Type (uint8_t)
 * Type:           (uint8_t)
 * CRC:            (uint8_t)
 *
 */

struct crsfPayloadRcChannelsPacked_s {
    // 176 bits of data (11 bits per channel * 16 channels) = 22 bytes.
    unsigned int chan0 : 11;
    unsigned int chan1 : 11;
    unsigned int chan2 : 11;
    unsigned int chan3 : 11;
    unsigned int chan4 : 11;
    unsigned int chan5 : 11;
    unsigned int chan6 : 11;
    unsigned int chan7 : 11;
    unsigned int chan8 : 11;
    unsigned int chan9 : 11;
    unsigned int chan10 : 11;
    unsigned int chan11 : 11;
    unsigned int chan12 : 11;
    unsigned int chan13 : 11;
    unsigned int chan14 : 11;
    unsigned int chan15 : 11;
} __attribute__((__packed__));

typedef struct crsfPayloadRcChannelsPacked_s crsfPayloadRcChannelsPacked_t;

uint8_t syncByte;
static crsfStatus_e status;
static uint8_t init_error = 0;

static crsfStatus_e crsf_process_frame(void);
static void crsfProcessPayload(uint8_t* payload);
static uint8_t crsfCrc8(const uint8_t* data, uint8_t len);

bool crsfInit(void)
{
    bool crsf_initialized = false;

    printf("\nCrossfire initializing \n");

    lwrb_init(&Buff_2.RxBuffer, (void*)Buff_2.RxBuffer_Data, sizeof(Buff_2.RxBuffer_Data));

    usart2BeginRx();

    status = CRSF_ERROR;

    for (uint8_t i = 0; i < 10; i++)
    {
        crsfProcess();
        if (syncByte == 0xC8)
        {
            crsf_initialized = true;
            rcData.connected = true;
            break;
        }
        delay(5);
    }

    if (!crsf_initialized)
    {
        color(RED, YES);
        printf("Crossfire initialization failed. Try again?\n");
#ifdef STLINK
        if (printfWaitFor('y'))
            return true;
        else
            return false;
#endif
        if (init_error++ > 10)
        {
            printf("\nToo many failed attempts\n");
            color(WHITE, NO);
            return true;
        }
        color(WHITE, NO);
        return false;
    }
    else
    {
        color(GREEN, YES);
        printf("\nCrossfire receiver recognized\n");
        colorDefault();
        crsfProcess();
        return true;
    }
    return true;
}

/** @brief Waits for the parser to not be busy
 *
 *  @return Void.
 */
void crsfProcess(void)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        crsfStatus_e result = crsf_process_frame();

        if (result == CRSF_BUSY)
            break;
    }
}

/** @brief State machine that evaluates the raw data based on what section it is parsing through.
 *
 *  @return ibusStatus_e The status of the function.
 *  	READY, BUSY, ERROR
 */
static crsfStatus_e
crsf_process_frame(void)
{

    uint8_t frame_buff[CRSF_FRAME_SIZE_MAX];   // temporary storage for frame data
    uint8_t header[2];
    uint8_t frame_size;

    crsfFrameParts_e part = CRSF_SYNCBYTE;      // which part we're reading from
    uint8_t frame_pos = 0;          // which byte to read
    crsfFrameDef_t frame = {};      // contains all the raw data from the packet

    /*
     * Need at least:
     * [0] device address
     * [1] frame length
     */
    if (lwrb_get_full(&Buff_2.RxBuffer) < 2)
    {
        return CRSF_BUSY;
    }

    lwrb_peek(&Buff_2.RxBuffer, 0, header, 2);

    if (header[0] != 0xC8)
    {
        uint8_t garbage;
        lwrb_read(&Buff_2.RxBuffer, &garbage, 1);
        return CRSF_ERROR;
    }

    frame_size = header[1] + 2;

    if (frame_size > CRSF_FRAME_SIZE_MAX)
    {
        uint8_t garbage;
        lwrb_read(&Buff_2.RxBuffer, &garbage, 1);
        return CRSF_ERROR;
    }

    if (lwrb_get_full(&Buff_2.RxBuffer) < frame_size)
    {
        return CRSF_BUSY;
    }

    lwrb_read(&Buff_2.RxBuffer, frame_buff, frame_size);

    uint8_t type = frame_buff[2];

    uint8_t received_crc = frame_buff[frame_size - 1];

    uint8_t calculated_crc = crsfCrc8(&frame_buff[2], header[1] - 1);

    if (calculated_crc != received_crc)
    {
        return CRSF_ERROR;
    }

    if (type == 0x16)
    {
        uint8_t* payload = &frame_buff[3];

        crsfProcessPayload(payload);
    }

    syncByte = frame_buff[0];

    return CRSF_READY;
}

static void crsfProcessPayload(uint8_t* payload)
{
    crsfPayloadRcChannelsPacked_t* rcChannels = (crsfPayloadRcChannelsPacked_t*)payload;
    crsfChannelData[0] = TICKS_TO_US(rcChannels->chan0);
    crsfChannelData[1] = TICKS_TO_US(rcChannels->chan1);
    crsfChannelData[2] = TICKS_TO_US(rcChannels->chan2);
    crsfChannelData[3] = TICKS_TO_US(rcChannels->chan3);
    crsfChannelData[4] = TICKS_TO_US(rcChannels->chan4);
    crsfChannelData[5] = TICKS_TO_US(rcChannels->chan5);
    crsfChannelData[6] = TICKS_TO_US(rcChannels->chan6);
    crsfChannelData[7] = TICKS_TO_US(rcChannels->chan7);
    crsfChannelData[8] = TICKS_TO_US(rcChannels->chan8);
    crsfChannelData[9] = TICKS_TO_US(rcChannels->chan9);
    crsfChannelData[10] = TICKS_TO_US(rcChannels->chan10);
    crsfChannelData[11] = TICKS_TO_US(rcChannels->chan11);
    crsfChannelData[12] = TICKS_TO_US(rcChannels->chan12);
    crsfChannelData[13] = TICKS_TO_US(rcChannels->chan13);
    crsfChannelData[14] = TICKS_TO_US(rcChannels->chan14);
    crsfChannelData[15] = TICKS_TO_US(rcChannels->chan15);
    updateRcChannels(crsfChannelData);
}

/**
 * @brief Calculate CRSF CRC8 DVB-S2
 *
 * @param data Pointer to first byte included in CRC
 * @param len  Number of bytes to process
 *
 * @return Calculated CRC
 */
static uint8_t crsfCrc8(const uint8_t* data, uint8_t len)
{
    uint8_t crc = 0;

    while (len--)
    {
        crc ^= *data++;

        for (uint8_t i = 0; i < 8; i++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0xD5;
            else
                crc <<= 1;
        }
    }

    return crc;
}