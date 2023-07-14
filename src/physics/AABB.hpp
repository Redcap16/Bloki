#pragma once

#include <glm/glm.hpp>

struct AABB
{
	glm::vec3 Position;
	glm::vec3 Size;

	AABB();
	AABB(const glm::vec3& position, const glm::vec3& size);

	inline glm::vec3 GetMaxCorner() const;
	inline glm::vec3 GetMinCorner() const;
	bool Intersects(const AABB& aabb) const;
};

glm::vec3 AABB::GetMaxCorner() const
{
	return Position + Size / 2.0f;
}

glm::vec3 AABB::GetMinCorner() const
{
	return Position - Size / 2.0f;
}