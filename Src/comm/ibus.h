/** @file 		ibus.h
 *  @brief
 *  	This file initializes the ibus receiver protocol
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

#ifndef __IBUS_H__
#define __IBUS_H__

/* Defines */
#define IBUS_FRAME_SIZE_MAX			32
#define PAYLOAD_SIZE				(IBUS_FRAME_SIZE_MAX - 4U)
#define CRC_SIZE					2
#define RC_CHANNELS					(PAYLOAD_SIZE / 2U)
#define ARRAY_LEN(x)				(sizeof(x) / sizeof((x)[0]))

/* Prototypes */
bool ibusInit(void);
void ibusProcess(void);

/* Global Variables */
extern uint16_t ibusChannels[RC_CHANNELS];

extern bool rcActive;

/* Global Enums */
typedef enum {
	IBUS_STATE_LENGTH,
	IBUS_STATE_TYPE,
	IBUS_STATE_PAYLOAD,
	IBUS_STATE_CRC
}ibusStates_e;

typedef enum{
	IBUS_READY,
	IBUS_BUSY,
	IBUS_ERROR
}ibusStatus_e;

#endif /* __IBUS_H__ */
