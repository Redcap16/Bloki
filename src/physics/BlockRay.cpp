#include <physics/BlockRay.hpp>

BlockRay::BlockRay(BlockArray& world, glm::vec3 origin, glm::vec3 direction) :
	m_World(world),
	m_Origin(origin),
	m_Direction(direction)
{

}

bool BlockRay::Raycast(float length, glm::ivec3& result)
{
	glm::ivec3 position = glm::floor(m_Origin);
	glm::vec3 tdelta, tmax = glm::vec3(0), step;
	
	prepare(step, tmax, tdelta);

	while(true)
	{
		if (glm::length(m_Origin - (glm::vec3)position) > length)
			break;

		if ((m_Direction.z == 0 || tmax.x < tmax.z) && (m_Direction.y == 0 || tmax.x < tmax.y))
		{
			tmax.x += tdelta.x;
			position.x += step.x;
		}
		else if ((m_Direction.z == 0 || tmax.y < tmax.z) && (m_Direction.x == 0 || tmax.y < tmax.x))
		{
			tmax.y += tdelta.y;
			position.y += step.y;
		}
		else
		{
			tmax.z += tdelta.z;
			position.z += step.z;
		}

		const Block* block = m_World.GetBlock(position);
		if (block != nullptr && 
			block->type != Block::BlockType::Air)
		{
			result = position;
			return true;
		}
	}

	return false;
}

bool BlockRay::RaycastAhead(float length, glm::ivec3& result)
{
	glm::ivec3 position = glm::floor(m_Origin),
		lastPosition = position;
	bool firstTime = true;

	glm::vec3 tdelta, tmax = glm::vec3(0), step;

	prepare(step, tmax, tdelta);

	while (true)
	{
		if (glm::length(m_Origin - (glm::vec3)position) > length)
			break;

		if ((m_Direction.z == 0 || tmax.x < tmax.z) && (m_Direction.y == 0 || tmax.x < tmax.y))
		{
			tmax.x += tdelta.x;
			position.x += step.x;
		}
		else if ((m_Direction.z == 0 || tmax.y < tmax.z) && (m_Direction.x == 0 || tmax.y < tmax.x))
		{
			tmax.y += tdelta.y;
			position.y += step.y;
		}
		else if ((m_Direction.x == 0 || tmax.z < tmax.x) && (m_Direction.y == 0 || tmax.z < tmax.y))
		{
			tmax.z += tdelta.z;
			position.z += step.z;
		}

		const Block* block = m_World.GetBlock(position);
		if (!firstTime &&
			block != nullptr &&
			block->type != Block::BlockType::Air)
		{
			result = lastPosition;
			return true;
		}

		lastPosition = position;
		firstTime = false;
	}

	return false;
}

void BlockRay::prepare(glm::vec3& step, glm::vec3& max, glm::vec3& delta)
{
	step.x = (m_Direction.x > 0) - (m_Direction.x < 0);
	step.y = (m_Direction.y > 0) - (m_Direction.y < 0);
	step.z = (m_Direction.z > 0) - (m_Direction.z < 0);

	delta = glm::vec3(1) / glm::abs(m_Direction);

	if (m_Direction.x > 0)
	{
		step.x = 1;
		max.x = (ceil(m_Origin.x) - m_Origin.x) / abs(m_Direction.x);
	}
	else if (m_Direction.x < 0)
	{
		step.x = -1;
		max.x = (m_Origin.x - floor(m_Origin.x)) / abs(m_Direction.x);
	}

	if (m_Direction.y > 0)
	{
		step.y = 1;
		max.y = (ceil(m_Origin.y) - m_Origin.y) / abs(m_Direction.y);
	}
	else if (m_Direction.y < 0)
	{
		step.y = -1;
		max.y = (m_Origin.y - floor(m_Origin.y)) / abs(m_Direction.y);
	}

	if (m_Direction.z > 0)
	{
		step.z = 1;
		max.z = (ceil(m_Origin.z) - m_Origin.z) / abs(m_Direction.z);
	}
	else if (m_Direction.z < 0)
	{
		step.z = -1;
		max.z = (m_Origin.z - floor(m_Origin.z)) / abs(m_Direction.z);
	}
}