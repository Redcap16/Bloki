#pragma once

#include <glm/glm.hpp>

class Transform
{
public:
	inline Transform(glm::vec3 position, glm::vec3 rotation);
	
	inline const glm::vec3& GetPosition() const;
	inline void SetPosition(const glm::vec3& pos);
	inline const glm::vec3& GetRotation() const;
	inline void SetRotation(const glm::vec3& rot);

private:
	glm::vec3 m_Position,
		m_Rotation;
};

Transform::Transform(glm::vec3 position, glm::vec3 rotation) :
	m_Position(position),
	m_Rotation(rotation)
{

}

const glm::vec3& Transform::GetPosition() const
{
	return m_Position;
}

void Transform::SetPosition(const glm::vec3& pos)
{
	m_Position = pos;
}

const glm::vec3& Transform::GetRotation() const
{
	return m_Rotation;
}

void Transform::SetRotation(const glm::vec3& rot)
{
	m_Rotation = rot;
}