#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <graphics/RenderPass.hpp>
#include <graphics/Vertex.hpp>
#include <graphics/VertexBuffer.hpp>
#include <world/Block.hpp>
#include <world/ChunkMesh.hpp>
#include <world/DroppedItem.hpp>

#include <algorithm>

class Chunk
{
public:
	Chunk(const BlockSubtextures& blockSubtextures, glm::vec3 globalPosition);

	void UpdateNeighbors(Chunk* neighbors[6]);
	void UpdateMesh();
	void Render(bool transparent);

	const Block& GetBlock(glm::uvec3 position);
	void SetBlock(glm::uvec3 position, const Block& block);

	inline void AddDroppedItem(std::unique_ptr<DroppedItem>&& item);

	void Update(float deltaTime);

	void ResetHighlight();
	void SetHighlight(glm::uvec3 position);

private:
	ChunkBlockData m_BlockData;
	ChunkMesh m_ChunkMesh;

	glm::vec3 m_GlobalPosition;

	Chunk* m_Neighbors[6] = { nullptr };

	bool m_MeshChanged = false;
	bool m_BuffersChanged = false;

	glm::uvec3 m_HighlightedPos = glm::uvec3(0);
	bool m_IsHighlighted = false;

	std::vector<std::unique_ptr<DroppedItem>> m_DroppedItems;

	inline void changed();
	void updateMeshBuffers();

	void checkItemBoundaries(DroppedItem* item, int index);
	void moveItemToNeighbor(int index, Chunk* chunk);
};

void Chunk::AddDroppedItem(std::unique_ptr<DroppedItem>&& item)
{
	m_DroppedItems.push_back(std::move(item));
}

void Chunk::changed()
{
	m_MeshChanged = true;
}