#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <engine/graphics/Camera.hpp>
#include <engine/util/Math.hpp>

class Camera3D : public Camera
{
public:
	Camera3D(glm::ivec2 windowSize);

	void WindowResize(glm::ivec2 windowSize);

	void Rotate(float horizontal, float vertical);
	void SetRotation(float horizontal, float vertical);
	void Move(const glm::vec3& move);
	void SetPosition(const glm::vec3& position);

	const glm::vec2& GetRotation() { return { m_HorizontalRotation, m_VerticalRotation }; }
	const glm::vec3& GetPosition() { return m_Position; }

	const glm::vec3& GetDirection() const { return m_Direction; }
	const glm::mat4& GetCameraMatrix() const override { return m_CameraMatrix; }
	
	const glm::vec3& GetRightVector() const { return m_Right; };
	const glm::vec3& GetUpVector() const { return m_Up; };

private:
	static constexpr float c_FOV = 45.0f,
		c_NearValue = 0.1f,
		c_FarValue = 200.0f;

	glm::vec3 m_Position = glm::vec3(0.0);
	float m_HorizontalRotation = 0,
		m_VerticalRotation = 0;
	glm::vec3 m_Direction, 
		m_Up,
		m_Right;

	float m_AspectRatio;

	glm::mat4 m_ProjectionMatrix,
		m_ViewMatrix,
		m_CameraMatrix;

	void updateDirection();
	void updateProjectionMatrix();
	void updateViewMatrix();
};

