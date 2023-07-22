#include <world/LoadedChunks.hpp>

LoadedChunks::LoadedChunk::LoadedChunk(std::shared_ptr<Chunk> data, std::shared_ptr<ChunkRenderer> renderer) :
	CData(data),
	CRenderer(renderer)
{

}

LoadedChunks::LoadedChunk::LoadedChunk(LoadedChunk&& other) noexcept :
	CData(std::move(other.CData)),
	CRenderer(std::move(other.CRenderer))
{

}

LoadedChunks::LoadedChunk& LoadedChunks::LoadedChunk::operator=(LoadedChunk&& other) noexcept
{
	if (this == &other)
		return *this;

	CData = std::move(other.CData);
	CRenderer = std::move(other.CRenderer);

	return *this;
}

LoadedChunks::UpdateRecord::UpdateRecord(std::weak_ptr<Chunk> data, std::weak_ptr<ChunkRenderer> renderer) :
	CData(data),
	CRenderer(renderer)
{

}

LoadedChunks::UpdateRecord::UpdateRecord(UpdateRecord&& other) noexcept :
	CData(std::move(other.CData)),
	CRenderer(std::move(other.CRenderer))
{

}

LoadedChunks::UpdateRecord& LoadedChunks::UpdateRecord::operator=(UpdateRecord&& other) noexcept
{
	if (this == &other)
		return *this;

	CData = std::move(other.CData);
	CRenderer = std::move(other.CRenderer);

	return *this;
}

LoadedChunks::LoadedChunks(Renderer3D& renderer, const std::string& savePath) :
	m_Renderer(renderer),
	m_FileLoader(savePath),
	m_ChunkGenerator(*this),
	m_UpdateThreadDone(true),
	m_ManagementThreadDone(true)
{
	assert(c_LoadedSize.x % 2 == 1);
	assert(c_LoadedSize.y % 2 == 1);
	assert(c_LoadedSize.z % 2 == 1);

	setupThreads();
	loadChunks();
}

LoadedChunks::~LoadedChunks()
{
	unloadChunks();
	destroyThreads();
}

bool LoadedChunks::IsLoaded(WorldPos position)
{
	const LoadedChunk* const chunk = getLoadedChunk(Chunk::GetChunkPosition(position));
	if (chunk == nullptr)
		return false;
	
	return chunk->CData->IsGenerated();
}

Block LoadedChunks::GetBlock(WorldPos position) const
{
	const LoadedChunk* const chunk = getLoadedChunk(Chunk::GetChunkPosition(position));
	if (chunk == nullptr)
		return Block::Air;

	return chunk->CData->GetBlock(Chunk::GetInChunkPosition(position));
}

bool LoadedChunks::PlaceBlock(WorldPos position, Block block, bool force)
{
	Chunk* const chunk = getChunk(Chunk::GetChunkPosition(position), true);
	if (chunk == nullptr)
		return false;

	if (force)
	{
		chunk->SetBlock(Chunk::GetInChunkPosition(position), block);
		return true;
	}
	else
	{
		const InChunkPos inChunkPosition = Chunk::GetInChunkPosition(position);
		if (chunk->GetBlock(inChunkPosition).Type == Block::Air)
		{
			chunk->SetBlock(inChunkPosition, block);
			return true;
		}
		return false;
	}
}

void LoadedChunks::DestroyBlock(WorldPos position)
{
	Chunk* const chunk = getChunk(Chunk::GetChunkPosition(position));
	if (chunk == nullptr)
		return;

	chunk->SetBlock(Chunk::GetInChunkPosition(position), Block::Air);
}

void LoadedChunks::Update()
{
	freeUnusedRenderers();

	std::deque<UpdateRecord> newQueue;
	forEveryChunk([&newQueue](LoadedChunk& chunk, const glm::ivec3& position)
		{
			chunk.CData->Update();
			if (chunk.CData->DoesNeedGeometryUpdate())
				newQueue.push_back({ chunk.CData, chunk.CRenderer });
		});

	std::lock_guard<std::mutex> lock(m_UpdateQueueMutex);
	m_UpdateQueue.swap(newQueue);
	if (newQueue.empty()) //If newQueue is empty, so was the UpdateQueue
		m_UpdateCondition.notify_one();
}

void LoadedChunks::SetHighlight(WorldPos position)
{
	m_ChunkWithHighlight = getLoadedChunk(Chunk::GetChunkPosition(position));
	if (m_ChunkWithHighlight == nullptr)
		return;

	m_ChunkWithHighlight->CRenderer->SetHighlight(position);
}

void LoadedChunks::ResetHighlight()
{
	if (m_ChunkWithHighlight == nullptr)
		return;

	m_ChunkWithHighlight->CRenderer->ResetHighlight();
	m_ChunkWithHighlight = nullptr;
}

void LoadedChunks::SetCenter(WorldPos position)
{
	const ChunkPos newCenter = Chunk::GetChunkPosition(position);
	if (newCenter == m_CenterChunkPos)
		return;
	reloadChunks(newCenter);
}

LoadedChunks::ManagementTask::ManagementTask(ManagementTask&& other) noexcept : 
	ToLoad(std::move(other.ToLoad)), 
	ToSave(std::move(other.ToSave)), 
	Type(other.Type) {}

LoadedChunks::ManagementTask& LoadedChunks::ManagementTask::operator=(ManagementTask&& other) noexcept
{ 
	if (this == &other) 
		return *this;
	ToLoad = std::move(other.ToLoad);
	ToSave = std::move(other.ToSave);
	Type = other.Type;
	return *this;
}

const LoadedChunks::LoadedChunk* LoadedChunks::getLoadedChunk(ChunkPos position) const
{
	glm::ivec3 arrayIndex = getArrayIndex(position);
	if (arrayIndexInBounds(arrayIndex))
		return &m_LoadedChunks[arrayIndex.x][arrayIndex.y][arrayIndex.z];

	return nullptr;
}

LoadedChunks::LoadedChunk* LoadedChunks::getLoadedChunk(ChunkPos position)
{
	glm::ivec3 arrayIndex = getArrayIndex(position);
	if (arrayIndexInBounds(arrayIndex))
		return &m_LoadedChunks[arrayIndex.x][arrayIndex.y][arrayIndex.z];

	return nullptr;
}

Chunk* LoadedChunks::getChunk(ChunkPos position, bool force)
{
	glm::ivec3 arrayIndex = getArrayIndex(position);
	if (arrayIndexInBounds(arrayIndex))
		return m_LoadedChunks[arrayIndex.x][arrayIndex.y][arrayIndex.z].CData.get();

	if (!force)
		return nullptr;

	if (m_SubsidiaryChunks.find(position) != m_SubsidiaryChunks.end())
		return m_SubsidiaryChunks.at(position).get();

	//It not exits, it should be created
	m_SubsidiaryChunks[position] = std::make_unique<Chunk>(m_Renderer, position);

	if (m_FileLoader.IsPresent(position))
	{
		m_FileLoader.LoadChunk(*m_SubsidiaryChunks[position]);
		return m_SubsidiaryChunks.at(position).get();
	}
	return m_SubsidiaryChunks[position].get();
}

void LoadedChunks::setupThreads()
{
	if (m_UpdateThreadDone)
	{
		m_UpdateThreadDone = false;
		m_UpdateThread = std::thread(&LoadedChunks::updateThreadLoop, this);
	}

	if (m_ManagementThreadDone)
	{
		m_ManagementThreadDone = false;
		m_ManagementThread = std::thread(&LoadedChunks::managementThreadLoop, this);
	}
}

void LoadedChunks::destroyThreads()
{
	if (m_UpdateThread.joinable())
	{
		m_UpdateThreadDone = true;
		m_UpdateCondition.notify_one(); //In case its waiting

		m_UpdateThread.join();
	}
	if (m_ManagementThread.joinable())
	{
		m_ManagementThreadDone = true;
		m_ManagementQueueEmpty.notify_one(); //In case its waiting

		m_ManagementThread.join();
	}
}

void LoadedChunks::reloadChunks(ChunkPos newCenter)
{
	std::lock_guard<std::mutex> uLock(m_UpdateQueueMutex);
	std::lock_guard<std::mutex> mLock(m_ManagementMutex);
	m_UpdateQueue.clear();

	LoadedChunk newLoadedChunks[c_LoadedSize.x][c_LoadedSize.y][c_LoadedSize.z];

	forEveryChunk(newLoadedChunks, [&](LoadedChunk& chunk, const glm::ivec3& position)
		{
			glm::ivec3 chunkPositionInOldArray = position + newCenter - m_CenterChunkPos;
			if (arrayIndexInBounds(chunkPositionInOldArray))
			{
				LoadedChunk& oldChunk = m_LoadedChunks[chunkPositionInOldArray.x][chunkPositionInOldArray.y][chunkPositionInOldArray.z];
				if (&oldChunk == m_ChunkWithHighlight)
					ResetHighlight();

				chunk = std::move(oldChunk);
			}
			else
			{
				const ChunkPos newChunkPos = position + newCenter - ((glm::ivec3)c_LoadedSize - glm::ivec3(1, 1, 1)) / 2;
				chunk = std::move(loadChunk(newChunkPos));
			}
		}); 

	forEveryChunk([&](LoadedChunk& chunk, const::glm::ivec3& position) 
		{
			if (chunk.CData)
				unloadChunk(std::move(chunk));
			chunk = std::move(newLoadedChunks[position.x][position.y][position.z]);
		});

	m_CenterChunkPos = newCenter;

	forEveryChunk([&](LoadedChunk& chunk, const glm::ivec3& position)
		{
			updateNeighbors(chunk.CData.get());
		});

	addFlushTask(); //To flush saving all of the unloaded chunks
}

void LoadedChunks::loadChunks()
{
	std::lock_guard<std::mutex> lock(m_ManagementMutex);

	forEveryChunk([this](LoadedChunk& chunk, const glm::ivec3& position)
		{
			chunk = std::move(loadChunk(position + m_CenterChunkPos - (c_LoadedSize - glm::ivec3(1, 1, 1)) / 2));
		});

	forEveryChunk([this](LoadedChunk& chunk, const glm::ivec3& position)
		{
			updateNeighbors(chunk.CData.get());
		});
}

void LoadedChunks::unloadChunks()
{
	{
		std::lock_guard<std::mutex> uLock(m_UpdateQueueMutex);
		m_UpdateQueue.clear();
	}

	forEveryChunk([this](LoadedChunk& chunk, const glm::ivec3& position)
		{
			unloadChunk(std::move(chunk));
		});
	
	addFlushTask();
}

void LoadedChunks::updateNeighbors(Chunk* chunk)
{
	if (chunk == nullptr)
		return;

	ChunkPos chunkPos = chunk->GetPosition();
	std::array<Chunk*, 6> neighbors = { nullptr };
	for (int side = 0; side < 6; ++side)
		neighbors[side] = getChunk(chunkPos + (ChunkPos)GetDirectionVector((Direction)side));

	chunk->UpdateNeighbors(neighbors);
}

LoadedChunks::LoadedChunk LoadedChunks::loadChunk(ChunkPos position)
{
	if (m_SubsidiaryChunks.find(position) != m_SubsidiaryChunks.end())
	{
		std::shared_ptr<Chunk> chunk = std::move(m_SubsidiaryChunks.at(position));
		if (!chunk->IsGenerated())
			addLoadTask(chunk);

		m_SubsidiaryChunks.erase(position);
		return { std::move(chunk), std::make_shared<ChunkRenderer>(m_Renderer, std::const_pointer_cast<const Chunk>(chunk)) };
	}

	std::shared_ptr<Chunk> newChunk = std::make_shared<Chunk>(m_Renderer, position);
	addLoadTask(newChunk);
	return { std::move(newChunk), std::make_shared<ChunkRenderer>(m_Renderer,  std::const_pointer_cast<const Chunk>(newChunk)) };
}

void LoadedChunks::unloadChunk(LoadedChunk&& chunk)
{
	addSaveTask(std::move(chunk.CData));
}

void LoadedChunks::freeUnusedRenderers()
{
	std::lock_guard<std::mutex> freeingLock(m_FreeingMutex);
	m_ChunkRenderersToFree.clear();
}

void LoadedChunks::updateThreadLoop()
{
	while (!m_UpdateThreadDone)
	{
		//TODO: Change to unique_lock
		m_UpdateQueueMutex.lock();
		if (m_UpdateQueue.empty())
		{
			m_UpdateQueueMutex.unlock();

			std::unique_lock<std::mutex> lock(m_UpdateConditionMutex);
			m_UpdateCondition.wait(lock);
			continue;
		}

		std::shared_ptr<Chunk> chunk = m_UpdateQueue.front().CData.lock();
		std::shared_ptr<ChunkRenderer> chunkRenderer = m_UpdateQueue.front().CRenderer.lock();
		m_UpdateQueue.pop_front();
		m_UpdateQueueMutex.unlock();

		if (!chunkRenderer ||
			!chunk) //if it was deleted in meantime
			continue;

		if (!chunk->DoesNeedGeometryUpdate())
		{
			std::lock_guard<std::mutex> freeingLock(m_FreeingMutex);
			m_ChunkRenderersToFree.push_back(chunkRenderer);
			continue;
		}

		chunkRenderer->UpdateGeometry();
		chunk->GeometryUpdated();

		std::lock_guard<std::mutex> freeingLock(m_FreeingMutex);
		m_ChunkRenderersToFree.push_back(chunkRenderer);
	}
}

void LoadedChunks::managementThreadLoop()
{
	while (true)
	{
		//TODO: Change to unique_lock
		m_ManagementQueueMutex.lock();
		if (m_ManagementTaskQueue.empty())
		{
			m_ManagementQueueMutex.unlock();

			if (m_ManagementThreadDone)
				break;

			std::unique_lock<std::mutex> lock(m_ManagementConditionMutex);
			m_ManagementQueueEmpty.wait(lock);
			continue;
		}
		
		ManagementTask task = std::move(m_ManagementTaskQueue.front());
		m_ManagementTaskQueue.pop_front();

		m_ManagementQueueMutex.unlock();

		std::lock_guard<std::mutex> lock(m_ManagementMutex);
		switch (task.Type)
		{
		case ManagementTask::Load:
		{
			std::shared_ptr<Chunk> chunk = task.ToLoad.lock();
			if (!chunk) //Chunk was unloaded in meantime
				continue;

			ChunkPos position = chunk->GetPosition();
			if (m_FileLoader.IsPresent(position))
				m_FileLoader.LoadChunk(*chunk);

			if (!chunk->IsGenerated())
				m_ChunkGenerator.GenerateChunk(*chunk);
			break;
		}
		case ManagementTask::Save:
			if (!task.ToSave)
				continue;
			m_FileLoader.SaveChunk(*task.ToSave);
			break;
		case ManagementTask::Flush:
			m_FileLoader.Flush();
			break;
		}
	}
}

void LoadedChunks::addLoadTask(std::weak_ptr<Chunk> chunk)
{
	std::lock_guard<std::mutex> lock(m_ManagementQueueMutex);
	if (m_ManagementTaskQueue.empty())
		m_ManagementQueueEmpty.notify_one();

	ManagementTask task(ManagementTask::Load);
	task.ToLoad = std::move(chunk);
	m_ManagementTaskQueue.push_back(std::move(task));
}

void LoadedChunks::addSaveTask(std::shared_ptr<Chunk> chunk)
{
	std::lock_guard<std::mutex> lock(m_ManagementQueueMutex);
	if (m_ManagementTaskQueue.empty())
		m_ManagementQueueEmpty.notify_one();

	ManagementTask task(ManagementTask::Save);
	task.ToSave = std::move(chunk);
	m_ManagementTaskQueue.push_back(std::move(task));
}

void LoadedChunks::addFlushTask()
{
	std::lock_guard<std::mutex> lock(m_ManagementQueueMutex);
	if (m_ManagementTaskQueue.empty())
		m_ManagementQueueEmpty.notify_one();

	ManagementTask task(ManagementTask::Flush);
	m_ManagementTaskQueue.push_back(std::move(task));
}