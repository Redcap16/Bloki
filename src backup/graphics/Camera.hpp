#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	virtual glm::mat4 GetCameraMatrix() = 0;
};