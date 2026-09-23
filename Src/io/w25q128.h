/** @file 		w25q128.h
 *  @brief
 *  	This file contains the functions to read and write data
 *      to the w25q128 chip
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		19 FEB 2026
 */

#ifndef __W25Q128_H__
#define __W25Q128_H__

#include <stdint.h>
#include <stdbool.h>

#define W25Q128_SECTOR_SIZE     4096U
#define W25Q128_SECTOR_COUNT    4096U
#define W25Q128_PAGE_SIZE       256U
#define W25Q128_PAGE_COUNT      65536U

#define BAREDRONES_JEDEC_ID     0xEF7018
#define NUCLEO_JEDEC_ID         0xEF4018


typedef enum {
    W25Q128_SUCCESS = 0,
    W25Q128_ERROR,
    W25Q128_READY,
    W25Q128_BUSY,
    W25Q128_ERROR_TIMEOUT,
    W25Q128_INVALID_PAGE,
    W25Q128_INVALID_OFFSET,
    W25Q128_INVALID_LENGTH
} W25Q128_Status_e;

typedef enum
{
    W25Q128_STATE_IDLE = 0,
    W25Q128_STATE_DMA_TRANSFER,
    W25Q128_STATE_PROGRAMMING
} W25Q128_State_e;

typedef enum {
    INST_WRITE_ENABLE = 0x06,
    INST_VOLATILE_SR_WRITE_ENABLE = 0x50,
    INST_WRITE_DISABLE = 0x04,

    INST_RELEASE_POWER_DOWN_ID = 0xAB,
    INST_MANUFACTURER_DEVICE_ID = 0x90,
    INST_JEDEC_ID = 0x9F,
    INST_READ_UNIQUE_ID = 0x4B,

    INST_READ_DATA = 0x03,
    INST_FAST_READ = 0x0B,

    INST_PAGE_PROGRAM = 0x02,

    INST_SECTOR_ERASE_4KB = 0x20,
    INST_BLOCK_ERASE_32KB = 0x52,
    INST_BLOCK_ERASE_64KB = 0xD8,
    INST_CHIP_ERASE = 0xC7, // 0x60

    INST_READ_STATUS_REG_1 = 0x05,
    INST_WRITE_STATUS_REG_1 = 0x01,
    INST_READ_STATUS_REG_2 = 0x35,
    INST_WRITE_STATUS_REG_2 = 0x31,
    INST_READ_STATUS_REG_3 = 0x15,
    INST_WRITE_STATUS_REG_3 = 0x11,

    INST_READ_SFDP_REG = 0x5A,
    INST_ERASE_SECURITY_REG = 0x44,
    INST_PROGRAM_SECURITY_REG = 0x42,
    INST_READ_SECURITY_REG = 0x48,

    INST_GLOBAL_BLOCK_LOCK = 0x7E,
    INST_GLOBAL_BLOCK_UNLOCK = 0x98,
    INST_READ_BLOCK_LOCK = 0x3D,
    INST_INDIVIDUAL_BLOCK_LOCK = 0x36,
    INST_INDIVIDUAL_BLOCK_UNLOCK = 0x39,

    INST_ERASE_PROGRAM_SUSPEND = 0x75,
    INST_ERASE_PROGRAM_RESUME = 0x7A,
    INST_POWER_DOWN = 0xB9,

    INST_ENABLE_RESET = 0x66,
    INST_RESET_DEVICE = 0x99,
} W25Q128_InstructionTypeDef;

void w25q128Init(void);
W25Q128_Status_e w25q128ReadJedecId(uint32_t* id);
W25Q128_Status_e w25q128Read(uint32_t start_page, uint16_t offset, uint8_t* data, uint16_t length);
W25Q128_Status_e w25q128SectorErase(uint16_t num_sector);
W25Q128_Status_e w25q128PageProgram(uint32_t page, uint16_t offset, uint8_t* data, uint16_t length);
W25Q128_Status_e w25q128PageProgramDMA(uint32_t page, uint16_t offset, uint8_t* data, uint32_t length);
void w25q128Process(void);
bool w25q128IsIdle(void);
W25Q128_Status_e w25q128WaitUntilReady(void);
// void testFunc(void);

#endif