#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <world/BlockArray.hpp>
#include <world/ChunkRenderer.hpp>
#include <entity/DroppedItem.hpp>

class Chunk
{
public:
	Chunk(Renderer3D& renderer, const glm::vec3& position);
	Chunk(const Chunk&) = delete;
	Chunk& operator=(const Chunk&) = delete;

	void SetHighlight(InChunkPos position);
	void ResetHighlight();

	void SetBlock(InChunkPos position, Block block);
	const Block& GetBlock(InChunkPos position) const;

	void Update();
	void UpdateNeighbors(Chunk* neighbors[6]);

	inline bool DoesNeedUpdate() const { return m_UpdateNeeded; }
	void UpdateGeometry();

	void AddDroppedItem(std::unique_ptr<DroppedItem> item);
	void RemoveDroppedItem(const DroppedItem* item);
	void GetDroppedItems(std::vector<DroppedItem*>& items);
private:
	ChunkRenderer m_ChunkRenderer;
	BlockArray m_BlockArray;
	glm::vec3 m_Position;

	Chunk* m_Neighbors[6];

	std::vector<std::unique_ptr<DroppedItem>> m_DroppedItems;

	InChunkPos m_HighlightedPosition;
	bool m_AnythingHighlighted;

	bool m_UpdateNeeded;

	void checkItemsBoundaries();
	void moveItem(Chunk& destination, std::vector<std::unique_ptr<DroppedItem>>::iterator it);
};


/*
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
}*/