/** @file 		drv_tim.h
 *  @brief
 *  	This file enables use of a timer to ensure accurate timing of the PID loop.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

#ifndef __DRV_TIM_H__
#define __DRV_TIM_H__

/* Function Prototypes */
void tim9Init(void);
uint32_t tim9GetCnt(void);
void tim9ResetCnt(void);
void tim9Enable(void);
void tim9Disable(void);

#endif /* __DRV_TIM_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
