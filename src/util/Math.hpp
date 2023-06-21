#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI 3.1415F
#endif

class Math
{
public:
	static inline float ToRadian(float deg)
	{
		return deg * (M_PI / 180.0f);
	}
	static inline float ToDegrees(float rad)
	{
		return rad * (180.0f / M_PI);
	}
	static inline bool IsBetween(float x, float min, float max)
	{
		return x >= min && x <= max;
	}
};