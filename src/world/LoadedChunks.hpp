#pragma once

#include <world/Chunk.hpp>
#include <world/ChunkRenderer.hpp>
#include <world/ChunkGenerator.hpp>
#include <world/BlockManager.hpp>
#include <world/ChunkFileLoader.hpp>
#include <core/Renderer.hpp>

#include <deque>
#include <unordered_map>

#include <thread>
#include <mutex>
#include <chrono>

class LoadedChunks : public BlockManager
{
public:
	LoadedChunks(Renderer3D& renderer, const std::string& savePath);
	~LoadedChunks();
	LoadedChunks(const LoadedChunks&) = delete;
	LoadedChunks& operator=(const LoadedChunks&) = delete;

	bool IsLoaded(WorldPos position) override;
	Block GetBlock(WorldPos position) const override;
	bool PlaceBlock(WorldPos position, Block block, bool force = false) override;
	void DestroyBlock(WorldPos position) override;

	void Update();

	void SetHighlight(WorldPos position);
	void ResetHighlight();

	void SetCenter(WorldPos position);

private:
	struct LoadedChunk
	{
		std::shared_ptr<Chunk> CData;
		std::shared_ptr<ChunkRenderer> CRenderer;
		LoadedChunk() {};
		LoadedChunk(std::shared_ptr<Chunk> data, std::shared_ptr<ChunkRenderer> renderer);
		~LoadedChunk() = default;
		LoadedChunk(const LoadedChunk&) = default;
		LoadedChunk& operator=(const LoadedChunk&) = default;
		LoadedChunk(LoadedChunk&& other) noexcept;
		LoadedChunk& operator=(LoadedChunk&& other) noexcept;
	};

	static const int c_LoadingRadius = 5;
	static constexpr glm::ivec3 c_LoadedSize = { c_LoadingRadius * 2 + 1, 3, c_LoadingRadius * 2 + 1 };

	LoadedChunk m_LoadedChunks[c_LoadedSize.x][c_LoadedSize.y][c_LoadedSize.z];
	ChunkPos m_CenterChunkPos;

	std::mutex m_FreeingMutex;
	std::vector<std::shared_ptr<ChunkRenderer>> m_ChunkRenderersToFree;

	std::unordered_map<ChunkPos, std::unique_ptr<Chunk>> m_SubsidiaryChunks;

	struct UpdateRecord
	{
		std::weak_ptr<Chunk> CData;
		std::weak_ptr<ChunkRenderer> CRenderer;
		
		UpdateRecord(std::weak_ptr<Chunk> data, std::weak_ptr<ChunkRenderer> renderer);
		UpdateRecord(UpdateRecord&& other) noexcept;
		UpdateRecord& operator=(UpdateRecord&& other) noexcept;
	};

	std::deque<UpdateRecord> m_UpdateQueue;
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
		m_ManagementConditionMutex,
		m_ManagementMutex;
	std::atomic<bool> m_ManagementThreadDone;

	Renderer3D& m_Renderer;
	ChunkFileLoader m_FileLoader;
	ChunkGenerator m_ChunkGenerator;

	LoadedChunk* m_ChunkWithHighlight;

	inline glm::ivec3 getArrayIndex(ChunkPos position) const;
	inline bool arrayIndexInBounds(glm::ivec3 index) const;
	const LoadedChunk* getLoadedChunk(ChunkPos position) const; 
	LoadedChunk* getLoadedChunk(ChunkPos position);
	Chunk* getChunk(ChunkPos position, bool force = false);

	void setupThreads();
	void destroyThreads();

	void reloadChunks(ChunkPos newCenter);
	void loadChunks();
	void unloadChunks();

	void updateNeighbors(Chunk* chunk);
	LoadedChunk loadChunk(ChunkPos position);
	void unloadChunk(LoadedChunk&& chunk);

	void freeUnusedRenderers();

	void updateThreadLoop();
	void managementThreadLoop();

	void addLoadTask(std::weak_ptr<Chunk> chunk);
	void addSaveTask(std::shared_ptr<Chunk> chunk);
	void addFlushTask();

	template <class Operation>
	inline void forEveryChunk(Operation op);

	template <class Operation>
	void forEveryChunk(LoadedChunk (&array)[c_LoadedSize.x][c_LoadedSize.y][c_LoadedSize.z], Operation op) const;
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
void LoadedChunks::forEveryChunk(LoadedChunk(&array)[c_LoadedSize.x][c_LoadedSize.y][c_LoadedSize.z], Operation op) const
{
	for (int x = 0; x < c_LoadedSize.x; x++)
		for (int y = 0; y < c_LoadedSize.y; y++)
			for (int z = 0; z < c_LoadedSize.z; z++)
				op(array[x][y][z], { x, y, z });
}