#pragma once

#include <glm/glm.hpp>

#include <graphics/Camera3D.hpp>
#include <world/Block.hpp>
#include <world/World.hpp>
#include <ui/DebugBox.hpp>
#include <util/Math.hpp>
#include <physics/Rigidbody.hpp>
#include <physics/BlockRay.hpp>

class Player
{
private:
	struct
	{
		bool looking = false;
		Block block;
		glm::ivec3 blockInFront; //Basically: position to place new blocks
		glm::ivec3 position = glm::ivec3(0);
	} lookatBlock;

	const float mouseSensitivity = 0.1f;
	const float speed = 0.8f;
	const float workingDistance = 5.0f;

	glm::ivec3 highlightedBlock = glm::ivec3(0);
	bool anythingHighlighted = false;

	Rigidbody m_Rigidbody;

	glm::vec2 rotation;
	Camera3D* eyeCamera = nullptr;

	World& m_World;

	void destroyBlock();
	void placeBlock();
	void highlightUpdate();

	void mouseMoved(glm::ivec2 position);
public:
	Player(World& world);

	void Update(float deltaTime, World &world);
	void OnEvent(const Event& e);
	void SetEyeCamera(Camera3D* camera);
	inline Camera3D* GetEyeCamera()
	{
		return eyeCamera;
	}

	void Render(Camera3D& camera);
};