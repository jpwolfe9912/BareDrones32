/** @file 		w25q128.c
 *  @brief
 *  	This file contains the functions to read and write data
 *      to the w25q128 chip
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		19 FEB 2026
 */

#include "w25q128.h"

#include "feature_config.h"
#include "drv_system.h"
#include "drv_spi2.h"

 /* Static variables */
static W25Q128_State_e w25q128_state = W25Q128_STATE_IDLE;

static uint32_t program_page;
static uint16_t program_offset;
static uint8_t* program_data;
static uint32_t program_length;
static uint32_t program_position;
static uint16_t program_bytes_current_page;

/* Static function prototypes */
static W25Q128_Status_e w25q128_write_enable(void);
static W25Q128_Status_e w25q128_write_disable(void);
static W25Q128_Status_e w25q128_read_status_reg1(uint8_t* status);
static bool w25q128_is_busy(void);
static void w25q128_start_page_program(void);

void
w25q128Init(void)
{
    FLASH_CS_HIGH;

    uint32_t id = 0x000000;
    do
    {
        w25q128ReadJedecId(&id);
        delay(1);
    }
#ifdef USE_BAREDRONES
    while (id != BAREDRONES_JEDEC_ID);
#else
    while (id != NUCLEO_JEDEC_ID);
#endif
}

static W25Q128_Status_e
w25q128_write_enable(void)
{
    FLASH_CS_LOW;

    spi2TransferByte(INST_WRITE_ENABLE);

    FLASH_CS_HIGH;

    return W25Q128_SUCCESS;
}

static W25Q128_Status_e
w25q128_write_disable(void)
{
    FLASH_CS_LOW;

    spi2TransferByte(INST_WRITE_DISABLE);

    FLASH_CS_HIGH;

    return W25Q128_SUCCESS;
}

W25Q128_Status_e
w25q128ReadJedecId(uint32_t* id)
{
    uint8_t temp_id[3];

    FLASH_CS_LOW;

    spi2TransferByte(INST_JEDEC_ID);

    spi2ReadBytes(temp_id, 3);

    FLASH_CS_HIGH;

    *id = ((uint32_t)temp_id[0] << 16) |
        ((uint32_t)temp_id[1] << 8) |
        temp_id[2];

    return W25Q128_SUCCESS;
}

W25Q128_Status_e
w25q128Read(uint32_t start_page, uint16_t offset, uint8_t* data, uint16_t length)
{
    uint32_t address = (start_page * 256) + offset;     // 256 bytes in a page

    FLASH_CS_LOW;

    spi2TransferByte(INST_READ_DATA);

    spi2TransferByte((address >> 16) & 0xFF);
    spi2TransferByte((address >> 8) & 0xFF);
    spi2TransferByte(address & 0xFF);

    spi2ReadBytes(data, length);

    FLASH_CS_HIGH;

    return W25Q128_SUCCESS;
}

W25Q128_Status_e
w25q128SectorErase(uint16_t num_sector)
{
    uint32_t address = num_sector * 16 * 256; // 16 pages, 256 bytes

    w25q128_write_enable();

    FLASH_CS_LOW;

    spi2TransferByte(INST_SECTOR_ERASE_4KB);

    spi2TransferByte((address >> 16) & 0xFF);
    spi2TransferByte((address >> 8) & 0xFF);
    spi2TransferByte(address & 0xFF);

    FLASH_CS_HIGH;

    w25q128WaitUntilReady();

    return W25Q128_SUCCESS;
}

W25Q128_Status_e
w25q128PageProgram(uint32_t page, uint16_t offset, uint8_t* data, uint16_t length)
{
    uint32_t address = (page * 256) + offset;

    w25q128_write_enable(); // first enable writes 

    FLASH_CS_LOW; spi2TransferByte(INST_PAGE_PROGRAM);

    spi2TransferByte((address >> 16) & 0xFF);
    spi2TransferByte((address >> 8) & 0xFF);
    spi2TransferByte(address & 0xFF);
    spi2WriteBytes(data, length);

    FLASH_CS_HIGH;

    w25q128WaitUntilReady();

    return W25Q128_SUCCESS;
}

W25Q128_Status_e
w25q128PageProgramDMA(uint32_t page, uint16_t offset, uint8_t* data, uint32_t length)
{
    if (w25q128_state != W25Q128_STATE_IDLE)
        return W25Q128_BUSY;

    /*
     * Validate page, offset, length, total flash bounds, etc.
     */

    program_page = page;
    program_offset = offset;
    program_data = data;
    program_length = length;
    program_position = 0;

    w25q128_start_page_program();

    return W25Q128_SUCCESS;
}

static void
w25q128_start_page_program(void)
{
    uint32_t address;
    uint16_t bytes_to_write;

    bytes_to_write = W25Q128_PAGE_SIZE - program_offset;

    uint32_t bytes_remaining = program_length - program_position;

    if (bytes_to_write > bytes_remaining)
        bytes_to_write = bytes_remaining;

    address = (program_page * W25Q128_PAGE_SIZE) + program_offset;

    w25q128_write_enable();

    FLASH_CS_LOW;

    spi2TransferByte(INST_PAGE_PROGRAM);

    spi2TransferByte((address >> 16) & 0xFF);
    spi2TransferByte((address >> 8) & 0xFF);
    spi2TransferByte(address & 0xFF);

    spi2WriteBytesDMA(
        &program_data[program_position],
        bytes_to_write
    );

    /*
     * Remember how much this particular page is writing.
     */
    program_bytes_current_page = bytes_to_write;

    w25q128_state = W25Q128_STATE_DMA_TRANSFER;
}

void
w25q128Process(void)
{
    switch (w25q128_state)
    {
    case W25Q128_STATE_IDLE:
        break;                          // if nothing is going on, break

    case W25Q128_STATE_DMA_TRANSFER:
        if (!spi2DMAWriteComplete())    // if DMA isn't done, break and come back later
            break;

        spi2EndDMAWrite();              // if DMA is done, finish the DMA write

        FLASH_CS_HIGH;

        w25q128_state = W25Q128_STATE_PROGRAMMING;

        break;

    case W25Q128_STATE_PROGRAMMING:
        if (w25q128_is_busy())          // DMA is done but flash hasn't processed the data yet
            break;

        program_position += program_bytes_current_page;

        if (program_position >= program_length)
        {
            w25q128_state = W25Q128_STATE_IDLE; // all done
            break;
        }
        program_page++;
        program_offset = 0;

        w25q128_start_page_program();

        break;
    }
}

bool
w25q128IsIdle(void)
{
    return (w25q128_state == W25Q128_STATE_IDLE);
}

static W25Q128_Status_e
w25q128_read_status_reg1(uint8_t* status)
{
    FLASH_CS_LOW;

    spi2TransferByte(INST_READ_STATUS_REG_1);

    *status = spi2TransferByte(0x00);

    FLASH_CS_HIGH;

    return W25Q128_SUCCESS;
}

static bool
w25q128_is_busy(void)
{
    uint8_t status;
    w25q128_read_status_reg1(&status);
    return (status & 0x01);
}

W25Q128_Status_e
w25q128WaitUntilReady(void)
{
    while (w25q128_is_busy())
        ;
    return W25Q128_SUCCESS;
}

// void testFunc(void)
// {
//     volatile uint8_t statusBefore;
//     volatile uint8_t statusAfterEnable;
//     volatile uint8_t statusAfterDisable;

//     statusBefore = w25q128_read_status_reg1();

//     w25q128_write_enable();
//     statusAfterEnable = w25q128_read_status_reg1();

//     w25q128_write_disable();
//     statusAfterDisable = w25q128_read_status_reg1();

//     uint8_t dummy;
// }