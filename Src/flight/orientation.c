/** @file 		orientation.c
 *  @brief
 *  	This file contains a function to orient the board.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		06 MAR 2022
 */

/* Includes */
#include "board.h"

/* Global Variables */
float mpuOrientationMatrix[9];

/** @brief Orients the board.
 *
 *  @return Void.
 */
void
orientSensors(void)
{
    switch (eepromConfig.sensorOrientation)
    {
        case 1: // Normal, no rotation
            mpuOrientationMatrix[0] =  1.0f;
            mpuOrientationMatrix[1] =  0.0f;
            mpuOrientationMatrix[2] =  0.0f;
            mpuOrientationMatrix[3] =  0.0f;
            mpuOrientationMatrix[4] =  1.0f;
            mpuOrientationMatrix[5] =  0.0f;
            mpuOrientationMatrix[6] =  0.0f;
            mpuOrientationMatrix[7] =  0.0f;
            mpuOrientationMatrix[8] =  1.0f;

            break;

        case 2: // 90 degree rotation
            mpuOrientationMatrix[0] =  0.0f;
            mpuOrientationMatrix[1] =  1.0f;
            mpuOrientationMatrix[2] =  0.0f;
            mpuOrientationMatrix[3] =  1.0f;
            mpuOrientationMatrix[4] =  0.0f;
            mpuOrientationMatrix[5] =  0.0f;
            mpuOrientationMatrix[6] =  0.0f;
            mpuOrientationMatrix[7] =  0.0f;
            mpuOrientationMatrix[8] = -1.0f;

            break;

        case 3: // 180 degree rotation
            mpuOrientationMatrix[0] = -1.0f;
            mpuOrientationMatrix[1] =  0.0f;
            mpuOrientationMatrix[2] =  0.0f;
            mpuOrientationMatrix[3] =  0.0f;
            mpuOrientationMatrix[4] =  1.0f;
            mpuOrientationMatrix[5] =  0.0f;
            mpuOrientationMatrix[6] =  0.0f;
            mpuOrientationMatrix[7] =  0.0f;
            mpuOrientationMatrix[8] = -1.0f;

            break;

        case 4: // -90 degree rotation
            mpuOrientationMatrix[0] =  0.0f;
            mpuOrientationMatrix[1] = -1.0f;
            mpuOrientationMatrix[2] =  0.0f;
            mpuOrientationMatrix[3] = -1.0f;
            mpuOrientationMatrix[4] =  0.0f;
            mpuOrientationMatrix[5] =  0.0f;
            mpuOrientationMatrix[6] =  0.0f;
            mpuOrientationMatrix[7] =  0.0f;
            mpuOrientationMatrix[8] = -1.0f;

            break;

        default: // Normal, no rotation
            mpuOrientationMatrix[0] =  1.0f;
            mpuOrientationMatrix[1] =  0.0f;
            mpuOrientationMatrix[2] =  0.0f;
            mpuOrientationMatrix[3] =  0.0f;
            mpuOrientationMatrix[4] =  1.0f;
            mpuOrientationMatrix[5] =  0.0f;
            mpuOrientationMatrix[6] =  0.0f;
            mpuOrientationMatrix[7] =  0.0f;
            mpuOrientationMatrix[8] =  1.0f;

            break;
    }
}

///////////////////////////////////////////////////////////////////////////////

