/** @file 		utilities.c
 *  @brief
 *  	This files contains random useful functions.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		07 MAR 2022
 */

/* Includes */
#include "board.h"

/** @brief Constrains an input between two values.
 *
 *  @param input Value to be constrained.
 *  @param minValue Lower threshold.
 *  @param maxValue Upper threshold.
 *  @return float Constrained value.
 */
float
constrain(float input, float minValue, float maxValue)
{
    if (input < minValue)
        return minValue;
    else if (input > maxValue)
        return maxValue;
    else
        return input;
}

/** @brief Constrains an input uint16_t between two values.
 *
 *  @param input Value to be constrained.
 *  @param minValue Lower threshold.
 *  @param maxValue Upper threshold.
 *  @return uint16_t Constrained value.
 */
uint16_t
constrain16(uint16_t input, uint16_t minValue, uint16_t maxValue)
{
    if (input < minValue)
        return minValue;
    else if (input > maxValue)
        return maxValue;
    else
        return input;
}


/** @brief Puts a value in standard radian format.
 *
 *  @param angle Value to be formatted.
 *  @return float Formatted value.
 */
float
standardRadianFormat(float angle)
{
    if (angle >= PI)
        return (angle - 2 * PI);
    else if (angle < -PI)
        return (angle + 2 * PI);
    else
        return (angle);
}

/** @brief Formats values from an iBus receiver for 3D mode.
 *
 *  @param rxCommand Value from iBus receiver.
 *  @return uint16_t Formatted value.
 */
uint16_t
dshot3dFormat(int16_t rxCmd)
{
	if(rxCmd < 0){
		return (constrain16((47 - rxCmd), 48, 1047));
	}
	else
		return (constrain16((rxCmd + 1048), 1048, 2047));

}

/** @brief Simple and fast atof (ascii to float) function.
 *
 * 		Executes about 5x faster than standard MSCRT library atof()
 * 		-An attractive alternative if the number of calls is in the millions.
 * 		-Assumes input is a proper integer, fraction, or scientific format.
 * 		-Matches library atof() to 15 digits (except at extreme exponents).
 * 		-Follows atof() precedent of essentially no error checking.
 *
 * 		09-May-2009 Tom Van Baak (tvb) www.LeapSecond.com
 *
 *  @param *p Pointer to string.
 *  @return float Converted float value.
 */
float
stringToFloat(const char *p)
{
    int frac = 0;
    double sign, value, scale;

    // Skip leading white space, if any.

    while (white_space(*p) ) {
        p += 1;
    }

    // Get sign, if any.

    sign = 1.0;
    if (*p == '-') {
        sign = -1.0;
        p += 1;

    } else if (*p == '+') {
        p += 1;
    }

    // Get digits before decimal point or exponent, if any.

    value = 0.0;
    while (valid_digit(*p)) {
        value = value * 10.0 + (*p - '0');
        p += 1;
    }

    // Get digits after decimal point, if any.

    if (*p == '.') {
        double pow10 = 10.0;
        p += 1;

        while (valid_digit(*p)) {
            value += (*p - '0') / pow10;
            pow10 *= 10.0;
            p += 1;
        }
    }

    // Handle exponent, if any.

    scale = 1.0;
    if ((*p == 'e') || (*p == 'E')) {
        unsigned int expon;
        p += 1;

        // Get sign of exponent, if any.

        frac = 0;
        if (*p == '-') {
            frac = 1;
            p += 1;

        } else if (*p == '+') {
            p += 1;
        }

        // Get digits of exponent, if any.

        expon = 0;
        while (valid_digit(*p)) {
            expon = expon * 10 + (*p - '0');
            p += 1;
        }
        if (expon > 308) expon = 308;

        // Calculate scaling factor.

        while (expon >= 50) { scale *= 1E50; expon -= 50; }
        while (expon >=  8) { scale *= 1E8;  expon -=  8; }
        while (expon >   0) { scale *= 10.0; expon -=  1; }
    }

    // Return signed and scaled floating point result.

    return sign * (frac ? (value / scale) : (value * scale));
}
