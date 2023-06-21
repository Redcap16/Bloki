#pragma once

#include <physics/Rigidbody.hpp>
#include <graphics/VertexBuffer.hpp>
#include <graphics/Vertex.hpp>

class DroppedItem
{
public:
	DroppedItem(BlockArray& world, glm::vec3 position);

	void Update(float deltaTime);
	void Render();
	inline glm::vec3 GetPosition() const;

private:
	Rigidbody m_Rigidbody;
	VertexBuffer<Vertex3D> m_Mesh;

	void setupMesh();
	void setupFace(FaceDirection direction);
};

glm::vec3 DroppedItem::GetPosition() const
{
	return m_Rigidbody.GetPosition();
}