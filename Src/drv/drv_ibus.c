/*
 * crsf.c
 *
 *  Created on: Jan 14, 2022
 *      Author: jeremywolfe
 */

#include "board.h"

/*		Static Function Prototypes	*/
static void ibus_update(uint8_t* pData);
static bool ibusFrameCRC(void);

/*		Global Variables	*/
uint8_t ibusPayload[PAYLOAD_SIZE];	// payload buffer
uint16_t ibusChannels[RC_CHANNELS];

uint8_t state, cmd, frameLength, framePos;

bool rcActive = false;

void ibusInit(void){
	lwrb_init(&rxRingBuf, rxRingBufData, sizeof(rxRingBufData));

	usart1Read(rxBuf, RXBUF_SIZE);

	rcActive = true;
}

void ibus_process(void){
	while(ibus_process_frame() == IBUS_BUSY);
}

bool ibus_process_frame(void){
	ibusState_e status;
	uint8_t b;
	if(lwrb_read(&rxRingBuf, &b, 1) == 1){
		status = IBUS_BUSY;
		switch(state){
			case IBUS_STATE_LENGTH: {				// length byte
				if(b == 0x20){
					frameLength = b;
					framePos = 0;
					++state;
				}
				else
					status = IBUS_ERROR;
				break;
			}
			case IBUS_STATE_TYPE: {
				cmd = b;
				if(cmd == 0x40){
					++state;
				}
				else
					status = IBUS_ERROR;
				break;
			}
			case IBUS_STATE_PAYLOAD: {				// payload bytes
				ibusPayload[framePos++] = b;
				--frameLength;
				if(framePos == PAYLOAD_SIZE){
					ibus_update(ibusPayload);
					++state;
					framePos = 0;
				}
				break;
			}
			case IBUS_STATE_CRC: {
				if(framePos == 1){
					if(ibusFrameCRC()){
						state = 0;
						status = IBUS_READY;
					}
					else{
						status = IBUS_ERROR;
					}
				}
				else{
					framePos++;
				}
				break;
			}
		}
	}
	if(status == IBUS_ERROR)
		memset(rxBuf, 0x00, RXBUF_SIZE);
	return status;
}

static void ibus_update(uint8_t* pData)
{
	for(int ch_index = 0, bf_index = 0; ch_index < RC_CHANNELS; ch_index++, bf_index += 2)
	{
		ibusChannels[ch_index] = pData[bf_index + 1] << 8 | pData[bf_index];
	}
}

static bool ibusFrameCRC(void){
	uint16_t checksum_cal = 0xFFFF;
	uint16_t checksum_ibus;

	for(int i = 0; i < 30; i++)
	{
		checksum_cal -= rxRingBufData[i];
	}

	checksum_ibus = rxRingBufData[31] << 8 | rxRingBufData[30]; // checksum value from ibus
	return (checksum_ibus == checksum_cal);
}

/**
 * \brief           Check for new data received with DMA
 *
 * User must select context to call this function from:
 * - Only interrupts (DMA HT, DMA TC, UART IDLE) with same preemption priority level
 * - Only thread context (outside interrupts)
 *
 * If called from both context-es, exclusive access protection must be implemented
 * This mode is not advised as it usually means architecture design problems
 *
 * When IDLE interrupt is not present, application must rely only on thread context,
 * by manually calling function as quickly as possible, to make sure
 * data are read from raw buffer and processed.
 *
 * Not doing reads fast enough may cause DMA to overflow unread received bytes,
 * hence application will lost useful data.
 *
 * Solutions to this are:
 * - Improve architecture design to achieve faster reads
 * - Increase raw buffer size and allow DMA to write more data before this function is called
 */
void usart_rx_check(uint32_t bytesLeft){
	static size_t old_pos;
	size_t pos;

	/* Calculate current position in buffer and check for new data available */
	pos = ARRAY_LEN(rxBuf) - bytesLeft;
	if (pos != old_pos) {                       /* Check change in received data */
		if (pos > old_pos) {                    /* Current position is over previous one */
			/*
			 * Processing is done in "linear" mode.
			 *
			 * Application processing is fast with single data block,
			 * length is simply calculated by subtracting pointers
			 *
			 * [   0   ]
			 * [   1   ] <- old_pos |------------------------------------|
			 * [   2   ]            |                                    |
			 * [   3   ]            | Single block (len = pos - old_pos) |
			 * [   4   ]            |                                    |
			 * [   5   ]            |------------------------------------|
			 * [   6   ] <- pos
			 * [   7   ]
			 * [ N - 1 ]
			 */
			usart_process_data(&rxBuf[old_pos], pos - old_pos);
		}
		else {
			/*
			 * Processing is done in "overflow" mode..
			 *
			 * Application must process data twice,
			 * since there are 2 linear memory blocks to handle
			 *
			 * [   0   ]            |---------------------------------|
			 * [   1   ]            | Second block (len = pos)        |
			 * [   2   ]            |---------------------------------|
			 * [   3   ] <- pos
			 * [   4   ] <- old_pos |---------------------------------|
			 * [   5   ]            |                                 |
			 * [   6   ]            | First block (len = N - old_pos) |
			 * [   7   ]            |                                 |
			 * [ N - 1 ]            |---------------------------------|
			 */
			usart_process_data(&rxBuf[old_pos], ARRAY_LEN(rxBuf) - old_pos);
			if (pos > 0) {
				usart_process_data(&rxBuf[0], pos);
			}
		}
		old_pos = pos;                          /* Save current position as old for next transfers */
	}
}


/**
 * \brief           Process received data over UART
 * Data are written to RX ringbuffer for application processing at latter stage
 * \param[in]       data: Data to process
 * \param[in]       len: Length in units of bytes
 */
void usart_process_data(const void* data, size_t len) {
	lwrb_write(&rxRingBuf, data, len);  /* Write data to receive buffer */
}
