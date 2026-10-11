#ifndef __OSD_WIDGETS_H__
#define __OSD_WIDGETS_H__



void updateMax7456(uint32_t currentOSDTime, uint8_t updateOSD);


// AltitudeHold Display


void displayAltitude(float pressureAltitude, float altitudeReference, uint8_t altHoldState, uint8_t updateOSD);


// Artificial Horizon Display


#define AH_DISPLAY_RECT_HEIGHT  9  // Height of rectangle bounding AI.
                                   // Should be odd so that there is an equal space
                                   // above/below the center reticle

extern uint8_t reticleRow;
extern uint8_t ahTopPixel;
extern uint8_t ahBottomPixel;
extern uint8_t ahCenter;

void displayArtificialHorizon(float roll, float pitch, uint8_t flightMode, uint8_t updateOSD);


// Attitude Display


#define AI_DISPLAY_RECT_HEIGHT  9  // Height of rectangle bounding AI.
                                   // Should be odd so that there is an equal space
                                   // above/below the center reticle

extern uint8_t aiTopPixel;
extern uint8_t aiBottomPixel;
extern uint8_t aiCenter;

void displayAttitude(float roll, float pitch, uint8_t flightMode, uint8_t updateOSD);


// Heading Display


void displayHeading(float currentHeading, uint8_t updateOSD);


// Battery Display


void displayBattery(uint8_t updateOSD);


// RSSI Display


void displayRSSI(uint8_t updateOSD);


// Throttle Display


void displayThrottle(uint8_t updateOSD);


// Motors Armed Timer Display


void displayMotorArmedTime(uint32_t currentOSDTime, uint8_t updateOSD);


#endif