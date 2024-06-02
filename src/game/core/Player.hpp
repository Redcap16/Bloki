#pragma once

#include <glm/glm.hpp>

#include <engine/graphics/Camera3D.hpp>
#include <game/world/BlockManager.hpp>
#include <engine/util/Math.hpp>
#include <game/physics/Rigidbody.hpp>
#include <game/physics/BlockRay.hpp>
#include <engine/window/Input.hpp>
#include <game/items/Inventory.hpp>
#include <game/entity/DroppedItem.hpp>

class Player : public ItemUser, public window::KeyboardListener
{
public:
	Player(BlockManager& world, window::Keyboard& keyboard, window::Mouse& mouse, glm::ivec2 windowSize, DroppedItemRepository& droppedItemRepository);
	~Player();
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	void Update(float deltaTime);

	const glm::vec3& GetPosition() const { return m_Rigidbody.GetPosition(); };
	void SetPosition(const glm::vec3& position);

	void OnKeyboardEvent(const window::KeyboardEvent& event) override;

	void SetWindowSize(const glm::ivec2& windowSize) { m_WindowSize = windowSize; };
	void KeyPressed(char key);
	void KeyReleased(char key);
	void MouseMoved(const glm::ivec2& position);
	void MouseClicked(const glm::ivec2& position, bool leftButton);

	void SetEyeCamera(Camera3D* camera) { m_Camera = camera; };

	Inventory& GetInventory() { return m_Inventory; }
	const Inventory& GetInventory() const { return m_Inventory; }

	glm::ivec3 GetPointingAt() const;
	bool IsPointingAtAnything() const;
	
	float GetHealth() const { return m_Health; }

	//ItemUser
	void ChangeHealth(float healthChange) override;
	WorldPos GetLookingAt() const override;
	bool GetPlacingAt(WorldPos& position) const override;

private:
	static constexpr glm::vec3 c_BodySize = {0.8f, 1.8f, 0.8f};
	static constexpr glm::vec3 c_BodyCenter = { 0, -0.8f, 0 };
	static constexpr float c_MouseSensitivity = 0.1f,
		c_MoveSpeed = 0.8f,
		c_MoveSpeedInAir = 0.2f * c_MoveSpeed,
		c_WorkingDistance = 3,
		c_JumpSpeed = 1.7f,
		c_FlyingSpeed = 0.5f,
		c_PickupDistance = 1.0f,
		c_AttractionDistance = 2.0f,
		c_AttractionSpeed = 0.1f,
		c_DroppedItemSpeed = 2.f;
	static constexpr int c_PickupCooldown = 5;

	float m_Health = 1.0f;

	Rigidbody m_Rigidbody;
	glm::vec2 m_Rotation;
	bool m_Flying;

	glm::ivec2 m_WindowSize;

	BlockManager& m_World;
	Camera3D* m_Camera;
	Inventory m_Inventory;

	DroppedItemRepository& m_DroppedItemRepository;
	std::vector<std::pair<std::shared_ptr<DroppedItem>, long long>> m_RecentlyDroppedItems;

	window::Keyboard& m_Keyboard;
	window::Mouse& m_Mouse;

	struct
	{
		bool AnythingHighlighted;
		glm::ivec3 HighlightedPos;
	} m_Highlight;

	void setFlying(bool flying);
	void updateHighlightment();
	bool getPlacePosition(WorldPos& position) const;
	void pickupItemsNearby();
	void attractItemsNearby();
	void dropItem(int index);
	bool wasItemDroppedRecently(DroppedItem* item);
	long long getTimestamp() const;
};