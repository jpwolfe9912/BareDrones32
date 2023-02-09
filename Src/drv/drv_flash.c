/** @file 		drv_flash.c
 *  @brief
 *  	This file initializes the flash.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

/* Includes */
#include "board.h"

/* Static Prototypes */
static void flashMassErase(uint8_t VoltageRange);
void flashEraseSector(uint32_t Sector, uint8_t VoltageRange);
static bool flashWaitForLastOperation(void);
static void flashProgramWord(uint32_t Address, uint32_t Data);

bool flashErase(FLASH_EraseInit_t *pEraseInit)
{
    uint32_t index = 0;

    if (!flashWaitForLastOperation())
        return false;
    if (pEraseInit->TypeErase == FLASH_TYPEERASE_MASSERASE)
    {
        flashMassErase((uint8_t)pEraseInit->VoltageRange);

        if (!flashWaitForLastOperation())
            return false;
    }
    else
    {
        /* Erase by sector by sector to be done*/
        for (index = pEraseInit->Sector; index < (pEraseInit->NbSectors + pEraseInit->Sector); index++)
        {
            flashEraseSector(index, (uint8_t)pEraseInit->VoltageRange);

            /* Wait for last operation to be completed */
            if (!flashWaitForLastOperation())
                return false;

            /* If the erase operation is completed, disable the SER Bit and SNB Bits */
            FLASH->CR &= ~(FLASH_CR_SER | FLASH_CR_SNB);
        }
    }
    return true;
}

/**
 * @brief  Program byte, halfword, word or double word at a specified address
 * @param  Address  specifies the address to be programmed.
 * @param  Data specifies the data to be programmed
 *
 * @retval Bool Whether the flash succeeded or not 
 */
bool flashProgram(uint32_t Address, uint64_t Data)
{
    /* Wait for last operation to be completed */
    if (!flashWaitForLastOperation())
        return false;

    /*Program word (32-bit) at a specified address.*/
    flashProgramWord(Address, (uint32_t)Data);

    /* Wait for last operation to be completed */
    if (!flashWaitForLastOperation())
        return false;

    /* If the program operation is completed, disable the PG Bit */
    FLASH->CR &= ~FLASH_CR_PG;

    return true;
}

/** @brief Changes clock speed to 216 MHz
 *
 * @retval Bool Whether the flash succeeded or not 
 */
bool flashUnlock(void)
{
    if (FLASH->CR & FLASH_CR_LOCK)
    {
        /* Authorize the FLASH Registers access */
        FLASH->KEYR |= FLASH_KEY1;
        FLASH->KEYR |= FLASH_KEY2;

        /* verify Flash is unlock */
        if (FLASH->CR & FLASH_CR_LOCK)
            return false;
    }
    return true;
}

/**
 * @brief  Locks the FLASH control register access
 */
void flashLock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}
/**
 * @brief  Full erase of FLASH memory sectors
 * @param  VoltageRange The device voltage range which defines the erase parallelism.
 *
 * @retval None
 */
static void
flashMassErase(uint8_t VoltageRange)
{
    /* if the previous operation is completed, proceed to erase all sectors */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= FLASH_CR_MER;
    FLASH->CR |= FLASH_CR_STRT | ((uint32_t)VoltageRange << 8);
    /* Data synchronous Barrier (DSB) Just after the write operation
       This will force the CPU to respect the sequence of instruction (no optimization).*/
    __DSB();
}

/**
 * @brief  Erase the specified FLASH memory sector
 * @param  Sector FLASH sector to erase
 *         The value of this parameter depend on device used within the same series
 * @param  VoltageRange The device voltage range which defines the erase parallelism.
 *
 * @retval None
 */
void flashEraseSector(uint32_t Sector, uint8_t VoltageRange)
{
    uint32_t tmp_psize = 0;

    if (VoltageRange == FLASH_VOLTAGE_RANGE_1)
        tmp_psize = FLASH_PSIZE_BYTE;
    else if (VoltageRange == FLASH_VOLTAGE_RANGE_2)
        tmp_psize = FLASH_PSIZE_HALF_WORD;
    else if (VoltageRange == FLASH_VOLTAGE_RANGE_3)
        tmp_psize = FLASH_PSIZE_WORD;
    else
        tmp_psize = FLASH_PSIZE_DOUBLE_WORD;

    /* If the previous operation is completed, proceed to erase the sector */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= tmp_psize;
    FLASH->CR &= SECTOR_MASK;
    FLASH->CR |= FLASH_CR_SER | (Sector << FLASH_CR_SNB_Pos);
    FLASH->CR |= FLASH_CR_STRT;

    /* Data synchronous Barrier (DSB) Just after the write operation
       This will force the CPU to respect the sequence of instruction (no optimization).*/
    __DSB();
}

static bool
flashWaitForLastOperation(void)
{
    /* Wait for last operation to be completed */
    while (FLASH->SR & FLASH_SR_BSY)
        ;                                                                                                     // wait for not busy
    if (FLASH->SR & (FLASH_SR_OPERR | FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_ERSERR)) // check for error flags cleared
        return false;
    if (FLASH->SR & FLASH_SR_EOP) // check for not EOP
        FLASH->SR |= FLASH_SR_EOP;
    return true;
}

/**
 * @brief  Program word (32-bit) at a specified address.
 * @note   This function must be used when the device voltage range is from
 *         2.7V to 3.3V.
 *
 * @param  Address specifies the address to be programmed.
 * @param  Data specifies the data to be programmed.
 * @retval None
 */
static void
flashProgramWord(uint32_t Address, uint32_t Data)
{
    /* If the previous operation is completed, proceed to program the new data */
    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= FLASH_PSIZE_WORD;
    FLASH->CR |= FLASH_CR_PG;

    *(__IO uint32_t *)Address = Data;

    /* Data synchronous Barrier (DSB) Just after the write operation
       This will force the CPU to respect the sequence of instruction (no optimization).*/
    __DSB();
}