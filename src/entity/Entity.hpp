#pragma once

#include <physics/Transform.hpp>
#include <physics/Rigidbody.hpp>

class Entity
{
public:
	Entity();
	
	void Update(float deltaTime);
	const Transform& GetTransform() const;

protected:
	Transform m_Transform;
	Rigidbody m_Rigidbody;

	void update(float deltaTime);
};