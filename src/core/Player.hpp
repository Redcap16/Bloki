#pragma once

#include <glm/glm.hpp>

#include <graphics/Camera3D.hpp>
#include <world/BlockManager.hpp>
#include <util/Math.hpp>
#include <physics/Rigidbody.hpp>
#include <physics/BlockRay.hpp>
#include <window/Input.hpp>
#include <items/Inventory.hpp>
#include <entity/DroppedItem.hpp>

class Player : public ItemUser
{
public:
	Player(BlockManager& world, window::Keyboard& keyboard, window::Mouse& mouse, glm::ivec2 windowSize, DroppedItemRepository& droppedItemRepository);
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	void Update(float deltaTime);

	const glm::vec3& GetPosition() const { return m_Rigidbody.GetPosition(); };
	void SetPosition(const glm::vec3& position);

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
		c_PickupDistance = 1.7f;

	float m_Health = 1.0f;

	Rigidbody m_Rigidbody;
	glm::vec2 m_Rotation;
	bool m_Flying;

	glm::ivec2 m_WindowSize;

	BlockManager& m_World;
	Camera3D* m_Camera;
	Inventory m_Inventory;

	DroppedItemRepository& m_DroppedItemRepository;

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
};