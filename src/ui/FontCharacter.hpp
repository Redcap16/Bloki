#pragma once

#include <glm/glm.hpp>

struct FontCharacter
{
	glm::vec2 uvLeftTop,
		uvSize;
	glm::ivec2 size,
		bearing;
	unsigned int advance;

	FontCharacter(glm::vec2 uvlt, glm::vec2 uvs, glm::ivec2 size, glm::ivec2 bearing, unsigned int adv);
	FontCharacter();
};