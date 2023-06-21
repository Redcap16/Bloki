#pragma once

#include <physics/AABB.hpp>
#include <world/BlockArray.hpp>
#include <ui/DebugBox.hpp>

class Rigidbody
{
public:
	Rigidbody(BlockArray& world, AABB collider);

	void Update(float deltaTime);

	inline void ChangeVelocity(glm::vec3 dVelocity);
	inline void SetVelocity(glm::vec3 velocity);
	inline glm::vec3 GetVelocity() const;

	inline void SetPosition(glm::vec3 position);
	inline void ChangePosition(glm::vec3 dPosition);
	inline glm::vec3 GetPosition() const;

	inline bool IsOnGround() const;
private:
	AABB m_Collider;
	glm::vec3 m_Velocity;
	bool m_OnGround;

	BlockArray& m_World;

	void checkCollisions();
}; 

void Rigidbody::ChangeVelocity(glm::vec3 dVelocity)
{
	m_Velocity += dVelocity;
}

void Rigidbody::SetVelocity(glm::vec3 velocity)
{
	m_Velocity = velocity;
}

glm::vec3 Rigidbody::GetVelocity() const
{
	return m_Velocity;
}

void Rigidbody::SetPosition(glm::vec3 position)
{
	m_Collider.Position = position;
}

void Rigidbody::ChangePosition(glm::vec3 dPosition)
{
	m_Collider.Position += dPosition;
}

glm::vec3 Rigidbody::GetPosition() const
{
	return m_Collider.Position;
}

bool Rigidbody::IsOnGround() const
{
	return m_OnGround;
}