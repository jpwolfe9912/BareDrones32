/** @file 		drv_flash.h
 *  @brief
 *  	This file initializes the flash.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

#ifndef __FLASH_H__
#define __FLASH_H__

/* Defines */
#define FLASH_KEY1 ((uint32_t)0x45670123U)
#define FLASH_KEY2 ((uint32_t)0xCDEF89ABU)

#define FLASH_TYPEERASE_SECTORS ((uint32_t)0x00U)   /*!< Sectors erase only          */
#define FLASH_TYPEERASE_MASSERASE ((uint32_t)0x01U) /*!< Flash Mass erase activation */

#define FLASH_PSIZE_BYTE ((uint32_t)0x00000000U)
#define FLASH_PSIZE_HALF_WORD ((uint32_t)FLASH_CR_PSIZE_0)
#define FLASH_PSIZE_WORD ((uint32_t)FLASH_CR_PSIZE_1)
#define FLASH_PSIZE_DOUBLE_WORD ((uint32_t)FLASH_CR_PSIZE)
#define CR_PSIZE_MASK ((uint32_t)0xFFFFFCFFU)

#define FLASH_VOLTAGE_RANGE_1 ((uint32_t)0x00U) /*!< Device operating range: 1.8V to 2.1V                */
#define FLASH_VOLTAGE_RANGE_2 ((uint32_t)0x01U) /*!< Device operating range: 2.1V to 2.7V                */
#define FLASH_VOLTAGE_RANGE_3 ((uint32_t)0x02U) /*!< Device operating range: 2.7V to 3.6V                */
#define FLASH_VOLTAGE_RANGE_4 ((uint32_t)0x03U) /*!< Device operating range: 2.7V to 3.6V + External Vpp */

#define SECTOR_MASK ((uint32_t)0xFFFFFF07U)
#define FLASH_SECTOR_0 ((uint32_t)0U)                  /*!< Sector Number 0   */
#define FLASH_SECTOR_1 ((uint32_t)1U)                  /*!< Sector Number 1   */
#define FLASH_SECTOR_2 ((uint32_t)2U)                  /*!< Sector Number 2   */
#define FLASH_SECTOR_3 ((uint32_t)3U)                  /*!< Sector Number 3   */
#define FLASH_SECTOR_4 ((uint32_t)4U)                  /*!< Sector Number 4   */
#define FLASH_SECTOR_5 ((uint32_t)5U)                  /*!< Sector Number 5   */
#define FLASH_SECTOR_6 ((uint32_t)6U)                  /*!< Sector Number 6   */
#define FLASH_SECTOR_7 ((uint32_t)7U)                  /*!< Sector Number 7   */

#define FLASH_TYPEPROGRAM_BYTE ((uint32_t)0x00U)       /*!< Program byte (8-bit) at a specified address           */
#define FLASH_TYPEPROGRAM_HALFWORD ((uint32_t)0x01U)   /*!< Program a half-word (16-bit) at a specified address   */
#define FLASH_TYPEPROGRAM_WORD ((uint32_t)0x02U)       /*!< Program a word (32-bit) at a specified address        */
#define FLASH_TYPEPROGRAM_DOUBLEWORD ((uint32_t)0x03U) /*!< Program a double word (64-bit) at a specified address */

/* Typedefs */
typedef struct
{
    uint32_t TypeErase;    /*!< Mass erase or sector Erase.
                                This parameter can be a value of @ref FLASHEx_Type_Erase */
    uint32_t Sector;       /*!< Initial FLASH sector to erase when Mass erase is disabled
                                This parameter must be a value of @ref FLASHEx_Sectors */
    uint32_t NbSectors;    /*!< Number of sectors to be erased.
                                This parameter must be a value between 1 and (max number of sectors - value of Initial sector)*/
    uint32_t VoltageRange; /*!< The device voltage range which defines the erase parallelism
                                This parameter must be a value of @ref FLASHEx_Voltage_Range */

} FLASH_EraseInit_t;

/* Function Prototypes */
bool flashErase(FLASH_EraseInitTypeDef *pEraseInit);
bool flashProgram(uint32_t Address, uint64_t Data);
bool flashUnlock(void);
void flashLock(void);

#endif /* INC_FLASH_H_ */
