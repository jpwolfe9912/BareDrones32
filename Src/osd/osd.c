/**
 * @file osd.c
 * @author Jeremy Wolfe
 * @brief
 * @date 11 OCT 26
 *
 */

#include "osd.h"
#include "osd_widgets.h"
#include "max7456.h"
#include "config.h"

void
osdInit(void)
{
    max7456Init();
}

void
osdUpdate(void)
{
    if (!eepromConfig.osdEnabled)
        return;

    if (eepromConfig.osdVoltage.enabled)
    {
        osdDrawVoltage(&eepromConfig.osdVoltage);
    }

    if (eepromConfig.osdThrottle.enabled)
    {
        osdDrawThrottle(&eepromConfig.osdThrottle);
    }

    if (eepromConfig.osdTimer.enabled)
    {
        // osdDrawTimer(&eepromConfig.osdTimer);
    }
}