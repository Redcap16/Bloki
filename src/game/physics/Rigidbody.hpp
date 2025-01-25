#pragma once

#include <game/physics/AABB.hpp>
#include <game/world/BlockManager.hpp>
#include <memory>

class Rigidbody
{
public:
	Rigidbody(const BlockManager& world, AABB collider);

	void Update(float deltaTime);

	inline void ChangeVelocity(glm::vec3 dVelocity) { m_Velocity += dVelocity; }
	inline void SetVelocity(glm::vec3 velocity) { m_Velocity = velocity; }
	inline glm::vec3 GetVelocity() const { return m_Velocity; }

	inline void SetPosition(glm::vec3 position) { m_Collider.Position = position; }
	inline void ChangePosition(glm::vec3 dPosition) { m_Collider.Position += dPosition; }
	inline const glm::vec3& GetPosition() const { return m_Collider.Position; }

	inline bool IsOnGround() const { return m_OnGround; }

	void EnableGravity(bool gravity) { m_GravityEnabled = gravity; }
	void EnableCollision(bool collision) { m_CollisionEnabled = collision; }
	void EnableDrag(bool drag) { m_DragEnabled = drag; }

	void Serialize(std::vector<char>& data) const;
	static std::unique_ptr<Rigidbody> Deserialize(const std::vector<char>& data, const BlockManager& world);
private:
	AABB m_Collider;
	glm::vec3 m_Velocity;
	bool m_OnGround;

	bool m_GravityEnabled,
		m_CollisionEnabled,
		m_DragEnabled;

	static constexpr float c_Drag = 0.3f,
		c_GroundFriction = 1.0f,
		c_Gravity = 0.7f,
		c_CollisionCorrection = 1.001f;

	const BlockManager& m_World;

	void checkCollisions();
}; 