///*
// * sensor_fusion.c
// *
// *  Created on: Feb 7, 2022
// *      Author: jeremywolfe
// */
//#include "board.h"
//
//void MadgwickUpdate(float gx, float gy, float gz,
//					float ax, float ay, float az, float deltat) {
//	float norm, normR;
//	float s0, s1, s2, s3;
//	float qDot1, qDot2, qDot3, qDot4;
//	float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;
//
//	// Rate of change of quaternion from gyroscope
//	qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
//	qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
//	qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
//	qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);
//
//	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
//	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
//
//		// Normalise accelerometer measurement
//		recipNorm = sqrt(SQR(ax) + SQR(ay) + SQR(az));
//		ax *= recipNorm;
//		ay *= recipNorm;
//		az *= recipNorm;
//
//		// Auxiliary variables to avoid repeated arithmetic
//		_2q0 = 2.0f * q0;
//		_2q1 = 2.0f * q1;
//		_2q2 = 2.0f * q2;
//		_2q3 = 2.0f * q3;
//		_4q0 = 4.0f * q0;
//		_4q1 = 4.0f * q1;
//		_4q2 = 4.0f * q2;
//		_8q1 = 8.0f * q1;
//		_8q2 = 8.0f * q2;
//		q0q0 = q0 * q0;
//		q1q1 = q1 * q1;
//		q2q2 = q2 * q2;
//		q3q3 = q3 * q3;
//
//		// Gradient decent algorithm corrective step
//		s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
//		s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
//		s2 = 4.0f * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
//		s3 = 4.0f * q1q1 * q3 - _2q1 * ax + 4.0f * q2q2 * q3 - _2q2 * ay;
//		recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
//		s0 *= recipNorm;
//		s1 *= recipNorm;
//		s2 *= recipNorm;
//		s3 *= recipNorm;
//
//		// Apply feedback step
//		qDot1 -= beta * s0;
//		qDot2 -= beta * s1;
//		qDot3 -= beta * s2;
//		qDot4 -= beta * s3;
//	}
//
//	// Integrate rate of change of quaternion to yield quaternion
//	q0 += qDot1 * deltat;
//	q1 += qDot2 * deltat;
//	q2 += qDot3 * deltat;
//	q3 += qDot4 * deltat;
//
//	// Normalise quaternion
//	recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
//	q0 *= recipNorm;
//	q1 *= recipNorm;
//	q2 *= recipNorm;
//	q3 *= recipNorm;
//	anglesComputed = 0;
//}
//
//bool initQuat(float ax, float ay, float az, float mx, float my, float mz){
//	// Compute feedback only if accelerometer measurement valid
//	// (avoids NaN in accelerometer normalisation)
//	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
//		float recipNorm;
//		float N[3], D[3], E[3]; //Global vectors North, Down, East relative to sensor
//
//		// Down is negative accelerometer measurement
//		recipNorm = invSqrt(ax * ax + ay * ay + az * az);
//		D[0] = - ax * recipNorm;
//		D[1] = - ay * recipNorm;
//		D[2] = - az * recipNorm;
//
//		// Magnetometer is not exatly perpendicular to Down, therefor not exatly North
//		// we will calculate North later
//		float m[3];
//		recipNorm = invSqrt(mx * mx + my * my + mz * mz);
//		m[0] = mx * recipNorm;
//		m[1] = mx * recipNorm;
//		m[2] = mx * recipNorm;
//
//		// Calculate East
//		vectorCross(m, D, E);
//		recipNorm = invSqrt(E[0] * E[0] + E[1] * E[1] + E[2] * E[2]);
//		E[0] *= recipNorm;
//		E[1] *= recipNorm;
//		E[2] *= recipNorm;
//
//		// Calculate North
//		vectorCross(D, E, N);
//
//		// Calculate Euler Parameter (quaternion) from the rotation matrix A=(N|D|E).
//		// Using Shepperd algorithm (Woernle 2011)
//		float Trace = N[0] + D[1] + E[2];
//		float a[4] = {Trace, N[0], D[1], E[2]};
//		float e[4];
//
//		//find index of Largest Euler parameter
//		int k=0;
//		for (int i=1; i<4; i++){
//			if (a[i] > a[k])
//				k = i;
//		}
//		//calculate that parameter
//		e[k] = sqrt(1 + 2 * a[k] - Trace)/2;
//
//		switch (k){
//		case 0:
//			e[1] = (D[2] - E[1]) / (4 * e[0]);
//			e[2] = (E[0] - N[2]) / (4 * e[0]);
//			e[3] = (N[1] - D[0]) / (4 * e[0]);
//			break;
//		case 1:
//			e[0] = (D[2] - E[1]) / (4 * e[1]);
//			e[2] = (D[0] + N[1]) / (4 * e[1]);
//			e[3] = (E[0] + N[2]) / (4 * e[1]);
//			break;
//		case 2:
//			e[0] = (E[0] - N[2]) / (4 * e[2]);
//			e[1] = (D[0] + N[1]) / (4 * e[2]);
//			e[3] = (E[1] + D[2]) / (4 * e[2]);
//			break;
//		case 3:
//			e[0] = (N[1] - D[0]) / (4 * e[3]);
//			e[1] = (E[0] + N[2]) / (4 * e[3]);
//			e[2] = (E[1] + D[2]) / (4 * e[3]);
//		}
//
//		// invert the quaternion rotation
//		// we calculated the rotation of Global NDE relative to the sensor
//		// but we need the rotation of the Sensor relative to NDE
//		q0 = e[0];
//		q1 = -e[1];
//		q2 = -e[2];
//		q3 = -e[3];
//
//		return true;
//	} else return false;
//}
//
//void vectorCross(float A[3], float B[3], float cross[3])
//{
//	cross[0] = A[1] * B[2] - A[2] * B[1];
//	cross[1] = A[2] * B[0] - A[0] * B[2];
//	cross[2] = A[0] * B[1] - A[1] * B[0];
//}
