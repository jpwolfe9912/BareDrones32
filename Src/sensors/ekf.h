/*
 * ekf.h
 *
 *  Created on: Mar 3, 2022
 *      Author: jeremywolfe
 */

#ifndef SRC_SENSORS_EKF_H_
#define SRC_SENSORS_EKF_H_

/* We will set the variables like so, these can also be tuned by the user */
Q_angle   = 0.001f;
Q_bias    = 0.003f;
R_measure = 0.03f;

angle = 0.0f; // Reset the angle
bias  = 0.0f; // Reset bias

P[0][0] = 0.0f; // Since we assume that the bias is 0 and we know the starting angle (use setAngle), the error covariance matrix is set like so - see: http://en.wikipedia.org/wiki/Kalman_filter#Example_application.2C_technical
P[0][1] = 0.0f;
P[1][0] = 0.0f;
P[1][1] = 0.0f;

float getAngle(float newAngle, float newRate, float dt);
void setAngle(float newAngle);
float getRate(void);

void setQangle(float newQ_angle);
void setQangle(float newQ_angle);
void setQbias(float newQ_bias);
void setRmeasure(float newR_measure);
float getQangle(void);
float getQbias(void);
float getRmeasure(void);

#endif /* SRC_SENSORS_EKF_H_ */
