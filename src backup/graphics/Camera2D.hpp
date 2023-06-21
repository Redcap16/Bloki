#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <graphics/Camera.hpp>
#include <util/Math.hpp>

class Camera2D : public Camera
{
private:
	glm::mat4 cameraMatrix;
	glm::ivec2 windowSize;

	void calculateMatrix();
public:
	Camera2D(glm::ivec2 windowSize);
	void SetWindowSize(glm::ivec2 windowSize);

	inline glm::mat4 GetCameraMatrix() override
	{
		return cameraMatrix;
	}
};