#include <graphics/Camera3D.hpp>

void Camera3D::updateProjectionMatrix()
{
	projectionMatrix = glm::perspective(fov, aspect, nearVal, farVal);
	cameraMatrix = projectionMatrix * viewMatrix;
}

void Camera3D::updateViewMatrix()
{
	const glm::vec3 lookVec = GetDirection();
	const glm::vec3 rightVec = glm::normalize(glm::cross(glm::vec3(0, 1, 0), lookVec));
	const glm::vec3 upVec = glm::cross(lookVec, rightVec);

	viewMatrix = glm::lookAt(position, position + lookVec, upVec);
	cameraMatrix = projectionMatrix * viewMatrix;
}

Camera3D::Camera3D(float aspect) :
	aspect(aspect)
{
	updateProjectionMatrix();
	updateViewMatrix();
}

void Camera3D::SetAspectRatio(float aspect)
{
	this->aspect = aspect;
	updateProjectionMatrix();
}

void Camera3D::Rotate(float horizontal, float vertical)
{
	horizontalRotation += horizontal;
	verticalRotation += vertical;

	while (horizontalRotation > 360)
		horizontalRotation -= 360;
	while (horizontalRotation < 0)
		horizontalRotation += 360;

	while (verticalRotation > 90)
		verticalRotation = 90;
	while (verticalRotation < -90)
		verticalRotation = -90;
	updateViewMatrix();
}

void Camera3D::SetRotation(float horizontal, float vertical)
{
	horizontalRotation = horizontal;
	verticalRotation = vertical;

	while (horizontalRotation > 360)
		horizontalRotation -= 360;
	while (horizontalRotation < 0)
		horizontalRotation += 360;

	while (verticalRotation > 90)
		verticalRotation = 90;
	while (verticalRotation < -90)
		verticalRotation = -90;
	updateViewMatrix();
}

glm::vec3 Camera3D::GetDirection()
{
	return glm::normalize(glm::vec3(
		cos(Math::ToRadian(verticalRotation)) * sin(Math::ToRadian(horizontalRotation)),
		sin(Math::ToRadian(verticalRotation)),
		-cos(Math::ToRadian(verticalRotation)) * cos(Math::ToRadian(horizontalRotation))
	));
}