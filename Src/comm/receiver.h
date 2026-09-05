/** @file 		receiver.h
 *  @brief
 *  	This file contains functions for processing receiver signals
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

#pragma once
#include "board.h"

#define RC_CHANNELS_MAX 16U

typedef struct
{
    uint16_t channels[16];
    bool connected;
    bool failsafe;
    uint32_t lastFrameTime;
} rcData_t;

extern rcData_t rcData;

void updateRcChannels(uint16_t* rawChannels);