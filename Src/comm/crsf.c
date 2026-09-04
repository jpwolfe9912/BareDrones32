/** @file 		crsf.h
 *  @brief
 *  	This file contains all the functions for crossfire compatibility.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

#include "board.h"

static crsfFrame_t crsfFrame;
static crsfFrame_t crsfChannelDataFrame;
static uint32_t crsfChannelData[CRSF_MAX_CHANNEL];

void crsfInit(void)
{
    printf("\nCrossfire initializing \n");

    usart1BeginRx();

    lwrb_init(&Buffs.RxBuffer, (void *)Buffs.RxBuffer_Data, sizeof(Buffs.RxBuffer_Data));
}

void crsfProcess(void)
{
    lwrb_read(&Buffs.RxBuffer, crsfChannelData, ARRAY_LEN(crsfChannelData));
    
}