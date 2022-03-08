/** @file 		drv_ibus.c
 *  @brief
 *  	This file initializes the ibus receiver protocol
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 *  @bug
 */

#include "board.h"

/* Static Function Prototypes */
static ibusStatus_e ibus_process_frame(void);
static void ibus_update(uint8_t* pData);
static bool ibusFrameCRC(void);
static void ibus_error_timeout(void);
static void usart_rx_check(uint32_t bytesLeft);
static void usart_process_data(const void *data, size_t len);

/* Global Variables */
uint8_t ibusPayload[PAYLOAD_SIZE];	// payload buffer
uint16_t ibusChannels[RC_CHANNELS];

uint8_t state, cmd, frameLength, framePos;

uint8_t dev_id = 0;

bool rcActive = false;

ibusStatus_e status;

uint8_t errorTimeOut;


/** @brief Initializes ibus ring buffer and low level usart registers.
 *
 *	@return bool True if initialization was successful.
 *				 False if failed.
 */
bool
ibusInit(void)
{
	uint8_t ibus_initialized = 0;

	printf("\nInitializing iBus Receiver\n");

	lwrb_init(&rxRingBuf, rxRingBufData, sizeof(rxRingBufData));

	usart1Read(rxBuf, RXBUF_SIZE);

	rcActive = true;

	status = IBUS_ERROR;

	for(uint8_t i = 0; i < 10; i++){
		ibusProcess();
		if(dev_id == 0x40){
			ibus_initialized = 1;
			break;
		}
		delay(10);
	}

	if(!ibus_initialized){
		color(RED, YES);
		printf("\niBus Initialization Failed. Try again?\n");
		color(WHITE, NO);
#ifdef STLINK
		if(serialWaitFor('y'))
			return false;
		else
			return true;
#endif
	}
	else{
		color(GREEN, YES);
		printf("\niBus receiver recognized\n");
		color(WHITE, NO);
		return true;
	}
	return true;
}

/** @brief Waits for the parser to not be busy
 *
 *  @return Void.
 */
void
ibusProcess(void)
{
	while(ibus_process_frame() == IBUS_BUSY);
}

/** @brief State machine that evaluates the raw data based on what section it is parsing through.
 *
 *  @return ibusStatus_e The status of the function.
 *  	READY, BUSY, ERROR
 */
static ibusStatus_e
ibus_process_frame(void)
{
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
					dev_id = cmd;
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
	if(status == IBUS_ERROR){
		ibus_error_timeout();
		lwrb_reset(&rxRingBuf);
		memset(rxBuf, 0x00, RXBUF_SIZE);
	}
	errorTimeOut = 0;
	return status;
}

/** @brief Packs the raw data stored in two bytes in an array.
 * 	into one 16 bit integer.
 *
 *  @param *pData A pointer to the raw data array that we want.
 *  to pack.
 *  @return Void.
 */
static void
ibus_update(uint8_t* pData)
{
	for(int ch_index = 0, bf_index = 0; ch_index < RC_CHANNELS; ch_index++, bf_index += 2)
	{
		ibusChannels[ch_index] = pData[bf_index + 1] << 8 | pData[bf_index];
	}
}

/** @brief Counts up if an error is recorded and disarms the quad.
 *
 *  @return Void.
 */
static void
ibus_error_timeout(void)
{
	errorTimeOut++;

	if(errorTimeOut > 100){
		armed = false;
	}
}

/** @brief Checksum calculator for incoming data.
 *
 * 		This function calculates the checksum based on the incoming
 * 		data. It then compares that value with the checksum value
 * 		in the packet.
 *
 *  @return bool True if it matches.
 *  			 False if it does not match.
 */
static bool
ibusFrameCRC(void)
{
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
 * 	@brief	Check for new data received with DMA
 *
 * 		User must select context to call this function from:
 * 		- Only interrupts (DMA HT, DMA TC, UART IDLE) with same preemption priority level
 * 		- Only thread context (outside interrupts)
 *
 * 		If called from both context-es, exclusive access protection must be implemented
 * 		This mode is not advised as it usually means architecture design problems
 *
 * 		When IDLE interrupt is not present, application must rely only on thread context,
 * 		by manually calling function as quickly as possible, to make sure
 * 		data are read from raw buffer and processed.
 *
 * 		Not doing reads fast enough may cause DMA to overflow unread received bytes,
 *		hence application will lost useful data.
 *
 * 		Solutions to this are:
 * 		- Improve architecture design to achieve faster reads
 * 		- Increase raw buffer size and allow DMA to write more data before this function is called
 *
 * 	@param bytesLeft The value in the DMA_NDTR register
 */
static void
usart_rx_check(uint32_t bytesLeft)
{
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

/** @brief	Process received data over UART
 *
 * 		Data are written to RX ringbuffer for application processing at later stage
 *
 * 	@param *data Pointer to the data to process
 * 	@param len Length in units of bytes
 */
static void
usart_process_data(const void* data, size_t len)
{
	lwrb_write(&rxRingBuf, data, len);  /* Write data to receive buffer */
}

/* Interrupt Handlers */

/** @brief	USART1 global interrupt handler
 *
 * 	@return Void.
 */
void
USART1_IRQHandler(void) {
	/* Check for IDLE line interrupt */
	if (USART1->ISR & USART_ISR_IDLE) {
		USART1->ICR		|= USART_ICR_IDLECF;	/* Clear IDLE line flag */
		usart_rx_check(DMA2_Stream2->NDTR);		/* Check for data to process */
	}
}

/** @brief	DMA2_Stream2 global interrupt handler
 *
 * 	@return Void.
 */
void
DMA2_Stream2_IRQHandler(void) {
	/* Check half-transfer complete interrupt */
	if(DMA2->LISR & DMA_LISR_TCIF2){
		DMA2->LIFCR		|= DMA_LIFCR_CTCIF2;	/* Clear half-transfer complete flag */
		usart_rx_check(DMA2_Stream2->NDTR);		/* Check for data to process */
	}

	/* Check transfer-complete interrupt */
	if(DMA2->LISR & DMA_LISR_HTIF2){
		DMA2->LIFCR		|= DMA_LIFCR_CHTIF2;	/* Clear half-transfer complete flag */
		usart_rx_check(DMA2_Stream2->NDTR);		/* Check for data to process */
	}
}
