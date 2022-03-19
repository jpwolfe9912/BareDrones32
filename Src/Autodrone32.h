/** @file 		Autodrone32.h
 *  @brief
 *  	This file contains many of the global variables and settings for the drone.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

#ifndef __AUTODRONE32_H__
#define __AUTODRONE32_H__

#ifndef PI
    #define PI  3.14159265358979f
#endif

#define TWO_PI (2.0f * PI)

#define D2R  (PI / 180.0f)

#define R2D  (180.0f / PI)

#define KNOTS2MPS 0.51444444f

#define EARTH_RADIUS 6371000f

#define SQR(x)  ((x) * (x))

///////////////////////////////////////////////////////////////////////////////

#define ROLL     0
#define PITCH    1
#define YAW      2
#define THROTTLE 3
#define AUX1     4
#define AUX2     5
#define AUX3     6
#define AUX4     7
#define AUX5     8
#define AUX6     9
#define AUX7     10
#define AUX8     11

#define XAXIS    0
#define YAXIS    1
#define ZAXIS    2

#define MINCOMMAND  2000
#define MIDCOMMAND  3000
#define MAXCOMMAND  4000

///////////////////////////////////////////////////////////////////////////////
// Misc Type Definitions
///////////////////////////////////////////////////////////////////////////////

typedef union {
    int16_t value;
    uint8_t bytes[2];
} int16andUint8_t;

typedef union {
    int32_t value;
    uint8_t bytes[4];
} int32andUint8_t;

typedef union {
    uint16_t value;
     uint8_t bytes[2];
} uint16andUint8_t;

typedef union {
	uint32_t value;
	 uint8_t bytes[4];
} uint32andUint8_t;

///////////////////////////////////////

typedef volatile uint8_t semaphore_t;

///////////////////////////////////////////////////////////////////////////////
// Sensor Variables
///////////////////////////////////////////////////////////////////////////////

typedef struct sensors_t
{
    float    accel500Hz[3];
    float    accel100Hz[3];
    float    attitude500Hz[3];
    float    gyro500Hz[3];
} sensors_t;

extern sensors_t sensors;

typedef struct heading_t
{
	float    mag;
	float    tru;
} heading_t;

extern heading_t heading;

typedef struct gps_t
{
	int32_t  latitude;     // 1e-7 degrees
	int32_t  longitude;    // 1e-7 degrees
	int32_t  height;       // mm above ellipsoid
	int32_t  hMSL;         // mm above mean sea level
	int32_t  velN;         // cm/s
	int32_t  velE;         // cm/s
	int32_t  velD;         // cm/s
	uint32_t speed;        // cm/s
	uint32_t gSpeed;       // cm/s
	int32_t  heading;      // deg 1e-5
    uint8_t  numSats;
    uint8_t  fix;
    uint8_t  statusFlags;
    uint32_t iTOW;         // mSec
    uint16_t year;         // years
    uint8_t  month;        // months
    uint8_t  day;          // days
    uint16_t hDop;
    uint16_t vDop;
    uint8_t  numCh;
    uint8_t  chn[50];      // channel number
    uint8_t  svid[50];     // satellite ID
    uint8_t  cno[50];      // carrier to noise ratio (signal strength)
    uint8_t  updated;
} gps_t;

extern gps_t gps;

typedef struct homeData_t
{
	int32_t latitude;
	int32_t longitude;
	float   altitude;
	float   magHeading;
} homeData_t;

extern homeData_t homeData;

///////////////////////////////////////////////////////////////////////////////
// PID Definitions
///////////////////////////////////////////////////////////////////////////////

#define NUMBER_OF_PIDS		6

#define ROLL_RATE_PID		0
#define PITCH_RATE_PID		1
#define YAW_RATE_PID		2

#define ROLL_ATT_PID		3
#define PITCH_ATT_PID		4
#define HEADING_PID			5

///////////////////////////////////////////////////////////////////////////////
// Flight Modes
///////////////////////////////////////////////////////////////////////////////

enum {
	TRANS_FLIGHT,
	FLIGHT,
	TRANS_ROVER,
	ROVER
};

///////////////////////////////////////////////////////////////////////////////
// MPU6000 DLPF Configurations
///////////////////////////////////////////////////////////////////////////////

enum { DLPF_256HZ, DLPF_188HZ, DLPF_98HZ, DLPF_42HZ };

///////////////////////////////////////////////////////////////////////////////
// EEPROM
///////////////////////////////////////////////////////////////////////////////

typedef struct eepromConfig_t
{
    ///////////////////////////////////

    uint8_t version;

    float accelBiasMPU[3];          // Bias for MPU60x0 Accel
    float accelScaleFactorMPU[3];   // Scale factor for MPU60x0 Accel

    float accelBiasMXR[3];          // Bias for MXR9150 Accel
    float accelScaleFactorMXR[3];   // Scale factor for MXR9150 Accel

    float accelTCBiasSlope[3];
    float accelTCBiasIntercept[3];

    float gyroTCBiasSlope[3];
    float gyroTCBiasIntercept[3];

    float magBias[6];

    float accelCutoff;

    float KpAcc;

    float KiAcc;

    float KpMag;

    float KiMag;

    float compFilterA;

    float compFilterB;

    uint8_t dlpfSetting;

    uint8_t sensorOrientation;

    ///////////////////////////////////

    float rollAndPitchRateScaling;
    float yawRateScaling;

    float attitudeScaling;

    float nDotEdotScaling;

    float hDotScaling;

    ///////////////////////////////////

    uint8_t receiverType;
    uint8_t ppmChannels;
    uint8_t slaveSpektrum;

    uint8_t rcMap[12];

    uint16_t escPwmRate;
    uint16_t servoPwmRate;

    float midCommand;
    float minCheck;
    float maxCheck;
    float minThrottle;
    float maxThrottle;

    ///////////////////////////////////

    uint8_t mixerConfiguration;
    float yawDirection;

    uint16_t triYawServoPwmRate;
    float    triYawServoMin;
    float    triYawServoMid;
    float    triYawServoMax;
    float    triCopterYawCmd500HzLowPassTau;

    uint8_t  freeMixMotors;

    float    freeMix[8][4];

    ///////////////////////////////////

    float    rollAttAltCompensationGain;
    float    rollAttAltCompensationLimit;

    float    pitchAttAltCompensationGain;
    float    pitchAttAltCompensationLimit;

    ///////////////////////////////////

	PIDdata_t PID[NUMBER_OF_PIDS];

    ///////////////////////////////////

    uint8_t osdEnabled;              // 0 = Disabled, 1 = Enabled
    uint8_t defaultVideoStandard;    // 0 = NTSC, 1 = PAL
    uint8_t metricUnits;             // 1 = metric

    uint8_t osdDisplayAlt;           // 1 = Display OSD Altitude
    uint8_t osdDisplayAltRow;
    uint8_t osdDisplayAltCol;
    uint8_t osdDisplayAltHoldState;  // 1 = display altitude hold state, 0 = don't display

    uint8_t osdDisplayAH;            // 1 = Display OSD Artificial Horizon
    uint8_t osdDisplayAtt;           // 1 = Display OSD Attitude

    uint8_t osdDisplayHdg;           // 1 = Display OSD Heading
    uint8_t osdDisplayHdgRow;
    uint8_t osdDisplayHdgCol;

    uint8_t osdDisplayHdgBar;	     // 1 = Display OSD Heading Bar (more visual)
    uint8_t osdDisplayHdgBarRow;
    uint8_t osdDisplayHdgBarCol;

    uint8_t osdDisplayVoltage;	     // 1 = Display OSD Voltage
    uint8_t osdDisplayVoltageRow;
    uint8_t osdDisplayVoltageCol;

    uint8_t osdDisplayCurrent;	     // 1 = Display OSD instantaneous current and used current
    uint8_t osdDisplayCurrentRow;
    uint8_t osdDisplayCurrentCol;

    uint8_t osdDisplayThrot;	     // 1 = Display OSD throttle - for now, just rx input, maybe some day show rx input and processed throttle (alt,gps,etc)
    uint8_t osdDisplayThrotRow;
    uint8_t osdDisplayThrotCol;

   	uint8_t osdDisplayRSSI;          // 1 = Display OSD RSSI
   	uint8_t osdDisplayRSSIRow;
   	uint8_t osdDisplayRSSICol;

    uint8_t osdDisplayTimer;
    uint8_t osdDisplayTimerRow;
    uint8_t osdDisplayTimerCol;

    ///////////////////////////////////

    uint8_t  aglPin;
    float    aglScale;
    float    aglBias;

	uint8_t  currentMonitoring;
	uint8_t  currentMonitorPin;
    float    currentMonitorScale;
	float    currentMonitorBias;

	uint8_t  rssiPPM;
	uint8_t  rssiPin;
	uint16_t rssiMax;
	uint16_t rssiMin;
	uint8_t  rssiWarning;

	uint8_t  voltageMonitorPin;
    float    voltageMonitorScale;
    float    voltageMonitorBias;

	uint8_t  batteryCells;

	float    batteryLow;
    float    batteryVeryLow;
    float    batteryMaxLow;

    ///////////////////////////////////

    uint8_t armCount;
    uint8_t disarmCount;

    ///////////////////////////////////

    uint16_t activeTelemetry;

    uint8_t  mavlinkEnabled;

    ///////////////////////////////////

    uint8_t verticalVelocityHoldOnly;

    ///////////////////////////////////

    uint8_t externalHMC5883;
    uint8_t externalMS5611;

    ///////////////////////////////////

    uint8_t  CRCFlags;
    uint32_t CRCAtEnd[1];

} eepromConfig_t;

enum crcFlags { CRC_HistoryBad = 1 };

extern eepromConfig_t eepromConfig;


#endif /* __AUTODRONE32_H__ */
