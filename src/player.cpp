#include <player.hpp>

Player::Player(World& world) :
	m_Rigidbody(world, { glm::vec3(0), glm::vec3(0.8f, 1.8f, 0.8f) }),
	rotation(0),
	m_World(world)
{
}

void Player::destroyBlock()
{
	if (eyeCamera == nullptr)
		return;

	/*glm::vec3 direction = glm::normalize(glm::vec3(
		cos(Math::ToRadian(rotation.y)) * sin(Math::ToRadian(rotation.x)),
		sin(Math::ToRadian(rotation.y)),
		-cos(Math::ToRadian(rotation.y)) * cos(Math::ToRadian(rotation.x))
	));*/

	glm::ivec3 hit;
	BlockRay ray(m_World, m_Rigidbody.GetPosition() + glm::vec3(0, 0.5f, 0), eyeCamera->GetDirection());

	if (ray.RaycastAhead(workingDistance, hit))
	{
		glm::ivec3 chunkPos = glm::floor((glm::vec3)hit / (glm::vec3)ChunkBlockData::ChunkSize),
			blockPos = hit - chunkPos * (glm::ivec3)ChunkBlockData::ChunkSize;

		Chunk* chunk = m_World.GetChunk(chunkPos);
		if (chunk != nullptr)
		{
			Block a(Block::BlockType::Wood);
			chunk->SetBlock(blockPos, a);
			m_World.UpdateChunks();
		}
	}
}

void Player::highlightUpdate()
{
	if (eyeCamera == nullptr)
		return;

	glm::ivec3 hit;
	BlockRay ray(m_World, m_Rigidbody.GetPosition() + glm::vec3(0, 0.5f, 0), eyeCamera->GetDirection());

	if (ray.Raycast(5, hit))
	{
		if (anythingHighlighted)
		{
			glm::ivec3 chunkPos = glm::floor((glm::vec3)highlightedBlock / (glm::vec3)ChunkBlockData::ChunkSize),
				blockPos = highlightedBlock - chunkPos * (glm::ivec3)ChunkBlockData::ChunkSize;

			Chunk* chunk = m_World.GetChunk(chunkPos);
			if (chunk)
				chunk->ResetHighlight();
		}
		anythingHighlighted = true;
		highlightedBlock = hit;

		glm::ivec3 chunkPos = glm::floor((glm::vec3)hit / (glm::vec3)ChunkBlockData::ChunkSize),
			blockPos = hit - chunkPos * (glm::ivec3)ChunkBlockData::ChunkSize;

		Chunk* chunk = m_World.GetChunk(chunkPos);
		if (chunk)
		{
			chunk->SetHighlight(blockPos);
			m_World.UpdateChunks();
			DebugBox::GetInstance().ChangeDebugValue("HIGH", std::to_string(hit.x) + " " + std::to_string(hit.y) + " " + std::to_string(hit.z));
		}
	}
	else
	{
		if (anythingHighlighted)
		{
			glm::ivec3 chunkPos = glm::floor((glm::vec3)highlightedBlock / (glm::vec3)ChunkBlockData::ChunkSize),
				blockPos = highlightedBlock - chunkPos * (glm::ivec3)ChunkBlockData::ChunkSize;

			Chunk* chunk = m_World.GetChunk(chunkPos);
			if (chunk)
			{
				chunk->ResetHighlight();
				m_World.UpdateChunks();
			}
		}
	}
}

void Player::placeBlock()
{
	glm::vec3 direction = glm::normalize(glm::vec3(
		cos(Math::ToRadian(rotation.y)) * sin(Math::ToRadian(rotation.x)),
		sin(Math::ToRadian(rotation.y)),
		-cos(Math::ToRadian(rotation.y)) * cos(Math::ToRadian(rotation.x))
	));

	glm::ivec3 hit;
	BlockRay ray(m_World, m_Rigidbody.GetPosition() + glm::vec3(0, 0.5f, 0), direction);

	if (ray.Raycast(workingDistance, hit))
	{
		glm::ivec3 chunkPos = glm::floor((glm::vec3)hit / (glm::vec3)ChunkBlockData::ChunkSize),
			blockPos = hit - chunkPos * (glm::ivec3)ChunkBlockData::ChunkSize;

		Chunk* chunk = m_World.GetChunk(chunkPos);
		if (chunk != nullptr)
		{
			Block a(Block::BlockType::Air);
			chunk->SetBlock(blockPos, a);
			m_World.UpdateChunks();
		}
	}
}

//
//void Player::checkCollisions()
//{
//
//	bool collided = true;
//
//
//	bool touchingGround = false;
//
//	DebugBox::GetInstance().ChangeDebugValue("Collided?: ", "false");
//
//	onGround = false;
//	while (collided)
//	{
//		glm::vec3 pCorner = position + glm::vec3(0.4f, 0.4f, 0.4f),
//			mCorner = position - glm::vec3(0.4f, 1.5f, 0.4f);
//
//		float maxStep = 0;
//		glm::vec3 maxDirection = glm::vec3(0);
//
//		collided = false;
//		for (int x = floor(mCorner.x); x <= floor(pCorner.x); x++)
//		{
//			for (int y = floor(mCorner.y); y <= floor(pCorner.y); y++)
//			{
//				for (int z = floor(mCorner.z); z <= floor(pCorner.z); z++)
//				{
//					if (mCorner.x < x + 1 &&
//						pCorner.x > x &&
//						mCorner.y < y + 1 &&
//						pCorner.y > y &&
//						mCorner.z < z + 1 &&
//						pCorner.z > z)
//					{
//						const glm::vec3 actualBlockPosition = glm::ivec3(x, y, z);
//						glm::ivec3 chunkPosition = glm::floor(actualBlockPosition / (glm::vec3)ChunkBlockData::ChunkSize),
//							blockPosition = actualBlockPosition - (glm::vec3)chunkPosition * (glm::vec3)ChunkBlockData::ChunkSize;
//						Chunk* chunk = m_World.GetChunk(chunkPosition);
//						if (chunk == nullptr)
//							continue;
//						const Block& block = chunk->GetBlock(blockPosition);
//
//						if (block.type != Block::BlockType::Air)
//						{
//							glm::vec3 intersection, step;
//							if (velocity.x > 0)
//								intersection.x = pCorner.x - actualBlockPosition.x;
//							else
//								intersection.x = actualBlockPosition.x + 1 - mCorner.x;
//
//							if (velocity.y > 0)
//								intersection.y = pCorner.y - actualBlockPosition.y;
//							else
//								intersection.y = actualBlockPosition.y + 1 - mCorner.y;
//
//							if (velocity.z > 0)
//								intersection.z = pCorner.z - actualBlockPosition.z;
//							else
//								intersection.z = actualBlockPosition.z + 1 - mCorner.z;
//
//							step = intersection / glm::abs(velocity);
//
//							if (velocity.x && step.x < step.z && step.x < step.y)
//							{
//								if (maxStep < step.x)
//								{
//									maxStep = step.x;
//									maxDirection = glm::vec3(1, 0, 0);
//								}
//								collided = true;
//							}
//							else if (velocity.y && step.y < step.z && step.y < step.x)
//							{
//								if (maxStep < step.y)
//								{
//									maxStep = step.y;
//									maxDirection = glm::vec3(0, 1, 0);
//								}
//
//								//if (step.y > 0)
//								//	touchingGround = true;
//								collided = true;
//							}
//							else if(velocity.z)
//							{
//								if (maxStep < step.z)
//								{
//									maxStep = step.z;
//									maxDirection = glm::vec3(0, 0, 1);
//								}
//								collided = true;
//							}
//
//						}
//					}
//				}
//			}
//		}
//
//		if (maxDirection == glm::vec3(0, 1, 0) && velocity.y < 0)
//			onGround = true;
//
//		position -= velocity * maxStep * maxDirection;
//		velocity -= velocity * maxDirection;
//	}
//
//
//	//onGround = touchingGround;
//
//	DebugBox::GetInstance().ChangeDebugValue("onGround ", onGround ? "true" : "false");
//}

void Player::mouseMoved(glm::ivec2 mposition)
{
	Window& win = Window::GetInstance();
	const glm::ivec2 winSize = win.GetSize();
	const glm::ivec2 delta = mposition - (winSize / 2);
	rotation.x += delta.x * mouseSensitivity;
	rotation.y -= delta.y * mouseSensitivity;
	if (rotation.y > 90.0f)
		rotation.y = 90;
	else if (rotation.y < -90)
		rotation.y = -90;

	if (rotation.x < 0)
		rotation.x += 360;
	else if (rotation.x > 360)
		rotation.x -= 360;

	highlightUpdate();

	win.SetMousePos(winSize.x / 2, winSize.y / 2);
}

void Player::Update(float deltaTime, World& world)
{
	Window& win = Window::GetInstance();

	glm::vec3 move = glm::vec3(sin(Math::ToRadian(rotation.x)), 0, -cos(Math::ToRadian(rotation.x)));
	move = glm::normalize(move) * speed;


	static bool qpressed = false;
	if (!qpressed && win.GetKey('Q'))
	{
		glm::ivec3 chunkPos = glm::floor((glm::vec3)m_Rigidbody.GetPosition() / (glm::vec3)ChunkBlockData::ChunkSize);

		Chunk* chunk = m_World.GetChunk(chunkPos);
		chunk->AddDroppedItem(std::move(std::make_unique<DroppedItem>(m_World, m_Rigidbody.GetPosition())));
		qpressed = true;
	}
	else if (qpressed && !win.GetKey('Q'))
		qpressed = false;

	if (win.GetKey('W'))
	{
		/*position.x += sin(Math::ToRadian(rotation.x)) * cos(Math::ToRadian(rotation.y)) * speed;
		position.z += -cos(Math::ToRadian(rotation.x)) * cos(Math::ToRadian(rotation.y)) * speed;
		position.y += sin(Math::ToRadian(rotation.y)) * speed;*/
		
		//position += glm::vec3(move);
		m_Rigidbody.ChangeVelocity(glm::vec3(move) * deltaTime * (m_Rigidbody.IsOnGround() ? 1.0f : 0.2f));
	}
	if (win.GetKey('A'))
	{
		//position.x += sin(Math::ToRadian((double)rotation.x - 90.0f)) * speed;
		//position.z += -cos(Math::ToRadian((double)rotation.x - 90.0f)) * speed;
		glm::vec3 dir = glm::vec3(-cos(Math::ToRadian((double)rotation.x)), 0, -sin(Math::ToRadian((double)rotation.x)));
		m_Rigidbody.ChangeVelocity(glm::normalize(dir) * speed * deltaTime * (m_Rigidbody.IsOnGround() ? 1.0f : 0.2f));
	}
	if (win.GetKey('S'))
	{
		/*position.x += sin(Math::ToRadian((double)rotation.x + 180.0f)) * cos(Math::ToRadian(rotation.y)) * speed;
		position.z += -cos(Math::ToRadian((double)rotation.x + 180.0f)) * cos(Math::ToRadian(rotation.y)) * speed;
		position.y += -sin(Math::ToRadian(rotation.y)) * speed;*/

		m_Rigidbody.ChangeVelocity(-glm::vec3(move) * deltaTime * (m_Rigidbody.IsOnGround() ? 1.0f : 0.2f));
	}
	if (win.GetKey('D'))
	{
		/*position.x += sin(Math::ToRadian((double)rotation.x + 90.0f)) * speed;
		position.z += -cos(Math::ToRadian((double)rotation.x + 90.0f)) * speed;*/

		glm::vec3 dir = glm::vec3(cos(Math::ToRadian((double)rotation.x)), 0, sin(Math::ToRadian((double)rotation.x)));
		m_Rigidbody.ChangeVelocity(glm::normalize(dir) * speed * deltaTime * (m_Rigidbody.IsOnGround() ? 1.0f : 0.2f));
	}
	if (win.GetKey(VK_SHIFT))
	{
		//position.y += -1 * speed;
	}
	if (win.GetKey(VK_SPACE))
	{
		//position.y += 1 * speed;
		if (m_Rigidbody.IsOnGround())
			m_Rigidbody.ChangeVelocity({ 0, 1.7f, 0 });
	}

	m_Rigidbody.Update(deltaTime);

	DebugBox::GetInstance().ChangeDebugValue("hor", std::to_string(rotation.x));
	DebugBox::GetInstance().ChangeDebugValue("ver", std::to_string(rotation.y));

	DebugBox::GetInstance().ChangeDebugValue("DT", std::to_string(deltaTime));


	world.SetWorldCenter(world.ToGlobalFromWorldPosition(m_Rigidbody.GetPosition()));
	const glm::vec3& position = m_Rigidbody.GetPosition() + glm::vec3(0, 0.5f, 0);
	DebugBox::GetInstance().ChangeDebugValue("position", std::to_string(position.x) + " " + std::to_string(position.y) + " " + std::to_string(position.z));

	eyeCamera->SetPosition(position);
	eyeCamera->SetRotation(rotation.x, rotation.y);
}

void Player::SetEyeCamera(Camera3D* camera)
{
	eyeCamera = camera;
}

void Player::OnEvent(const Event& e)
{
	Window& win = Window::GetInstance();

	switch (e.type)
	{
	case Event::EventType::MouseMoved:
		mouseMoved(e.params.mouse.position);
		break;
	case Event::EventType::MouseClicked:
		if (e.params.mouseButton.button == Event::MouseButton::LMB)
			placeBlock();
		else if (e.params.mouseButton.button == Event::MouseButton::RMB)
			destroyBlock();
		break;
	}
}

void Player::Render(Camera3D& camera)
{

}