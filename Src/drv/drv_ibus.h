/** @file 		drv_ibus.h
 *  @brief
 *  	This file initializes the ibus receiver protocol
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 *  @bug
 */

#ifndef INC_DRV_IBUS_H_
#define INC_DRV_IBUS_H_

/* Includes */
#include "drv_lwrb.h"

/* Defines */
#define IBUS_FRAME_SIZE_MAX			32
#define PAYLOAD_SIZE				(IBUS_FRAME_SIZE_MAX - 4U)
#define RC_CHANNELS					(PAYLOAD_SIZE / 2U)
#define ARRAY_LEN(x)				(sizeof(x) / sizeof((x)[0]))

/* Prototypes */
bool ibusInit(void);
void ibusProcess(void);

/* Global Variables */
extern uint8_t rxBuf[RXBUF_SIZE];

extern lwrb_t rxRingBuf;
extern uint8_t rxRingBufData[RXBUF_SIZE];

extern uint8_t ibusPayload[PAYLOAD_SIZE];
extern uint16_t ibusChannels[RC_CHANNELS];

enum {
	IBUS_STATE_LENGTH,
	IBUS_STATE_TYPE,
	IBUS_STATE_PAYLOAD,
	IBUS_STATE_CRC
};

typedef enum{
	IBUS_READY,
	IBUS_BUSY,
	IBUS_ERROR
}ibusStatus_e;

#endif /* INC_DRV_IBUS_H_ */
