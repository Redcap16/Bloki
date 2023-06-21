#pragma once

#include <world/BlockArray.hpp>

class BlockRay
{
public:
	BlockRay(BlockArray& world, glm::vec3 origin, glm::vec3 direction);
	bool Raycast(float length, glm::ivec3& result);
	bool RaycastAhead(float length, glm::ivec3& result);

private:
	BlockArray& m_World;
	glm::vec3 m_Origin, m_Direction;

	void prepare(glm::vec3& step, glm::vec3& max, glm::vec3& delta);
};