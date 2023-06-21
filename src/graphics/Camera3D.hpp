#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <graphics/Camera.hpp>
#include <util/Math.hpp>

class Camera3D : public Camera
{
private:
	glm::vec3 position = glm::vec3(1.0);
	float horizontalRotation = 0,
		verticalRotation = 0;
	float aspect;

	const float fov = 45.0f,
		nearVal = 0.1f,
		farVal = 200.0f;

	glm::mat4 projectionMatrix,
		viewMatrix,
		cameraMatrix;

	void updateProjectionMatrix();
	void updateViewMatrix();
public:
	Camera3D(float aspect);

	void SetAspectRatio(float aspect);
	void Rotate(float horizontal, float vertical);
	void SetRotation(float horizontal, float vertical);
	glm::vec3 GetDirection();

	inline void Move(glm::vec3 move)
	{
		position += move;
		updateViewMatrix();
	}
	inline void SetPosition(glm::vec3 position)
	{
		this->position = position;
		updateViewMatrix();
	}
	inline glm::vec2 GetRotation()
	{
		return { horizontalRotation, verticalRotation };
	}
	inline glm::vec3 GetPosition()
	{
		return position;
	}
	inline const glm::mat4& GetCameraMatrix() const override
	{
		return cameraMatrix;
	}
};