/** @file 		config.h
 *  @brief
 *  	This file configures the EEPROM settings of the flight controller.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

///////////////////////////////////////////////////////////////////////////////

#ifndef __CONFIG_H__
#define __CONFIG_H__

/* Defines */
#define FLASH_WRITE_EEPROM_ADDR  0x08040000  // FLASH_Sector_6

/* Enumerations */
enum { eepromConfigNUMWORD =  sizeof(eepromConfig_t)/sizeof(uint32_t) };

/* Global Variables */
extern const char rcChannelLetters[12];

extern semaphore_t eepromChanged;

/* Function Prototypes */
void parseRcChannels(const char *input);
uint32_t crc32bEEPROM(eepromConfig_t *e, int includeCRCAtEnd);
void readEEPROM(void);
HAL_StatusTypeDef writeEEPROM(void);
void checkFirstTime(bool eepromReset);

///////////////////////////////////////////////////////////////////////////////
#endif
