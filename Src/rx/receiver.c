/** @file 		receiver.c
 *  @brief
 *  	This file contains functions for processing receiver signals
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		03 MAR 2022
 */

#include "receiver.h"

rcData_t rcData = {0};

void updateRcChannels(uint16_t* rawChannels)
{
    for (uint8_t channel = 0; channel < RC_CHANNELS_MAX; channel++)
    {
        rcData.channels[channel] = rawChannels[channel];
    }
}