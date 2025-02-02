#pragma once

#include <game/world/Chunk.hpp>
#include <game/graphics/ChunkRenderer.hpp>
#include <game/world/ChunkGenerator.hpp>
#include <game/world/BlockManager.hpp>
#include <game/save_loading/BlockDataLoader.hpp>
#include <engine/core/Renderer.hpp>
#include <engine/util/Event.hpp>

#include <deque>
#include <unordered_map>

#include <thread>
#include <mutex>
#include <chrono>

class ChunkEventListener {
public:
	virtual void ChunkLoaded(const glm::ivec3& chunkPosition) = 0;
	virtual void ChunkUnloaded(const glm::ivec3& chunkPosition) = 0;
	virtual void ChunkUpdated(const glm::ivec3& chunkPosition) = 0; //Not even used
};

class LoadedChunks : public BlockManager
{
public:
	LoadedChunks(Renderer3D& renderer, WholeSaveLoader& loader);
	~LoadedChunks();
	LoadedChunks(const LoadedChunks&) = delete;
	LoadedChunks& operator=(const LoadedChunks&) = delete;

	void AddChunkListener(ChunkEventListener* listener) { m_ChunkEvent.AddListener(listener); }
	void RemoveChunkListener(ChunkEventListener* listener) { m_ChunkEvent.RemoveListener(listener); }

	Chunk* GetChunk(ChunkPos chunkPosition) { return getLoadedChunk(chunkPosition); }

	bool IsLoaded(WorldPos position) override;
	Block GetBlock(WorldPos position) const override;
	bool PlaceBlock(WorldPos position, Block block, bool force = false) override;
	void DestroyBlock(WorldPos position) override;

	void Update();

	void SetCenter(WorldPos position);

private:
	util::Event<ChunkEventListener> m_ChunkEvent;

	static const int c_LoadingRadius = 5;
	static constexpr glm::ivec3 c_LoadedSize = { c_LoadingRadius * 2 + 1, 3, c_LoadingRadius * 2 + 1 };

	std::shared_ptr<Chunk> m_LoadedChunks[c_LoadedSize.x][c_LoadedSize.y][c_LoadedSize.z];
	ChunkPos m_CenterChunkPos;

	std::unordered_map<ChunkPos, std::unique_ptr<Chunk>> m_SubsidiaryChunks;

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
		m_ManagementConditionMutex,
		m_ManagementMutex;
	std::atomic<bool> m_ManagementThreadDone;

	Renderer3D& m_Renderer;
	WholeSaveLoader& m_FileLoader; //TODO: Other object should be responsible for flushing save
	BlockDataLoader m_BlockDataLoader;
	ChunkGenerator m_ChunkGenerator;

	inline glm::ivec3 getArrayIndex(ChunkPos position) const;
	inline bool arrayIndexInBounds(glm::ivec3 index) const;
	const Chunk* getLoadedChunk(ChunkPos position) const; 
	Chunk* getLoadedChunk(ChunkPos position);
	Chunk* getChunk(ChunkPos position, bool force = false);

	void setupThreads();
	void destroyThreads();

	void reloadChunks(ChunkPos newCenter);
	void loadChunks();
	void unloadChunks();

	void updateNeighbors(Chunk* chunk);
	std::shared_ptr<Chunk> loadChunk(ChunkPos position);
	void unloadChunk(std::shared_ptr<Chunk>&& chunk);

	void managementThreadLoop();

	void addLoadTask(std::weak_ptr<Chunk> chunk);
	void addSaveTask(std::shared_ptr<Chunk>&& chunk);
	void addFlushTask();

	template <class Operation>
	inline void forEveryChunk(Operation op);

	template <class Operation>
	void forEveryChunk(std::shared_ptr<Chunk> (&array)[c_LoadedSize.x][c_LoadedSize.y][c_LoadedSize.z], Operation op) const;
};

glm::ivec3 LoadedChunks::getArrayIndex(ChunkPos position) const
{
	return position - m_CenterChunkPos + ((glm::ivec3)c_LoadedSize - glm::ivec3(1, 1, 1)) / 2;
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
void LoadedChunks::forEveryChunk(std::shared_ptr<Chunk> (&array)[c_LoadedSize.x][c_LoadedSize.y][c_LoadedSize.z], Operation op) const
{
	for (int x = 0; x < c_LoadedSize.x; x++)
		for (int y = 0; y < c_LoadedSize.y; y++)
			for (int z = 0; z < c_LoadedSize.z; z++)
				op(array[x][y][z], { x, y, z });
}