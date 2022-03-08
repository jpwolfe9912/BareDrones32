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
#define FLASH_WRITE_EEPROM_ADDR  0x08004000  // FLASH_Sector_1

/* Global Variables */
extern const char rcChannelLetters[12];

/* Function Prototypes */
void parseRcChannels(const char *input);
uint32_t crc32bEEPROM(eepromConfig_t *e, int includeCRCAtEnd);
void readEEPROM(void);
int writeEEPROM(void);
void checkFirstTime(bool eepromReset);

///////////////////////////////////////////////////////////////////////////////
#endif
