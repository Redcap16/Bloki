#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI 3.1415F
#endif

template <>
class std::hash<glm::ivec3>
{
	size_t operator()(const ChunkPos& other) const noexcept
	{
		return (other.x * 42600713) ^ (other.y * 69498817) ^ (other.z * 54492923);
	}
};

class Math
{
public:
	static inline float ToRadian(float deg);
	static inline float ToDegrees(float rad);
	static inline bool IsBetween(float x, float min, float max);

	static inline float Mod(float a, float b);
	static inline int Mod(int a, int b);
};

float Math::ToRadian(float deg)
{
	return deg * (M_PI / 180.0f);
}

float Math::ToDegrees(float rad)
{
	return rad * (180.0f / M_PI);
}

bool Math::IsBetween(float x, float min, float max)
{
	return x >= min && x <= max;
}

float Math::Mod(float a, float b)
{
	return a - b * std::floor(a / b);
}

int Math::Mod(int a, int b)
{
	return a - b * std::floor((float)a / b);
}