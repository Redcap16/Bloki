#include <physics/BlockRay.hpp>

BlockRay::BlockRay(const BlockManager& world, const glm::vec3& origin, const glm::vec3& direction) :
	m_World(world),
	m_Origin(origin),
	m_Direction(direction)
{
	setup();
}

bool BlockRay::Raycast(float length, glm::ivec3& result) const
{
	glm::ivec3 position = glm::floor(m_Origin);
	glm::vec3 max = m_Max;

	while(true)
	{
		if (glm::length(m_Origin - (glm::vec3)position) > length)
			break;

		if ((m_Direction.z == 0 || m_Max.x < m_Max.z) && (m_Direction.y == 0 || m_Max.x < m_Max.y))
		{
			max.x += m_Delta.x;
			position.x += m_Step.x;
		}
		else if ((m_Direction.z == 0 || m_Max.y < m_Max.z) && (m_Direction.x == 0 || m_Max.y < m_Max.x))
		{
			max.y += m_Delta.y;
			position.y += m_Step.y;
		}
		else if ((m_Direction.x == 0 || m_Max.z < m_Max.x) && (m_Direction.y == 0 || m_Max.z < m_Max.y))
		{
			max.z += m_Delta.z;
			position.z += m_Step.z;
		}

		const Block block = m_World.GetBlock(position);
		if (block.Type != Block::BlockType::Air)
		{
			result = position;
			return true;
		}
	}

	return false;
}

bool BlockRay::RaycastAhead(float length, glm::ivec3& result) const
{
	glm::ivec3 position = glm::floor(m_Origin),
		lastPosition = position;
	bool firstTime = true;

	glm::vec3 max = m_Max;

	while (true)
	{
		if (glm::length(m_Origin - (glm::vec3)position) > length)
			break;

		if ((m_Direction.z == 0 || m_Max.x < m_Max.z) && (m_Direction.y == 0 || m_Max.x < m_Max.y))
		{
			max.x += m_Delta.x;
			position.x += m_Step.x;
		}
		else if ((m_Direction.z == 0 || m_Max.y < m_Max.z) && (m_Direction.x == 0 || m_Max.y < m_Max.x))
		{
			max.y += m_Delta.y;
			position.y += m_Step.y;
		}
		else if ((m_Direction.x == 0 || m_Max.z < m_Max.x) && (m_Direction.y == 0 || m_Max.z < m_Max.y))
		{
			max.z += m_Delta.z;
			position.z += m_Step.z;
		}

		const Block block = m_World.GetBlock(position);
		if (!firstTime &&
			block.Type != Block::BlockType::Air)
		{
			result = lastPosition;
			return true;
		}

		lastPosition = position;
		firstTime = false;
	}

	return false;
}

void BlockRay::setup()
{
	m_Step.x = (m_Direction.x > 0) - (m_Direction.x < 0);
	m_Step.y = (m_Direction.y > 0) - (m_Direction.y < 0);
	m_Step.z = (m_Direction.z > 0) - (m_Direction.z < 0);

	m_Delta = glm::vec3(1) / glm::abs(m_Direction);

	if (m_Direction.x > 0)
		m_Max.x = (ceil(m_Origin.x) - m_Origin.x) / abs(m_Direction.x);
	else if (m_Direction.x < 0)
		m_Max.x = (m_Origin.x - floor(m_Origin.x)) / abs(m_Direction.x);

	if (m_Direction.y > 0)
		m_Max.y = (ceil(m_Origin.y) - m_Origin.y) / abs(m_Direction.y);
	else if (m_Direction.y < 0)
		m_Max.y = (m_Origin.y - floor(m_Origin.y)) / abs(m_Direction.y);

	if (m_Direction.z > 0)
		m_Max.z = (ceil(m_Origin.z) - m_Origin.z) / abs(m_Direction.z);
	else if (m_Direction.z < 0)
		m_Max.z = (m_Origin.z - floor(m_Origin.z)) / abs(m_Direction.z);
}