#include <game/core/Player.hpp>

#include <game/items/FoodItem.hpp>
#include <game/items/BlockItem.hpp>

using window::Keyboard;
using window::KeyboardEvent;
using window::Mouse;

Player::Player(BlockManager& world, Keyboard& keyboard, Mouse& mouse, glm::ivec2 windowSize, DroppedItemRepository& droppedItemRepository) :
	m_Rigidbody(world, AABB(glm::vec3(0), c_BodyCenter, c_BodySize)),
	m_World(world),
	m_Keyboard(keyboard),
	m_Flying(false),
	m_WindowSize(windowSize),
	m_Mouse(mouse),
	m_DroppedItemRepository(droppedItemRepository)
{
	setFlying(false);
	m_Keyboard.AddKeyboardListener(*this);

	m_Inventory.GetItemStack(2).Set(FoodItem(FoodItem::FoodType::Apple), 20);
	m_Inventory.GetItemStack(3).Set(FoodItem(FoodItem::FoodType::Apple), 10);
	m_Inventory.GetItemStack(5).Set(FoodItem(FoodItem::FoodType::Bread), 3);
	m_Inventory.GetItemStack(6).Set(BlockItem(Block::Wood), 15);
}

Player::~Player() {
	m_Keyboard.RemoveKeyboardListener(*this);
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
	pickupItemsNearby();
	
	for (auto it = m_RecentlyDroppedItems.begin(); it != m_RecentlyDroppedItems.end();) {
		if (getTimestamp() - it->second >= c_PickupCooldown)
			it = m_RecentlyDroppedItems.erase(it);
		else
			++it;
	}
}

void Player::SetPosition(const glm::vec3& position) 
{ 
	m_Rigidbody.SetPosition(position); 
	m_Rigidbody.SetVelocity(glm::vec3(0)); 
}

void Player::OnKeyboardEvent(const KeyboardEvent& event) {
	if(event.EventType == KeyboardEvent::KeyPressed &&
		event.KeyCode.IsChar())
		KeyPressed(event.KeyCode.GetCharValue());
}

void Player::KeyPressed(char key)
{
	if (key == 'F')
		setFlying(!m_Flying);
	else if (key == 'Q')
		dropItem(m_Inventory.GetSelectedItemIndex());
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
		m_Inventory.GetSelectedItem().Use(*this, m_World);
	}
}

glm::ivec3 Player::GetPointingAt() const {
	return m_Highlight.HighlightedPos;
}

bool Player::IsPointingAtAnything() const {
	return m_Highlight.AnythingHighlighted;
}

void Player::ChangeHealth(float healthChange) {
	m_Health += healthChange;
	m_Health = std::min(std::max(m_Health, 1.f), 0.f);
}

WorldPos Player::GetLookingAt() const {
	return GetPointingAt();
}

bool Player::GetPlacingAt(WorldPos& position) const {
	return getPlacePosition(position);
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

bool Player::getPlacePosition(WorldPos& position) const {
	if (m_Camera == nullptr)
		return false;

	BlockRay ray(m_World, m_Rigidbody.GetPosition(), m_Camera->GetDirection());
	return ray.RaycastAhead(c_WorkingDistance, position);
}

void Player::pickupItemsNearby() {
	std::vector<DroppedItem*> itemsNearby = m_DroppedItemRepository.FindsItemNearby(m_Rigidbody.GetPosition(), c_PickupDistance);
	for (auto item : itemsNearby) {
		if (wasItemDroppedRecently(item))
			continue;

		if (m_Inventory.AddItem(item->GetItemStack())) //Successfully transfered items to inventory
			m_DroppedItemRepository.RemoveDroppedItem(item);
	}
}

void Player::dropItem(int index) {
	ItemStack& source = m_Inventory.GetItemStack(index);
	if (source.Empty())
		return;

	ItemStack droppedStack;
	m_Inventory.GetItemStack(index).MoveTo(droppedStack, 1);

	std::shared_ptr<DroppedItem> itemDropped = m_DroppedItemRepository.AddDroppedItem(std::move(droppedStack), 
		m_Rigidbody.GetPosition(), 
		c_DroppedItemSpeed * m_Camera->GetDirection()).lock();

	m_RecentlyDroppedItems.push_back({ std::move(itemDropped), getTimestamp()});
}

bool Player::wasItemDroppedRecently(DroppedItem* item) {
	for (auto recentlyDroppedItem : m_RecentlyDroppedItems)
		if (item == recentlyDroppedItem.first.get())
			return true;

	return false;
}

long long Player::getTimestamp() const {
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}