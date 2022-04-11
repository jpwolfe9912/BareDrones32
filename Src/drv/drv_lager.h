/** @file 		drv_lager.h
 *  @brief
 *  	This file initializes the UART and provides functions for writing
 *  	to the OpenLager.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 APR 2022
 */

#ifndef SRC_DRV_DRV_LAGER_H_
#define SRC_DRV_DRV_LAGER_H_

void lagerInit(void);
void lagerWriteLog(uint8_t *pLog);

#endif /* SRC_DRV_DRV_LAGER_H_ */
