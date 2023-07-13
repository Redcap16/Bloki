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

	void Update();
	void UpdateNeighbors(Chunk* neighbors[6]);

	inline bool DoesNeedGeometryUpdate() const { return m_GeometryUpdateNeeded; }
	void UpdateGeometry();

	void AddDroppedItem(std::unique_ptr<DroppedItem> item);
	void RemoveDroppedItem(const DroppedItem* item);
	void GetDroppedItems(std::vector<DroppedItem*>& items);
private:
	ChunkRenderer m_ChunkRenderer;
	BlockArray m_BlockArray;
	ChunkPos m_Position;

	Chunk* m_Neighbors[6];

	std::vector<std::unique_ptr<DroppedItem>> m_DroppedItems;

	InChunkPos m_HighlightedPosition;
	bool m_AnythingHighlighted;

	bool m_GeometryUpdateNeeded;
	std::mutex m_GeometryMutex;

	void checkItemsBoundaries();
	void moveItem(Chunk& destination, std::vector<std::unique_ptr<DroppedItem>>::iterator it);
};