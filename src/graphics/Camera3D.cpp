#include <graphics/Camera3D.hpp>

Camera3D::Camera3D(glm::ivec2 windowSize) :
	m_AspectRatio((float)windowSize.x / windowSize.y)
{
	updateProjectionMatrix();
	updateDirection();
	updateViewMatrix();
}

void Camera3D::WindowResize(glm::ivec2 windowSize)
{
	m_AspectRatio = (float)windowSize.x / windowSize.y;

	updateProjectionMatrix();
}

void Camera3D::Rotate(float horizontal, float vertical)
{
	m_HorizontalRotation += fmod(m_HorizontalRotation + horizontal, 360);
	m_VerticalRotation += vertical;

	if (m_VerticalRotation > 90)
		m_VerticalRotation = 90;
	else if (m_VerticalRotation < -90)
		m_VerticalRotation = -90;
	
	updateDirection();
	updateViewMatrix();
}

void Camera3D::SetRotation(float horizontal, float vertical)
{
	m_HorizontalRotation = fmod(horizontal, 360);
	m_VerticalRotation = vertical;

	if (m_VerticalRotation > 90)
		m_VerticalRotation = 90;
	else if (m_VerticalRotation < -90)
		m_VerticalRotation = -90;

	updateDirection();
	updateViewMatrix();
}

void Camera3D::Move(const glm::vec3& move)
{
	m_Position += move;
	updateViewMatrix();
}

void Camera3D::SetPosition(const glm::vec3& position)
{
	m_Position = position;
	updateViewMatrix();
}

void Camera3D::updateDirection()
{
	m_Direction = glm::normalize(glm::vec3(
		cos(Math::ToRadian(m_VerticalRotation)) * sin(Math::ToRadian(m_HorizontalRotation)),
		sin(Math::ToRadian(m_VerticalRotation)),
		-cos(Math::ToRadian(m_VerticalRotation)) * cos(Math::ToRadian(m_HorizontalRotation))));
}

void Camera3D::updateProjectionMatrix()
{
	m_ProjectionMatrix = glm::perspective(c_FOV, m_AspectRatio, c_NearValue, c_FarValue);
	m_CameraMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void Camera3D::updateViewMatrix()
{
	const glm::vec3 rightVec = glm::normalize(glm::cross(glm::vec3(0, 1, 0), m_Direction));
	const glm::vec3 upVec = glm::cross(m_Direction, rightVec);

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Direction, upVec);
	m_CameraMatrix = m_ProjectionMatrix * m_ViewMatrix;
}