#include <physics/Rigidbody.hpp>

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

void Rigidbody::checkCollisions()
{
	bool collided;
	m_OnGround = false;

	do
	{
		collided = false;

		glm::vec3 maxCorner = m_Collider.GetMaxCorner(),
			minCorner = m_Collider.GetMinCorner();

		float maxStep = 0;
		glm::vec3 maxDirection = glm::vec3(0);

		for (int x = floor(minCorner.x); x <= floor(maxCorner.x); x++)
			for (int y = floor(minCorner.y); y <= floor(maxCorner.y); y++)
				for (int z = floor(minCorner.z); z <= floor(maxCorner.z); z++)
				{
					AABB block({ x, y, z }, { 1, 1, 1 });
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


						collided = true;
						if (m_Velocity.x && step.x < std::min(step.y, step.z))
							if (maxStep < step.x)
							{
								maxStep = step.x;
								maxDirection = glm::vec3(1, 0, 0);
							}
						else if (m_Velocity.y && step.y < std::min(step.x, step.z))
							if (maxStep < step.y)
							{
								maxStep = step.y;
								maxDirection = glm::vec3(0, 1, 0);
							}
						else if (m_Velocity.z && step.z < std::min(step.x, step.y)) //Experimental
							if (maxStep < step.z)
							{
								maxStep = step.z;
								maxDirection = glm::vec3(0, 0, 1);
							}
						else
							collided = false;
					}
				}
		if (maxDirection == glm::vec3(0, 1, 0) && m_Velocity.y < 0)
			m_OnGround = true;

		m_Collider.Position -= m_Velocity * maxStep * maxDirection;
		m_Velocity -= m_Velocity * maxDirection;
	} while (collided);
}