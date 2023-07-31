#pragma once

#include <cmath>
#include <glm/glm.hpp>

#ifndef M_PI
#define M_PI 3.1415F
#endif

inline void hash_combine(std::size_t& seed) { }

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) 
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	hash_combine(seed, rest...);
} //Thanks to Matteo Italia

template <>
class std::hash<glm::ivec3>
{
public:
	size_t operator()(const glm::ivec3& other) const noexcept
	{
		return (other.x * 42600713LL) ^ (other.y * 69498817LL) ^ (other.z * 54492923LL) % SIZE_MAX;
	}
};

template <>
class std::hash<glm::vec<3, uint8_t>>
{
public:
	size_t operator()(const glm::vec<3, uint8_t>& other) const noexcept
	{
		return (other.x * 42600713LL) ^ (other.y * 69498817LL) ^ (other.z * 54492923LL) % SIZE_MAX;
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
	static inline glm::ivec3 Mod(const glm::ivec3& a, const glm::ivec3& b);
};

float Math::ToRadian(float deg)
{
	return deg * ((float)M_PI / 180.0f);
}

float Math::ToDegrees(float rad)
{
	return rad * (180.0f / (float)M_PI);
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
	return a - b * (int)std::floor((float)a / b);
}

glm::ivec3 Math::Mod(const glm::ivec3& a, const glm::ivec3& b)
{
	return a - b * (glm::ivec3)glm::floor((glm::vec3)a / (glm::vec3)b);
}