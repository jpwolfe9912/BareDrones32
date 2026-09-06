/** @file 		receiver.h
 *  @brief
 *  	This file contains functions for processing receiver signals
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

#ifndef __RECEIVER_H__
#define __RECEIVER_H__

#include <stdint.h>
#include <stdbool.h>

#define RC_CHANNELS_MAX 16U

#define CRSF_BAUDRATE 416666U
#define IBUS_BAUDRATE 115200U

typedef struct
{
    uint16_t channels[16];
    bool connected;
    bool failsafe;
    uint32_t lastFrameTime;
} rcData_t;

extern rcData_t rcData;

void updateRcChannels(uint16_t* rawChannels);

#endif /* __RECEIVER_H__ */