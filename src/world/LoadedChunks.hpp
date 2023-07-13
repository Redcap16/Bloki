#pragma once

#include <world/Chunk.hpp>
#include <world/ChunkGenerator.hpp>
#include <world/BlockManager.hpp>
#include <world/ChunkFileLoader.hpp>
#include <core/Renderer.hpp>

#include <deque>
#include <unordered_map>

#include <thread>
#include <mutex>
#include <chrono>

template <>
class std::hash<ChunkPos>
{
	size_t operator()(const ChunkPos& other) const noexcept
	{
		return (other.x * 42600713) ^ (other.y * 69498817) ^ (other.z * 54492923);
	}
};

class LoadedChunks : BlockManager
{
public:
	LoadedChunks(Renderer3D& renderer, ChunkFileLoader& fileLoader, ChunkGenerator& chunkGenerator);
	~LoadedChunks();
	LoadedChunks(const LoadedChunks&) = delete;
	LoadedChunks& operator=(const LoadedChunks&) = delete;

	Block GetBlock(glm::ivec3 position) const override;
	bool PlaceBlock(glm::ivec3 position, Block block, bool force = false) override;
	void DestroyBlock(glm::ivec3 position) override;

	void Update();

	void SetHighlight(glm::ivec3 position);
	void ResetHighlight();

	void SetCenter(glm::ivec3 position);

private:
	static const int c_LoadingRadius = 5;
	static constexpr glm::uvec3 c_LoadedSize = { c_LoadingRadius * 2 + 1, 3, c_LoadingRadius * 2 + 1 };

	std::unique_ptr<Chunk> m_LoadedChunks[c_LoadedSize.x][c_LoadedSize.y][c_LoadedSize.z] = { nullptr };
	ChunkPos m_CenterChunkPos;

	std::unordered_map<ChunkPos, ChunkFileLoader::RawChunk> m_SubsidiaryChunks;

	std::deque<Chunk*> m_UpdateQueue;
	std::mutex m_UpdateQueueMutex;
	std::unique_ptr<std::thread> m_UpdateThread;
	std::condition_variable m_UpdateCondition;
	std::mutex m_ConditionMutex;
	std::atomic<bool> m_UpdateThreadDone = false;

	Renderer3D& m_Renderer;
	ChunkFileLoader& m_FileLoader;
	ChunkGenerator& m_ChunkGenerator;

	Chunk* m_ChunkWithHighlight;

	inline InChunkPos getInChunkPosition(glm::ivec3 position) const;
	inline ChunkPos getChunkPosition(glm::ivec3 position) const;
	inline glm::ivec3 getArrayIndex(glm::ivec3 position) const;
	inline bool arrayIndexInBounds(glm::ivec3 index) const;
	const Chunk* getChunk(glm::ivec3 position) const;
	Chunk* getChunk(glm::ivec3 position, bool force = false);

	void setupThread();
	void destroyThread();

	void reloadChunks(ChunkPos newCenter);
	void loadChunks();

	void updateNeighbors(Chunk* chunk);
	std::unique_ptr<Chunk> loadChunk(ChunkPos position);
	void unloadChunk(std::unique_ptr<Chunk>& chunk);

	void threadLoop();

	template <class Operation>
	inline void forEveryChunk(Operation op);

	template <class Operation>
	void forEveryChunk(std::unique_ptr<Chunk> (&array)[c_LoadedSize.x][c_LoadedSize.y][c_LoadedSize.z], Operation op) const;
};

InChunkPos LoadedChunks::getInChunkPosition(glm::ivec3 position) const
{
	return (InChunkPos)glm::mod(position, (glm::ivec3)BlockArray::ChunkSize);
}

ChunkPos LoadedChunks::getChunkPosition(glm::ivec3 position) const
{
	return (ChunkPos)glm::floor((glm::vec3)position / (glm::vec3)BlockArray::ChunkSize);
}

glm::ivec3 LoadedChunks::getArrayIndex(glm::ivec3 position) const
{
	return getChunkPosition(position) - m_CenterChunkPos + ((glm::ivec3)c_LoadedSize - glm::ivec3(1, 1, 1)) / 2;
}

bool LoadedChunks::arrayIndexInBounds(glm::ivec3 index) const
{
	return index.x >= 0 && index.x < c_LoadedSize.x &&
		index.y >= 0 && index.y < c_LoadedSize.y &&
		index.z >= 0 && index.z < c_LoadedSize.z;
}

template <class Operation>
void LoadedChunks::forEveryChunk(Operation op)
{
	forEveryChunk(m_LoadedChunks, op);
}

template <class Operation>
void LoadedChunks::forEveryChunk(std::unique_ptr<Chunk>(&array)[c_LoadedSize.x][c_LoadedSize.y][c_LoadedSize.z], Operation op) const
{
	for (unsigned int x; x < c_LoadedSize.x; x++)
		for (unsigned int y; y < c_LoadedSize.y; y++)
			for (unsigned int z; z < c_LoadedSize.z; z++)
				op(array[x][y][z], { x, y, z });
}