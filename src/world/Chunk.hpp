#pragma once

#include <array>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

#include <glm/glm.hpp>
#include <glm/gtx/integer.hpp>

#include <core/Renderer.hpp>
#include <world/BlockArray.hpp>
#include <entity/DroppedItem.hpp>

typedef glm::ivec3 ChunkPos;
typedef glm::ivec3 WorldPos;

class Chunk
{
public:
	template <class TBlockArray>
	class BlockAccess
	{
	public:
		BlockAccess(TBlockArray& blockArray, std::mutex& mutex) : m_BlockArray(&blockArray), m_Lock(mutex) {}
		~BlockAccess() = default;
		BlockAccess(const BlockAccess&) = delete;
		BlockAccess& operator=(const BlockAccess&) = delete;
		BlockAccess(BlockAccess&& other) noexcept;
		BlockAccess& operator=(BlockAccess&& other) noexcept;

		bool IsValid() const { return m_BlockArray != nullptr; }
		void SetBlock(InChunkPos position, Block block) { m_BlockArray->Set(position, block); }
		Block GetBlock(InChunkPos position) const { return m_BlockArray->Get(position); }
	private:
		std::unique_lock<std::mutex> m_Lock;
		TBlockArray* m_BlockArray;
	};

	Chunk(Renderer3D& renderer, const ChunkPos& position);
	Chunk(const Chunk&) = delete;
	Chunk& operator=(const Chunk&) = delete;

	const ChunkPos& GetPosition() const { return m_Position; }

	void SetBlock(InChunkPos position, Block block);
	Block GetBlock(InChunkPos position) const;
	void SwapBlockArray(BlockArray&& blockArray);
	BlockAccess<BlockArray> GetBlockAccess() { return BlockAccess<BlockArray>(m_BlockArray, m_GeometryMutex); }
	BlockAccess<const BlockArray> GetBlockAccess() const { return BlockAccess<const BlockArray>(m_BlockArray, m_GeometryMutex); }

	void Update();
	void UpdateNeighbors(const std::array<Chunk*, 6>& neighbors);
	const std::array<const Chunk*, 6>& GetNeighbors() const { return reinterpret_cast<const std::array<const Chunk*, 6>&>(m_Neighbors); }

	inline bool DoesNeedGeometryUpdate() const { return m_GeometryUpdateNeeded; }
	inline void GeometryUpdated() { m_GeometryUpdateNeeded = false; }

	void AddDroppedItem(std::unique_ptr<DroppedItem> item);
	void RemoveDroppedItem(const DroppedItem* item);
	void GetDroppedItems(std::vector<DroppedItem*>& items);

	inline static InChunkPos GetInChunkPosition(WorldPos position);
	inline static ChunkPos GetChunkPosition(WorldPos position);

	void Deserialize(const std::vector<char>& data);
	void Serialize(std::vector<char>& result) const;

	void SetGenerated() { m_Generated = true; }
	bool IsGenerated() const { return m_Generated; }
private:
	BlockArray m_BlockArray;
	ChunkPos m_Position;

	std::array<Chunk*, 6> m_Neighbors;

	std::vector<std::unique_ptr<DroppedItem>> m_DroppedItems;

	bool m_Generated;
	std::atomic<bool> m_GeometryUpdateNeeded;
	mutable std::mutex m_GeometryMutex;

	void checkItemsBoundaries();
	void moveItem(Chunk& destination, std::vector<std::unique_ptr<DroppedItem>>::iterator it);
};

template <class TBlockArray>
Chunk::BlockAccess<TBlockArray>::BlockAccess(BlockAccess&& other) noexcept :
	m_Lock(std::move(other.m_Lock)),
	m_BlockArray(other.m_BlockArray)
{
	other.m_BlockArray = nullptr;
}

template <class TBlockArray>
Chunk::BlockAccess<TBlockArray>& Chunk::BlockAccess<TBlockArray>::operator=(BlockAccess&& other) noexcept 
{ 
	m_Lock = std::move(other.m_Lock); 
	m_BlockArray = other.m_BlockArray;

	other.m_BlockArray = nullptr; 
}

InChunkPos Chunk::GetInChunkPosition(WorldPos position)
{
	return (InChunkPos)Math::Mod(position, (glm::ivec3)BlockArray::ChunkSize);
}

ChunkPos Chunk::GetChunkPosition(WorldPos position)
{
	return (ChunkPos)glm::floor((glm::vec3)position / (glm::vec3)BlockArray::ChunkSize);
}