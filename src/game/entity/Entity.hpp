/*#pragma once

#include <physics/Transform.hpp>
#include <physics/Rigidbody.hpp>

class Entity
{
public:
	Entity(BlockManager& blockManager, const AABB& collider);
	
	void Update(float deltaTime);
	const glm::vec3& GetPosition() const { return m_Rigidbody.GetPosition(); };

protected:
	Rigidbody m_Rigidbody;

	virtual void update(float deltaTime) = 0;
};*/

//For potencial use in future