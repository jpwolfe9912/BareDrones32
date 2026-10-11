/** @file 		max7456.c
 *  @brief
 *  	This file initializes the MAX7456 OSD chip.
 *
 *
 *  @author 	Jeremy Wolfe
 *  @date 		10 OCT 2026
 */


#include "max7456.h"

#include "max7456font.h"
#include "osd_widgets.h"
#include "drv_spi3.h"
#include "drv_system.h"
#include "config.h"

uint8_t charbuf[54]; // for NVM_read

uint8_t osdDisabled = false;

uint16_t maxScreenSize = 0;
uint16_t maxScreenRows = 0;
uint8_t  enableDisplay = 0;
uint8_t  enableDisplayVert = 0;
uint8_t  max7456Reset = 0;
uint8_t  disableDisplay = 0;

/* Static Functions */
static void detect_video_standard(void);
void hide_OSD(void);
void unhide_OSD(void);
static void max7456_set_white_level(void);
static bool wait_NVM(void);
static bool max7456_verify_nvm_character(uint8_t ch);

/**
 * @brief Initializes the MAX7456 OSD chip
 *
 */
void
max7456Init(void)
{
    uint8_t i;
    eepromConfig.osdEnabled = true;

    if (eepromConfig.osdEnabled == false)
        return;

    spi3WriteOneByte(DMDI_REG, END_STRING); // finish any auto increment sends

    detect_video_standard();

    resetMax7456();
}

/**
 * @brief Detects the video standard. PAL or NTSC
 *
 */
static void
detect_video_standard(void)
{
    uint8_t standard = AUTO; //eepromConfig.defaultVideoStandard;

    // if autodetect enabled modify the default if signal is present on either standard
    // otherwise default is preserved

    if (standard == AUTO)
    {
        uint8_t stat = 0;

        spi3ReadOneByte(STAT_REG, &stat);

        if (stat & 0x01)
            standard = PAL;

        else if (stat & 0x02)
            standard = NTSC;

        else
            standard = PAL;
    }

    if (standard == PAL)
    {
        maxScreenSize = 480;
        maxScreenRows = 16;
        enableDisplay = 0x48;
        enableDisplayVert = 0x4C;
        max7456Reset = 0x42;
        disableDisplay = 0x40;
    }
    else
    {
        maxScreenSize = 390;
        maxScreenRows = 13;
        enableDisplay = 0x08;
        enableDisplayVert = 0x0C;
        max7456Reset = 0x02;
        disableDisplay = 0x00;
    }

    // Artificial Horizon Display Parameters

    // reticleRow = maxScreenRows / 2;
    // ahTopPixel = (reticleRow - AH_DISPLAY_RECT_HEIGHT / 2) * 18;
    // ahBottomPixel = (reticleRow + AH_DISPLAY_RECT_HEIGHT / 2) * 18;
    // ahCenter = reticleRow * 18 + 10;

    // // Attitude Display Parameters

    // aiTopPixel = (reticleRow - AI_DISPLAY_RECT_HEIGHT / 2) * 18;
    // aiBottomPixel = (reticleRow + AI_DISPLAY_RECT_HEIGHT / 2) * 18;
    // aiCenter = reticleRow * 18 + 10;
}

/**
 * @brief Hides the OSD
 *
 */
void
hide_OSD(void)
{
    if (!osdDisabled)
    {
        spi3WriteOneByte(VM0_REG, disableDisplay);

        osdDisabled = true;
    }
}


/**
 * @brief Unhides the OSD
 *
 */
void
unhide_OSD(void)
{
    if (osdDisabled)
    {
        spi3WriteOneByte(VM0_REG, enableDisplay);

        osdDisabled = false;
    }
}

static void
max7456_set_white_level(void)
{
    uint8_t tx[32];

    for (uint8_t i = 0; i < maxScreenRows; i++)
    {
        tx[2 * i] = RB0_REG + i;
        tx[2 * i + 1] = WHITE_LEVEL_90;
    }

    spi3WriteBytes(tx, 2 * maxScreenRows);
}


/**
 * @brief Writes characters to the MAX7456 display
 *
 * @param buf Pointer to the buffer containing characters to write
 * @param len Number of characters to write
 * @param flags Flags for character attributes (blink, invert)
 * @param y Row position
 * @param x Column position
 */
void writeMax7456Chars(const char* buf, uint8_t len, uint8_t flags, uint8_t y, uint8_t x)
{
    if (len == 0 || x >= 30 || y >= maxScreenRows)
        return;

    uint16_t offset = y * 30 + x;

    if ((uint32_t)offset + len > maxScreenSize)
        len = (uint8_t)(maxScreenSize - offset);

    uint8_t text_len = 0;
    if (buf != NULL)
    {
        while (text_len < len && buf[text_len] != '\0')
            text_len++;
    }

    //* Single-character write: no auto-increment.
    if (len == 1)
    {
        uint8_t tx[] = {
            DMM_REG,
            ((flags & 1U) ? 0x10U : 0U) |
            ((flags & 2U) ? 0x08U : 0U),

            DMAH_REG, (uint8_t)(offset >> 8),
            DMAL_REG, (uint8_t)offset,

            DMDI_REG, (text_len > 0) ? (uint8_t)buf[0] : 0
        };

        spi3WriteBytes(tx, sizeof(tx));
        return;
    }


    //* Multi-character write using auto-increment.
    uint8_t tx[520];
    uint16_t index = 0;

    uint8_t dmm = ((flags & 1U) ? 0x10U : 0U) |
        ((flags & 2U) ? 0x08U : 0U);

    //* Enable auto-increment.
    tx[index++] = DMM_REG;
    tx[index++] = dmm | 0x01U;

    //* Set starting address.
    tx[index++] = DMAH_REG;
    tx[index++] = (uint8_t)(offset >> 8);
    tx[index++] = DMAL_REG;
    tx[index++] = (uint8_t)offset;

    //* Write characters.
    for (uint16_t i = 0; i < len; i++)
    {
        tx[index++] = DMDI_REG;
        tx[index++] = (i < text_len) ? (uint8_t)(buf[i] - 1) : 0;
    }

    //* Terminate auto-increment.
    tx[index++] = DMDI_REG;
    tx[index++] = END_STRING;

    //* Restore normal display memory mode.
    tx[index++] = DMM_REG;
    tx[index++] = dmm;

    spi3WriteBytes(tx, index);
}

/**
 * @brief Resets the MAX7456 display
 *
 */
void
resetMax7456(void)
{
    //Soft reset the MAX7456 - clear display memory
    spi3WriteOneByte(VM0_REG, max7456Reset);
    delay(10);

    //Set white level to 90% for all rows
    max7456_set_white_level();

    //ensure device is enabled
    spi3WriteOneByte(VM0_REG, enableDisplay);

    spi3WriteOneByte(DMM_REG, CLEAR_DISPLAY);
    delay(10);

    osdDisabled = false;
}


/**
 * @brief Shows the MAX7456 font
 *
 */
void
showMax7456Font(void)
{
    // Clear display
    spi3WriteOneByte(DMM_REG, CLEAR_DISPLAY);
    delay(10);

    // Normal 16-bit display memory mode
    spi3WriteOneByte(DMM_REG, 0x00);

    uint16_t address = 33;

    for (uint16_t ch = 0; ch < 255; ch++)
    {
        spi3WriteOneByte(DMAH_REG, address >> 8);
        spi3WriteOneByte(DMAL_REG, address & 0xFF);
        spi3WriteOneByte(DMDI_REG, (uint8_t)ch);

        address++;

        // Insert 6 blank positions after every 24 characters
        if ((ch % 24) == 23)
            address += 6;
    }
}

//* NVM Functions
/**
 * @brief Writes a character to the MAX7456 NVM
 *
 * @param ch Character to write
 * @param index Index in the font data array
 */
bool
writeNVMcharacter(uint8_t ch)
{
    uint8_t tx[54 * 4];
    uint16_t index = 0;

    // Disable OSD during NVM operations
    spi3WriteOneByte(VM0_REG, disableDisplay);
    delay(1);

    // Select character
    spi3WriteOneByte(CMAH_REG, ch);  // set start address high

    for (uint8_t x = 0; x < NVM_RAM_SIZE; x++)
    {
        tx[index++] = CMAL_REG;
        tx[index++] = x;

        tx[index++] = CMDI_REG;
        tx[index++] = fontdata[ch][x];
    }
    // Transfer 216 bytes
    spi3WriteBytes(tx, index);

    // Commit shadow RAM to NVM
    spi3WriteOneByte(CMM_REG, WRITE_NVR);
    // Wait for NVM operation to begin
    bool success = wait_NVM();

    spi3WriteOneByte(VM0_REG, enableDisplay);

    return success;
}

/**
 * @brief Downloads font data to the MAX7456 NVM
 *
 */
void
downloadMax7456Font(void)
{
    printf("Programming MAX7456 font...\n");

    for (uint16_t ch = 0; ch < 256; ch++)
    {
        if (!writeNVMcharacter((uint8_t)ch))
        {
            printf("NVM write failed at character %u\n", ch);
            return;
        }

        if (!max7456_verify_nvm_character((uint8_t)ch))
        {
            printf("NVM verification failed at character %u\n", ch);
            return;
        }

        printf("Character %u/256 OK\n", ch + 1);
    }

    resetMax7456();

    printf("Font programming complete\n");
}

/**
 * @brief Waits for the NVM to be ready
 *
 */
static bool
wait_NVM(void)
{
    uint8_t stat = 0;
    for (uint16_t i = 0; i < 100; i++)
    {
        spi3ReadOneByte(STAT_REG, &stat);
        if (!(stat & STATUS_REG_NVR_BUSY))
            return true;

        delay(1);
    }
    return false;
}


static bool
max7456_verify_nvm_character(uint8_t ch)
{
    uint8_t data;

    // OSD must be disabled during character NVM reads.
    spi3WriteOneByte(VM0_REG, disableDisplay);
    delay(1);

    spi3WriteOneByte(CMAH_REG, ch);

    // Read NVM character into shadow RAM
    spi3WriteOneByte(CMM_REG, 0x50);
    delay(1);

    if (!wait_NVM())
    {
        spi3WriteOneByte(VM0_REG, enableDisplay);
        return false;
    }

    // Read back all 54 bytes
    for (uint8_t i = 0; i < NVM_RAM_SIZE; i++)
    {
        spi3WriteOneByte(CMAL_REG, i);

        // CMDO read address = 0xC0;
        // SPI driver adds read bit 0x80.
        spi3ReadOneByte(0x40, &data);

        if (data != fontdata[ch][i])
        {
            printf("Font verify failed: ch=%u byte=%u "
                   "expected=%02X actual=%02X\n",
                   ch, i, fontdata[ch][i], data);

            spi3WriteOneByte(VM0_REG, enableDisplay);
            return false;
        }
    }

    spi3WriteOneByte(VM0_REG, enableDisplay);
    return true;
}
