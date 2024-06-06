#include <game/physics/AABB.hpp>

AABB::AABB() :
	Position(0),
	Size(0)
{

}

AABB::AABB(const glm::vec3& position, const glm::vec3& centerPosition, const glm::vec3& size) :
	Position(position),
	CenterPosition(centerPosition),
	Size(size)
{

}

bool AABB::Intersects(const AABB& aabb) const
{
	const glm::vec3 mCorner1 = GetMinCorner(),
		pCorner1 = GetMaxCorner(),
		mCorner2 = aabb.GetMinCorner(),
		pCorner2 = aabb.GetMaxCorner();

	return mCorner1.x < pCorner2.x &&
		pCorner1.x > mCorner2.x &&
		mCorner1.y < pCorner2.y &&
		pCorner1.y > mCorner2.y &&
		mCorner1.z < pCorner2.z &&
		pCorner1.z > mCorner2.z;
}