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

class LoadedChunks : BlockManager
{
public:
	LoadedChunks(Renderer3D& renderer, const std::string& savePath);
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

	std::shared_ptr<Chunk> m_LoadedChunks[c_LoadedSize.x][c_LoadedSize.y][c_LoadedSize.z] = { nullptr };
	ChunkPos m_CenterChunkPos;

	std::unordered_map<ChunkPos, std::unique_ptr<Chunk>> m_SubsidiaryChunks;

	std::deque<std::weak_ptr<Chunk>> m_UpdateQueue;
	std::mutex m_UpdateQueueMutex;
	std::thread m_UpdateThread;
	std::condition_variable m_UpdateCondition;
	std::mutex m_UpdateConditionMutex;
	std::atomic<bool> m_UpdateThreadDone;

	struct ManagementTask
	{
		enum TaskType
		{
			Load,
			Save,
			Flush
		};
		TaskType Type;
		std::weak_ptr<Chunk> ToLoad;
		std::shared_ptr<Chunk> ToSave;

		ManagementTask(TaskType type) : Type(type) {}
		ManagementTask(ManagementTask&& other) noexcept;
		ManagementTask& operator=(ManagementTask&& other) noexcept;
	};

	std::thread m_ManagementThread;
	std::deque<ManagementTask> m_ManagementTaskQueue;
	std::condition_variable m_ManagementQueueEmpty;
	std::mutex m_ManagementQueueMutex,
		m_ManagementConditionMutex;
	std::atomic<bool> m_ManagementThreadDone;

	Renderer3D& m_Renderer;
	ChunkFileLoader m_FileLoader;
	ChunkGenerator m_ChunkGenerator;

	Chunk* m_ChunkWithHighlight;

	inline glm::ivec3 getArrayIndex(glm::ivec3 position) const;
	inline bool arrayIndexInBounds(glm::ivec3 index) const;
	const Chunk* getChunk(glm::ivec3 position) const;
	Chunk* getChunk(glm::ivec3 position, bool force = false);

	void setupThreads();
	void destroyThreads();

	void reloadChunks(ChunkPos newCenter);
	void loadChunks();

	void updateNeighbors(Chunk* chunk);
	std::shared_ptr<Chunk> loadChunk(ChunkPos position);
	void unloadChunk(std::shared_ptr<Chunk>&& chunk);

	void updateThreadLoop();
	void managementThreadLoop();

	void addLoadTask(std::weak_ptr<Chunk> chunk);
	void addSaveTask(std::shared_ptr<Chunk> chunk);
	void addFlushTask();

	template <class Operation>
	inline void forEveryChunk(Operation op);

	template <class Operation>
	void forEveryChunk(std::shared_ptr<Chunk> (&array)[c_LoadedSize.x][c_LoadedSize.y][c_LoadedSize.z], Operation op) const;
};

glm::ivec3 LoadedChunks::getArrayIndex(glm::ivec3 position) const
{
	return Chunk::GetChunkPosition(position) - m_CenterChunkPos + ((glm::ivec3)c_LoadedSize - glm::ivec3(1, 1, 1)) / 2;
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
void LoadedChunks::forEveryChunk(std::shared_ptr<Chunk>(&array)[c_LoadedSize.x][c_LoadedSize.y][c_LoadedSize.z], Operation op) const
{
	for (unsigned int x = 0; x < c_LoadedSize.x; x++)
		for (unsigned int y = 0; y < c_LoadedSize.y; y++)
			for (unsigned int z = 0; z < c_LoadedSize.z; z++)
				op(array[x][y][z], { x, y, z });
}