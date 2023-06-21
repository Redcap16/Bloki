#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	virtual const glm::mat4& GetCameraMatrix() const = 0;
};