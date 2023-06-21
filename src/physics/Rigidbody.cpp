#include <physics/Rigidbody.hpp>

Rigidbody::Rigidbody(BlockArray& world, AABB collider) :
	m_World(world),
	m_Collider(collider),
	m_OnGround(false),
	m_Velocity(0)
{

}

void Rigidbody::Update(float deltaTime)
{
	m_Velocity.y -= 0.7f * deltaTime;

	m_Velocity -= m_Velocity * 0.3f * deltaTime; //Drag

	if (m_OnGround)
	{
		m_Velocity.x -= m_Velocity.x * 1.0f * deltaTime;
		m_Velocity.z -= m_Velocity.z * 1.0f * deltaTime;
	}

	m_Collider.Position += m_Velocity * deltaTime;

	DebugBox::GetInstance().ChangeDebugValue("VelocityB", std::to_string(m_Velocity.x) + " " + std::to_string(m_Velocity.y) + " " + std::to_string(m_Velocity.z));
	checkCollisions();

	DebugBox::GetInstance().ChangeDebugValue("VelocityA", std::to_string(m_Velocity.x) + " " + std::to_string(m_Velocity.y) + " " + std::to_string(m_Velocity.z));
}

void Rigidbody::checkCollisions()
{
	bool collided = true;

	m_OnGround = false;
	while (collided)
	{
		glm::vec3 pCorner = m_Collider.GetMaxCorner(),
			mCorner = m_Collider.GetMinCorner();

		float maxStep = 0;
		glm::vec3 maxDirection = glm::vec3(0);

		collided = false;
		for (int x = floor(mCorner.x); x <= floor(pCorner.x); x++)
		{
			for (int y = floor(mCorner.y); y <= floor(pCorner.y); y++)
			{
				for (int z = floor(mCorner.z); z <= floor(pCorner.z); z++)
				{
					if (mCorner.x < x + 1 &&
						pCorner.x > x &&
						mCorner.y < y + 1 &&
						pCorner.y > y &&
						mCorner.z < z + 1 &&
						pCorner.z > z)
					{
						const glm::vec3 actualBlockPosition = glm::ivec3(x, y, z);
						const Block* block = m_World.GetBlock(actualBlockPosition);

						if (block == nullptr)
							continue;

						if (block->type != Block::BlockType::Air)
						{
							glm::vec3 intersection = glm::vec3(0), 
								step = glm::vec3(0);

							if (m_Velocity.x > 0)
								intersection.x = pCorner.x - actualBlockPosition.x;
							else
								intersection.x = actualBlockPosition.x + 1 - mCorner.x;

							if (m_Velocity.y > 0)
								intersection.y = pCorner.y - actualBlockPosition.y;
							else
								intersection.y = actualBlockPosition.y + 1 - mCorner.y;

							if (m_Velocity.z > 0)
								intersection.z = pCorner.z - actualBlockPosition.z;
							else
								intersection.z = actualBlockPosition.z + 1 - mCorner.z;

							step = intersection / glm::abs(m_Velocity);

							if (m_Velocity.x && step.x < step.z && step.x < step.y)
							{
								if (maxStep < step.x)
								{
									maxStep = step.x;
									maxDirection = glm::vec3(1, 0, 0);
								}
								collided = true;
							}
							else if (m_Velocity.y && step.y < step.z && step.y < step.x)
							{
								if (maxStep < step.y)
								{
									maxStep = step.y;
									maxDirection = glm::vec3(0, 1, 0);
								}
							
								collided = true;
							}
							else if (m_Velocity.z)
							{
 								if (maxStep < step.z)
								{
									maxStep = step.z;
									maxDirection = glm::vec3(0, 0, 1);
								}
								collided = true;
							}
						}
					}
				}
			}
		}

		if (maxDirection == glm::vec3(0, 1, 0) && m_Velocity.y < 0)
			m_OnGround = true;

		m_Collider.Position -= m_Velocity * maxStep * maxDirection;
		m_Velocity -= m_Velocity * maxDirection;
	}

	DebugBox::GetInstance().ChangeDebugValue("onGround ", m_OnGround ? "true" : "false");
}