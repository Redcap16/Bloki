#include <core/Player.hpp>

Player::Player(BlockManager& world, Keyboard& keyboard, Mouse& mouse, glm::ivec2 windowSize) :
	m_Rigidbody(world, AABB(glm::vec3(0), c_BodyCenter, c_BodySize)),
	m_World(world),
	m_Keyboard(keyboard),
	m_Flying(false),
	m_WindowSize(windowSize),
	m_Mouse(mouse)
{
	setFlying(false);
}

void Player::Update(float deltaTime)
{
	if (!m_Camera)
		return;

	if (!m_World.IsLoaded(m_Rigidbody.GetPosition()))
		return;

	glm::vec3 moveDirection = m_Camera->GetDirection(),
		rightDirection = glm::cross(moveDirection, glm::vec3(0, 1, 0));


	if (m_Flying)
	{
		rightDirection = glm::normalize(rightDirection);
		moveDirection = glm::normalize(moveDirection);

		moveDirection *= c_FlyingSpeed;
		rightDirection *= c_FlyingSpeed;

		m_Rigidbody.SetVelocity(glm::vec3(0));
		if (m_Keyboard.GetKeyState('W'))
			m_Rigidbody.ChangeVelocity(moveDirection);
		if (m_Keyboard.GetKeyState('S'))
			m_Rigidbody.ChangeVelocity(-moveDirection);
		if (m_Keyboard.GetKeyState('D'))
			m_Rigidbody.ChangeVelocity(rightDirection);
		if (m_Keyboard.GetKeyState('A'))
			m_Rigidbody.ChangeVelocity(-rightDirection);
		if (m_Keyboard.GetKeyState(Keyboard::Key::Spacebar))
			m_Rigidbody.ChangeVelocity(glm::vec3(0, c_FlyingSpeed, 0));
		if (m_Keyboard.GetKeyState(Keyboard::Key::Shift))
			m_Rigidbody.ChangeVelocity(glm::vec3(0, -c_FlyingSpeed, 0));
	}
	else
	{
		moveDirection.y = 0;
		moveDirection = glm::normalize(moveDirection);
		rightDirection.y = 0;
		rightDirection = glm::normalize(rightDirection);

		moveDirection *= (m_Rigidbody.IsOnGround() ? c_MoveSpeed : c_MoveSpeedInAir);
		rightDirection *= (m_Rigidbody.IsOnGround() ? c_MoveSpeed : c_MoveSpeedInAir);

		if (m_Keyboard.GetKeyState('W'))
			m_Rigidbody.ChangeVelocity(moveDirection * deltaTime);
		if (m_Keyboard.GetKeyState('S'))
			m_Rigidbody.ChangeVelocity(-moveDirection * deltaTime);
		if (m_Keyboard.GetKeyState('D'))
			m_Rigidbody.ChangeVelocity(rightDirection * deltaTime);
		if (m_Keyboard.GetKeyState('A'))
			m_Rigidbody.ChangeVelocity(-rightDirection * deltaTime);
		if (m_Keyboard.GetKeyState(Keyboard::Key::Spacebar) && m_Rigidbody.IsOnGround())
			m_Rigidbody.ChangeVelocity(glm::vec3(0, c_JumpSpeed, 0));
	}

	m_Rigidbody.Update(deltaTime);

	m_Camera->SetPosition(m_Rigidbody.GetPosition());
}

void Player::SetPosition(const glm::vec3& position) 
{ 
	m_Rigidbody.SetPosition(position); 
	m_Rigidbody.SetVelocity(glm::vec3(0)); 
}

void Player::KeyPressed(char key)
{
	if (key == 'F')
		setFlying(!m_Flying);
}

void Player::KeyReleased(char key)
{

}

void Player::MouseMoved(const glm::ivec2& position)
{
	m_Mouse.SetPosition(m_WindowSize / 2);

	glm::ivec2 delta = position - m_WindowSize / 2;
	delta.y = -delta.y;
	m_Rotation += (glm::vec2)delta * c_MouseSensitivity;

	m_Rotation.x = Math::Mod(m_Rotation.x, 360.0f);
	if (m_Rotation.y > 90) m_Rotation.y = 90;
	if (m_Rotation.y < -90) m_Rotation.y = -90;

	m_Camera->SetRotation(m_Rotation.x, m_Rotation.y);
	updateHighlightment();
}

void Player::MouseClicked(const glm::ivec2& position, bool leftButton)
{
	if (leftButton)
	{
		if (m_Highlight.AnythingHighlighted)
			m_World.DestroyBlock(m_Highlight.HighlightedPos);
	}
	else
	{
		if (m_Camera)
		{
			BlockRay ray(m_World, m_Rigidbody.GetPosition(), m_Camera->GetDirection());
			glm::ivec3 placePosition;
			if (ray.RaycastAhead(c_WorkingDistance, placePosition))
				m_World.PlaceBlock(placePosition, Block::Stone, false);
		}
	}
}

glm::ivec3 Player::GetPointingAt() const {
	return m_Highlight.HighlightedPos;
}

bool Player::IsPointingAtAnything() const {
	return m_Highlight.AnythingHighlighted;
}

void Player::setFlying(bool flying)
{
	m_Flying = flying;
	m_Rigidbody.EnableGravity(!flying);
	m_Rigidbody.EnableDrag(!flying);
	m_Rigidbody.EnableCollision(!flying);
}

void Player::updateHighlightment()
{
	if (!m_Camera)
		return;

	BlockRay ray(m_World, m_Rigidbody.GetPosition(), m_Camera->GetDirection());
	m_Highlight.AnythingHighlighted = ray.Raycast(c_WorkingDistance, m_Highlight.HighlightedPos);
}