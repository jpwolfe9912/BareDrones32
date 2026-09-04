/** @file 		crsf.h
 *  @brief
 *  	This file contains all the functions for crossfire compatibility.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

#ifndef __CRSF_H__
#define __CRSF_H__

#define CRSF_FRAME_SIZE_MAX  64 // 62 bytes frame plus 2 bytes frame header(<length><type>)
#define CRSF_PAYLOAD_SIZE_MAX  (CRSF_FRAME_SIZE_MAX - 6)

#define CRSF_MAX_CHANNEL        16

typedef struct crsfFrameDef_s {
    uint8_t deviceAddress;
    uint8_t frameLength;
    uint8_t type;
    uint8_t payload[CRSF_FRAME_SIZE_MAX + 1]; // +1 for CRC at end of payload
} crsfFrameDef_t;

typedef union crsfFrame_u {
    uint8_t bytes[CRSF_FRAME_SIZE_MAX];
    crsfFrameDef_t frame;
} crsfFrame_t;

void crsfInit(void);
void crsfProcess(void);

#endif