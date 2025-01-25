#include <game/physics/Rigidbody.hpp>

Rigidbody::Rigidbody(const BlockManager& world, AABB collider) :
	m_World(world),
	m_Collider(collider),
	m_OnGround(false),
	m_Velocity(0),
	m_GravityEnabled(true),
	m_CollisionEnabled(true),
	m_DragEnabled(true)
{

}

void Rigidbody::Update(float deltaTime)
{
	if(m_GravityEnabled) m_Velocity.y -= c_Gravity * deltaTime; //Gravity
	if (m_DragEnabled)
	{
		m_Velocity -= m_Velocity * c_Drag * deltaTime; //Drag
		if (m_OnGround)
		{
			m_Velocity.x -= m_Velocity.x * c_GroundFriction * deltaTime;
			m_Velocity.z -= m_Velocity.z * c_GroundFriction * deltaTime;
		}
	}

	m_Collider.Position += m_Velocity * deltaTime;
	if(m_CollisionEnabled) checkCollisions();
}

void Rigidbody::Serialize(std::vector<char>& data) const {
	const char* aabbdata = reinterpret_cast<const char*>(&m_Collider);
	data.insert(data.end(), aabbdata, aabbdata + sizeof(m_Collider));

	const char* velocity = reinterpret_cast<const char*>(&m_Velocity);
	data.insert(data.end(), velocity, velocity + sizeof(m_Velocity));

	data.push_back(static_cast<char>(m_OnGround));
	data.push_back(static_cast<char>(m_GravityEnabled));
	data.push_back(static_cast<char>(m_CollisionEnabled));
	data.push_back(static_cast<char>(m_DragEnabled));
}

std::unique_ptr<Rigidbody> Rigidbody::Deserialize(const std::vector<char>& data, const BlockManager& world) {
	if (data.size() != sizeof(Rigidbody::m_Collider) +
		sizeof(Rigidbody::m_Velocity) +
		4 * sizeof(bool))
		return nullptr;

	size_t currentIndex = 0;
	AABB collider = *reinterpret_cast<const AABB*>(&data[currentIndex]);
	currentIndex += sizeof(AABB);

	Rigidbody result(world, collider);
	result.m_Velocity = *reinterpret_cast<const glm::vec3*>(&data[currentIndex]);
	currentIndex += sizeof(Rigidbody::m_Velocity);

	result.m_OnGround = static_cast<bool>(data[currentIndex++]);
	result.m_GravityEnabled = static_cast<bool>(data[currentIndex++]);
	result.m_CollisionEnabled = static_cast<bool>(data[currentIndex++]);
	result.m_DragEnabled = static_cast<bool>(data[currentIndex++]);

	return std::make_unique<Rigidbody>(result);
}

void Rigidbody::checkCollisions()
{
	m_OnGround = false;

	glm::vec3 maxDirection;
	do
	{
		glm::vec3 maxCorner = m_Collider.GetMaxCorner(),
			minCorner = m_Collider.GetMinCorner();

		float maxStep = 0;
		maxDirection = glm::vec3(0);

		for (int x = (int)floor(minCorner.x); x <= (int)floor(maxCorner.x); x++)
			for (int y = (int)floor(minCorner.y); y <= (int)floor(maxCorner.y); y++)
				for (int z = (int)floor(minCorner.z); z <= (int)floor(maxCorner.z); z++)
				{
					AABB block({ x, y, z }, { 0.5f, 0.5, 0.5f }, { 1, 1, 1 });
					if (m_Collider.Intersects(block))
					{
						const glm::vec3 actualBlockPosition = glm::ivec3(x, y, z);
						const Block block = m_World.GetBlock(actualBlockPosition);

						if (block.Type == Block::BlockType::Air)
							continue;

						glm::vec3 intersection,
							step;

						if (m_Velocity.x > 0)
							intersection.x = maxCorner.x - actualBlockPosition.x;
						else
							intersection.x = actualBlockPosition.x + 1 - minCorner.x;

						if (m_Velocity.y > 0)
							intersection.y = maxCorner.y - actualBlockPosition.y;
						else
							intersection.y = actualBlockPosition.y + 1 - minCorner.y;

						if (m_Velocity.z > 0)
							intersection.z = maxCorner.z - actualBlockPosition.z;
						else
							intersection.z = actualBlockPosition.z + 1 - minCorner.z;

						step = intersection / glm::abs(m_Velocity);

						if (m_Velocity.x && step.x < std::min(step.y, step.z))
						{
							if (maxStep < step.x)
							{
								maxStep = step.x;
								maxDirection = glm::vec3(1, 0, 0);
							}
						}
						else if (m_Velocity.y && step.y < std::min(step.x, step.z))
						{
							if (maxStep < step.y)
							{
								maxStep = step.y;
								maxDirection = glm::vec3(0, 1, 0);
							}
						}
						else if (m_Velocity.z && step.z < std::min(step.x, step.y)) //Experimental
						{
							if (maxStep < step.z)
							{
								maxStep = step.z;
								maxDirection = glm::vec3(0, 0, 1);
							}
						}
					}
				}
		if (maxDirection == glm::vec3(0, 1, 0) && m_Velocity.y < 0)
			m_OnGround = true;

		m_Collider.Position -= m_Velocity * maxStep * maxDirection * c_CollisionCorrection;
		m_Velocity -= m_Velocity * maxDirection;
	} while (maxDirection != glm::vec3(0));
}