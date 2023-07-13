#include <util/Direction.hpp>

glm::vec3 GetDirectionVector(Direction dir)
{
	switch (dir)
	{
	case Direction::Top:
		return glm::vec3(0, 1, 0);
	case Direction::Bottom:
		return glm::vec3(0, -1, 0);
	case Direction::Right:
		return glm::vec3(1, 0, 0);
	case Direction::Left:
		return glm::vec3(-1, 0, 0);
	case Direction::Front:
		return glm::vec3(0, 0, 1);
	case Direction::Back:
		return glm::vec3(0, 0, -1);
	}
}