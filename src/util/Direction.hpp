#pragma once

#include <glm/glm.hpp>

/// <summary>
/// Where Y+ is considered top, X+ right and Z+ front
/// </summary>
enum class Direction
{
	Top,
	Bottom,
	Right,
	Left,
	Front,
	Back
};

glm::vec3 GetDirectionVector(Direction dir);