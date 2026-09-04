/** @file 		time.h
 *  @brief
 *  	This file handles time related functions.
 *
 *  @author 	Jeremy Wolfe
 *  @date 		23 FEB 2022
 */

#ifndef __TIME_H__
#define __TIME_H__

#include <stdint.h>

// time difference, 32 bits always sufficient
typedef int32_t timeDelta_t;
// millisecond time
typedef uint32_t timeMs_t ;
// microsecond time
#ifdef USE_64BIT_TIME
typedef uint64_t timeUs_t;
#define TIMEUS_MAX UINT64_MAX
#else
typedef uint32_t timeUs_t;
#define TIMEUS_MAX UINT32_MAX
#endif

#define TIMEZONE_OFFSET_MINUTES_MIN -780  // -13 hours
#define TIMEZONE_OFFSET_MINUTES_MAX 780   // +13 hours

static inline timeDelta_t cmpTimeUs(timeUs_t a, timeUs_t b) { return (timeDelta_t)(a - b); }
static inline int32_t cmpTimeCycles(uint32_t a, uint32_t b) { return (int32_t)(a - b); }

#define FORMATTED_DATE_TIME_BUFSIZE 30

#endif