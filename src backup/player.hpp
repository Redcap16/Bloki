#pragma once

#include <glm/glm.hpp>

#include <graphics/Camera3D.hpp>
#include <world/Block.hpp>
#include <world/World.hpp>
#include <ui/DebugBox.hpp>
#include <util/Math.hpp>

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
	const float speed = 0.05f;
	const float workingDistance = 3.0f;

	const float gravity = 0.1f;
	bool onGround = false;

	glm::vec3 velocity = glm::vec3(0);

	glm::vec3 position;
	glm::vec2 rotation;
	Camera3D* eyeCamera = nullptr;

	Player();
	void raycast();

	void destroyBlock();
	void placeBlock();

	void mouseMoved(glm::ivec2 position);

	std::pair<bool, glm::ivec3> collide();
	void checkIfOnGround();
public:
	static Player& GetInstance();

	void Update(float deltaTime);
	void OnEvent(Event& e);
	void SetEyeCamera(Camera3D* camera);
	inline Camera3D* GetEyeCamera()
	{
		return eyeCamera;
	}

	void Render(Camera3D& camera);
};