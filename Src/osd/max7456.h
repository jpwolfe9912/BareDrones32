/** @file 		max7456.h
 *  @brief
 *  	This file initializes the MAX7456 OSD chip.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		10 OCT 2026
 */

#ifndef __MAX7456_H__
#define __MAX7456_H__

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#define NTSC                0
#define PAL                 1
#define AUTO				2

//MAX7456 Registers
#define VM0_REG             0x00
#define VM1_REG             0x01
#define DMM_REG             0x04
#define DMAH_REG            0x05
#define DMAL_REG            0x06
#define DMDI_REG            0x07
#define CMM_REG             0x08
#define CMAH_REG            0x09
#define RB0_REG             0x10
#define CMAL_REG            0x0A
#define CMDI_REG            0x0B
#define STAT_REG            0xA0

#define READ_MAX7456_REG    0x80

//MAX7456 Commands
#define CLEAR_DISPLAY       0x04
#define CLEAR_DISPLAY_VERT  0x06
#define END_STRING          0xFF
#define WRITE_NVR           0xA0

#define WHITE_LEVEL_80      0x03
#define WHITE_LEVEL_90      0x02
#define WHITE_LEVEL_100     0x01
#define WHITE_LEVEL_120     0x00

#define MAX_FONT_ROM        0xFF
#define STATUS_REG_NVR_BUSY 0x20
#define NVM_RAM_SIZE        0x36

///////////////////////////////////////////////////////////////////////////////
// MAX7456 Variables
///////////////////////////////////////////////////////////////////////////////

extern uint8_t  osdDisabled;

extern uint16_t maxScreenSize;
extern uint16_t maxScreenRows;
extern uint8_t  enableDisplay;
extern uint8_t  enableDisplayVert;
extern uint8_t  max7456Reset;
extern uint8_t  disableDisplay;

void max7456Init();
void resetMax7456(void);
void showMax7456Font(void);
void downloadMax7456Font(void);
void writeMax7456Chars(const char* buf, uint8_t len, uint8_t flags, uint8_t y, uint8_t x);

void showMax7456Font(void);
void writeNVMcharacter(uint8_t ch, const uint16_t index);
void downloadMax7456Font(void);

#endif