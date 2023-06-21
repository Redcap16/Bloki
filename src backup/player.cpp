#include <player.hpp>

Player::Player() :
	position(0, 50, 0),
	rotation(0)
{
}

void Player::raycast()
{
	World& world = World::GetInstance();

	static const Block::BlockType airID = Block::GetIdByName("Air");

	glm::vec3 step = eyeCamera->GetDirection();
	step = glm::normalize(step);
	const float stepSize = 0.03f;
	step *= stepSize;

	glm::vec3 cpos = position;
	glm::ivec3 oldIntegerCPos = glm::floor(cpos) - glm::vec3(1); //To make it different

	bool hit = false;

	while (glm::distance(cpos, position) <= workingDistance)
	{
		const glm::ivec3 icpos = glm::floor(cpos);
		if (icpos != oldIntegerCPos)
		{
			Block block = world.GetBlock(icpos).first;

			if (block.type != airID)
			{
				world.SetHighlightedBlock(icpos);
				lookatBlock.looking = true;
				lookatBlock.position = icpos;
				lookatBlock.block = block;
				lookatBlock.blockInFront = oldIntegerCPos;
				hit = true;
				break;
			}

			oldIntegerCPos = icpos;
		}

		cpos += step;
	}
	if (!hit)
	{
		world.DisableHighlightedBlock();
		lookatBlock.looking = false;
	}
}

void Player::destroyBlock()
{
	static const Block::BlockType airID = Block::GetIdByName("Air");
	World& world = World::GetInstance();
	if (lookatBlock.looking)
	{
		Block b;
		b.type = airID;
		world.SetBlock(lookatBlock.position, b, true);
	}

	raycast(); //Update lookatBlock
}

void Player::placeBlock()
{
	static const Block::BlockType stoneID = Block::GetIdByName("Stone");

	World& world = World::GetInstance();
	if (lookatBlock.looking)
	{
		Block b;
		b.type = stoneID;
		world.SetBlock(lookatBlock.blockInFront, b, true);
	}

	raycast(); //Update lookatBlock
}

void Player::mouseMoved(glm::ivec2 position)
{
	Window& win = Window::GetInstance();
	const glm::ivec2 winSize = win.GetSize();
	const glm::ivec2 delta = position - (winSize / 2);
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

	raycast();

	win.SetMousePos(winSize.x / 2, winSize.y / 2);
}

std::pair<bool, glm::ivec3> Player::collide()
{
	World& world = World::GetInstance();
	const float height = 1.8f;
	static const Block::BlockType airID = Block::GetIdByName("Air");

	for (float y = 0; y < ceil(height) + 1; y++)
	{
		const glm::ivec3 blockPosition = glm::floor(position + glm::vec3(0, -y, 0));
		const auto block = world.GetBlock(blockPosition);
		if (block.second && block.first.type != airID)
			return { true, blockPosition };
	}

	return { false, glm::ivec3(0) };
}

void Player::checkIfOnGround()
{
	if (World::GetInstance().GetBlock(glm::ivec3(glm::floor(position)) - glm::ivec3(0, -2, 0)).first.type != Block::GetIdByName("Air"))
		onGround = true;
	else
		onGround = false;
}

Player& Player::GetInstance()
{
	static Player player;
	return player;
}

void Player::Update(float deltaTime)
{
	Window& win = Window::GetInstance();

	if (World::GetInstance().GetChunk(position) && !World::GetInstance().GetChunk(position)->IsLoaded())
		return;

	glm::vec4 move = glm::rotate(glm::mat4(1.0f), Math::ToRadian(rotation.y), glm::vec3(1, 0, 0)) * glm::vec4(0, 0, -1, 1);
	move = glm::rotate(glm::mat4(1.0f), Math::ToRadian(rotation.x), glm::vec3(0, -1, 0)) * move;
	move = glm::normalize(move) * speed;

	if (win.GetKey('W'))
	{
		/*position.x += sin(Math::ToRadian(rotation.x)) * cos(Math::ToRadian(rotation.y)) * speed;
		position.z += -cos(Math::ToRadian(rotation.x)) * cos(Math::ToRadian(rotation.y)) * speed;
		position.y += sin(Math::ToRadian(rotation.y)) * speed;*/
		position += glm::vec3(move);


		//glm::vec3 acc = eyeCamera->GetDirection();
		//acc.y = 0;
		//acc = glm::normalize(acc) * speed * deltaTime;
		//velocity += acc;
	}
	if (win.GetKey('A'))
	{
		position.x += sin(Math::ToRadian((double)rotation.x - 90.0f)) * speed;
		position.z += -cos(Math::ToRadian((double)rotation.x - 90.0f)) * speed;
	}
	if (win.GetKey('S'))
	{
		position.x += sin(Math::ToRadian((double)rotation.x + 180.0f)) * cos(Math::ToRadian(rotation.y)) * speed;
		position.z += -cos(Math::ToRadian((double)rotation.x + 180.0f)) * cos(Math::ToRadian(rotation.y)) * speed;
		position.y += -sin(Math::ToRadian(rotation.y)) * speed;
	}
	if (win.GetKey('D'))
	{
		position.x += sin(Math::ToRadian((double)rotation.x + 90.0f)) * speed;
		position.z += -cos(Math::ToRadian((double)rotation.x + 90.0f)) * speed;
	}
	if (win.GetKey(VK_SHIFT))
	{
		position.y += -1 * speed;
	}
	if (win.GetKey(VK_SPACE))
	{
		position.y += 1 * speed;
	}
	DebugBox::GetInstance().ChangeDebugValue("hor", std::to_string(rotation.x));
	DebugBox::GetInstance().ChangeDebugValue("ver", std::to_string(rotation.y));

	/*checkIfOnGround();
	if (!onGround)
		velocity.y -= gravity * deltaTime;
	else
		velocity.y = 0;

	velocity /= 1 + (0.1f * deltaTime);
	position += velocity * deltaTime;*/

	World::GetInstance().SetCenterChunk(position);
	DebugBox::GetInstance().ChangeDebugValue("position", std::to_string(position.x) + " " + std::to_string(position.y) + " " + std::to_string(position.z));

	/*bool colides = collide().first;
	Debug::GetInstance().ChangeDebugValue("onGnd", onGround ? "true" : "false");
	Debug::GetInstance().ChangeDebugValue("coll", colides ? "true" : "false");

	if (colides)
	{
		position -= velocity;
		velocity = glm::vec3(0);
	}
	*/
	/*static glm::vec3 velocity2 = glm::vec3(0);
	velocity2.y -= gravity;
	position += velocity2;

	colides = collide().first;
	Debug::GetInstance().ChangeDebugValue("coll", colides ? "true" : "false");

	if (colides)
	{
		position -= velocity2;
		velocity2 = glm::vec3(0);
	}*/

	eyeCamera->SetPosition(position);
	eyeCamera->SetRotation(rotation.x, rotation.y);
}

void Player::SetEyeCamera(Camera3D* camera)
{
	eyeCamera = camera;
}

void Player::OnEvent(Event& e)
{
	Window& win = Window::GetInstance();

	switch (e.type)
	{
	case Event::EventType::MouseMoved:
		mouseMoved(e.params.mouse.position);
		break;
	case Event::EventType::MouseClicked:
		if (e.params.mouseButton.button == Event::MouseButton::LMB)
			destroyBlock();
		else if (e.params.mouseButton.button == Event::MouseButton::RMB)
			placeBlock();
		break;
	}
}

void Player::Render(Camera3D& camera)
{

}