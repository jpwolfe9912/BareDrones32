/** @file 		drv_dshot.h
 *  @brief
 *  	This files enables the gpio, timer, and dma peripherals
 *  	to send dshot commands with pwm.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 *  @bug
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DRV_DSHOT_H_
#define __DRV_DSHOT_H_

/* Defines */
#define TIMER_CLOCK				108000000	// 108MHz

#define MHZ_TO_HZ(x) 			((x) * 1000000)

#define DSHOT600_HZ     		MHZ_TO_HZ(12)
#define DSHOT300_HZ     		MHZ_TO_HZ(6)
#define DSHOT150_HZ     		MHZ_TO_HZ(3)

//#define MOTOR_BIT_0            	7
//#define MOTOR_BIT_1            	14
//#define MOTOR_BITLENGTH        	20

#define MOTOR_BIT_0            	60
#define MOTOR_BIT_1            	120
#define MOTOR_BITLENGTH        	180

#define DSHOT_FRAME_SIZE       	16
#define DSHOT_DMA_BUFFER_SIZE   18 /* resolution + frame reset (2us) */

#define DSHOT_MIN_THROTTLE      48
#define DSHOT_MAX_THROTTLE     	2047
#define DSHOT_RANGE 			(DSHOT_MAX_THROTTLE - DSHOT_MIN_THROTTLE)

/* Enumeration */
typedef enum
{
    DSHOT150,
    DSHOT300,
    DSHOT600
} dshot_type_e;

/* Function Prototypes */
void dshotInit(dshot_type_e dshot_type);
void dshotWrite(uint16_t *motor_value);


#endif /* __DRV_DSHOT_H_ */
