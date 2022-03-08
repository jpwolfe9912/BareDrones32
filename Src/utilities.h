/** @file 		utilities.h
 *  @brief
 *  	This files contains random useful functions.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

#ifndef __UTILITIES_H__
#define __UTILITIES_H__

/* Defines */
#define white_space(c) ((c) == ' ' || (c) == '\t')
#define valid_digit(c) ((c) >= '0' && (c) <= '9')

/* Function Prototypes */
float constrain(float input, float minValue, float maxValue);
uint16_t constrain16(uint16_t input, uint16_t minValue, uint16_t maxValue);
float standardRadianFormat(float angle);
uint16_t dshot3dFormat(int16_t rxCmd);
float stringToFloat(const char *p);


#endif /* __UTILITIES_H__ */
