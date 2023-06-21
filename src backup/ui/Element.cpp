#include <ui/Element.hpp>

void  Element::recalculateMatrix()
{
	model = glm::translate(glm::mat4(1.0f), glm::vec3((glm::vec2)position, 0));
}

Element::Element(glm::ivec2 position) :
	position(position)
{
	if (position != glm::ivec2(0))
		recalculateMatrix();
}
void  Element::SetPosition(glm::ivec2 position)
{
	this->position = position;
	recalculateMatrix();
}