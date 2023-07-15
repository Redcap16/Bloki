#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>

#include <glm/glm.hpp>

#include <world/BlockArray.hpp>
#include <world/ChunkRenderer.hpp>
#include <entity/DroppedItem.hpp>

typedef glm::ivec3 ChunkPos;

class Chunk
{
public:
	Chunk(Renderer3D& renderer, const ChunkPos& position);
	Chunk(const Chunk&) = delete;
	Chunk& operator=(const Chunk&) = delete;

	const ChunkPos& GetPosition() const { return m_Position; }

	void SetHighlight(InChunkPos position);
	void ResetHighlight();

	void SetBlock(InChunkPos position, Block block);
	const Block& GetBlock(InChunkPos position) const;
	void SwapBlockArray(BlockArray&& blockArray);

	void Update();
	void UpdateNeighbors(Chunk* neighbors[6]);

	inline bool DoesNeedGeometryUpdate() const { return m_GeometryUpdateNeeded; }
	void UpdateGeometry();

	void AddDroppedItem(std::unique_ptr<DroppedItem> item);
	void RemoveDroppedItem(const DroppedItem* item);
	void GetDroppedItems(std::vector<DroppedItem*>& items);

	inline static InChunkPos GetInChunkPosition(glm::ivec3 position);
	inline static ChunkPos GetChunkPosition(glm::ivec3 position);

	void Deserialize(const std::vector<char>& data);
	void Serialize(std::vector<char>& result) const;

	void SetGenerated() { m_Generated = true; }
	bool IsGenerated() const { return m_Generated; }
private:
	ChunkRenderer m_ChunkRenderer;
	BlockArray m_BlockArray;
	ChunkPos m_Position;

	Chunk* m_Neighbors[6];

	std::vector<std::unique_ptr<DroppedItem>> m_DroppedItems;

	InChunkPos m_HighlightedPosition;
	bool m_AnythingHighlighted;

	bool m_Generated;
	bool m_GeometryUpdateNeeded;
	mutable std::mutex m_GeometryMutex;

	void checkItemsBoundaries();
	void moveItem(Chunk& destination, std::vector<std::unique_ptr<DroppedItem>>::iterator it);
};

InChunkPos Chunk::GetInChunkPosition(glm::ivec3 position)
{
	return (InChunkPos)glm::mod(position, (glm::ivec3)BlockArray::ChunkSize);
}

ChunkPos Chunk::GetChunkPosition(glm::ivec3 position)
{
	return (ChunkPos)glm::floor((glm::vec3)position / (glm::vec3)BlockArray::ChunkSize);
}