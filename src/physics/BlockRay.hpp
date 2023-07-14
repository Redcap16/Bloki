#pragma once

#include <world/BlockManager.hpp>

class BlockRay
{
public:
	BlockRay(const BlockManager& world, const glm::vec3& origin, const glm::vec3& direction);

	bool Raycast(float length, glm::ivec3& result) const;
	bool RaycastAhead(float length, glm::ivec3& result) const;

private:
	const BlockManager& m_World;
	glm::vec3 m_Origin, m_Direction;
	
	glm::vec3 m_Step, m_Max, m_Delta;

	void setup();
};