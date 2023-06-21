#include <graphics/Camera2D.hpp>

glm::mat4 cameraMatrix;
glm::ivec2 windowSize;

void Camera2D::calculateMatrix()
{
	cameraMatrix = glm::ortho<glm::f32>(0, windowSize.x, windowSize.y, 0, -1, 1);//, 0);
}

Camera2D::Camera2D(glm::ivec2 windowSize) :
	windowSize(windowSize)
{
	calculateMatrix();
}

void Camera2D::SetWindowSize(glm::ivec2 windowSize)
{
	this->windowSize = windowSize;
	calculateMatrix();
}