#include <ui/FontCharacter.hpp>

FontCharacter::FontCharacter(glm::vec2 uvlt, glm::vec2 uvs, glm::ivec2 size, glm::ivec2 bearing, unsigned int adv) :
	uvLeftTop(uvlt),
	uvSize(uvs),
	size(size),
	bearing(bearing),
	advance(adv)
{
}

FontCharacter::FontCharacter() :
	FontCharacter(glm::vec2(0), glm::vec2(1), glm::ivec2(1), glm::ivec2(0), 0)
{

}