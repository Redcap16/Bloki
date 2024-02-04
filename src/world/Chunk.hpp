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
#include <util/Event.hpp>

typedef glm::ivec3 ChunkPos;

class ChunkUpdateListener {
public:
	virtual void ChunkUpdated(const glm::ivec3& position) = 0;
};

class Chunk
{
public:
	template <class TBlockArray>
	class BlockAccess
	{
	public:
		BlockAccess(TBlockArray& blockArray, std::mutex& mutex) : 
			m_BlockArray(&blockArray), 
			m_Lock(mutex), 
			m_UpdateEvent(nullptr), 
			m_ChunkPosition(0) {}
		BlockAccess(TBlockArray& blockArray, std::mutex& mutex, util::Event<ChunkUpdateListener>* updateEvent, ChunkPos chunkPosition) : 
			m_BlockArray(&blockArray), 
			m_Lock(mutex), 
			m_UpdateEvent(updateEvent),
			m_ChunkPosition(chunkPosition) {}
		~BlockAccess() { if (m_UpdateEvent != nullptr) m_UpdateEvent->Invoke(&ChunkUpdateListener::ChunkUpdated, m_ChunkPosition); };
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

		util::Event<ChunkUpdateListener>* m_UpdateEvent;
		ChunkPos m_ChunkPosition;
	};

	Chunk(const ChunkPos& position);
	Chunk(const Chunk&) = delete;
	Chunk& operator=(const Chunk&) = delete;

	void AddUpdateListener(ChunkUpdateListener* listener) { m_UpdateEvent.AddListener(listener); }
	void RemoveUpdateListener(ChunkUpdateListener* listener) { m_UpdateEvent.RemoveListener(listener); }

	const ChunkPos& GetPosition() const { return m_Position; }

	void SetBlock(InChunkPos position, Block block);
	Block GetBlock(InChunkPos position) const;
	void SwapBlockArray(BlockArray& blockArray);
	BlockAccess<BlockArray> GetBlockAccess() { return BlockAccess<BlockArray>(m_BlockArray, m_GeometryMutex, &m_UpdateEvent, m_Position); }
	BlockAccess<const BlockArray> GetBlockAccess() const { return BlockAccess<const BlockArray>(m_BlockArray, m_GeometryMutex); }

	void Update();
	void UpdateNeighbors(const std::array<Chunk*, 6>& neighbors);
	const std::array<const Chunk*, 6>& GetNeighbors() const { return reinterpret_cast<const std::array<const Chunk*, 6>&>(m_Neighbors); }

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
	util::Event<ChunkUpdateListener> m_UpdateEvent;

	BlockArray m_BlockArray;
	ChunkPos m_Position;

	std::array<Chunk*, 6> m_Neighbors;

	std::vector<std::unique_ptr<DroppedItem>> m_DroppedItems;

	bool m_Generated;
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