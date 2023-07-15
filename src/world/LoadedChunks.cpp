#include <world/LoadedChunks.hpp>

LoadedChunks::LoadedChunks(Renderer3D& renderer, const std::string& savePath) :
	m_Renderer(renderer),
	m_FileLoader(savePath),
	m_ChunkGenerator(*this)
{
	assert(c_LoadedSize.x % 2 == 1);
	assert(c_LoadedSize.y % 2 == 1);
	assert(c_LoadedSize.z % 2 == 1);

	setupThreads();
	loadChunks();
}

LoadedChunks::~LoadedChunks()
{
	destroyThreads();
}

Block LoadedChunks::GetBlock(glm::ivec3 position) const
{
	const Chunk* const chunk = getChunk(position);
	if (chunk == nullptr)
		return Block::Air;

	return chunk->GetBlock(Chunk::GetInChunkPosition(position));
}

bool LoadedChunks::PlaceBlock(glm::ivec3 position, Block block, bool force = false)
{
	Chunk* const chunk = getChunk(position, true);
	if (chunk == nullptr)
		return;

	if (force)
		chunk->SetBlock(Chunk::GetInChunkPosition(position), block);
	else
	{
		const InChunkPos inChunkPosition = Chunk::GetInChunkPosition(position);
		if (chunk->GetBlock(inChunkPosition).Type == Block::Air)
			chunk->SetBlock(inChunkPosition, block);
	}
}

void LoadedChunks::DestroyBlock(glm::ivec3 position)
{
	Chunk* const chunk = getChunk(position);
	if (chunk == nullptr)
		return;

	chunk->SetBlock(Chunk::GetInChunkPosition(position), Block::Air);
}

void LoadedChunks::Update()
{
	std::deque<std::weak_ptr<Chunk>> newQueue;
	forEveryChunk([&newQueue](std::shared_ptr<Chunk>& chunk, const glm::ivec3& position)
		{
			chunk->Update();
			if (chunk->DoesNeedGeometryUpdate())
				newQueue.push_back(chunk);
		});

	std::lock_guard<std::mutex> lock(m_UpdateQueueMutex);
	m_UpdateQueue.swap(newQueue);
	if (newQueue.empty()) //If newQueue is empty, so was the UpdateQueue
		m_UpdateCondition.notify_one();
}

void LoadedChunks::SetHighlight(glm::ivec3 position)
{
	m_ChunkWithHighlight = getChunk(position, false);
	if (m_ChunkWithHighlight == nullptr)
		return;

	m_ChunkWithHighlight->SetHighlight(position);
}

void LoadedChunks::ResetHighlight()
{
	if (m_ChunkWithHighlight == nullptr)
		return;

	m_ChunkWithHighlight->ResetHighlight();
	m_ChunkWithHighlight = nullptr;
}

void LoadedChunks::SetCenter(glm::ivec3 position)
{
	const ChunkPos newCenter = Chunk::GetChunkPosition(position);
	reloadChunks(newCenter);
	m_CenterChunkPos = newCenter;
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

const Chunk* LoadedChunks::getChunk(glm::ivec3 position) const
{
	glm::ivec3 arrayIndex = getArrayIndex(position);
	if (arrayIndexInBounds(arrayIndex))
		return m_LoadedChunks[arrayIndex.x][arrayIndex.y][arrayIndex.z].get();

	return nullptr;
}

Chunk* LoadedChunks::getChunk(glm::ivec3 position, bool force)
{
	glm::ivec3 arrayIndex = getArrayIndex(position);
	if (arrayIndexInBounds(arrayIndex))
		return m_LoadedChunks[arrayIndex.x][arrayIndex.y][arrayIndex.z].get();

	if (!force)
		return nullptr;

	ChunkPos chunkPos = Chunk::GetChunkPosition(position);
	if (m_SubsidiaryChunks.find(chunkPos) != m_SubsidiaryChunks.end())
		return m_SubsidiaryChunks.at(chunkPos).get();

	//It not exits, it should be created
	m_SubsidiaryChunks[chunkPos] = std::make_unique<Chunk>(m_Renderer, chunkPos);

	if (m_FileLoader.IsPresent(chunkPos))
	{
		m_FileLoader.LoadChunk(*m_SubsidiaryChunks[chunkPos]);
		return m_SubsidiaryChunks.at(chunkPos).get();
	}
	return m_SubsidiaryChunks[chunkPos].get();
}

void LoadedChunks::setupThreads()
{
	m_UpdateThreadDone = false;
	m_UpdateThread = std::make_unique<std::thread>(updateThreadLoop);
}

void LoadedChunks::destroyThreads()
{
	if (!m_UpdateThread)
		return;
	
	m_UpdateThreadDone = true;
	m_UpdateCondition.notify_one(); //In case its waiting

	m_UpdateThread->join();
	m_UpdateThread.reset();
}

void LoadedChunks::reloadChunks(ChunkPos newCenter)
{
	std::lock_guard<std::mutex> lock(m_UpdateQueueMutex);
	m_UpdateQueue.clear();

	std::shared_ptr<Chunk> newLoadedChunks[c_LoadedSize.x][c_LoadedSize.y][c_LoadedSize.z];

	forEveryChunk(newLoadedChunks, [&](std::shared_ptr<Chunk>& chunk, const glm::ivec3& position)
		{
			glm::ivec3 chunkPositionInOldArray = position + newCenter - m_CenterChunkPos;
			if (arrayIndexInBounds(chunkPositionInOldArray))
			{
				std::shared_ptr<Chunk>& oldChunk = m_LoadedChunks[chunkPositionInOldArray.x][chunkPositionInOldArray.y][chunkPositionInOldArray.z];
				if (oldChunk.get() == m_ChunkWithHighlight)
					ResetHighlight();

				chunk = std::move(oldChunk);
			}
			else
			{
				const ChunkPos newChunkPos = position + newCenter - ((glm::ivec3)c_LoadedSize - glm::ivec3(1, 1, 1)) / 2;
				chunk = std::move(loadChunk(newChunkPos));
			}
		}); 

	forEveryChunk([&](std::shared_ptr<Chunk>& chunk, const::glm::ivec3& position) 
		{
			if (chunk)
				unloadChunk(std::move(chunk));
			chunk = std::move(newLoadedChunks[position.x][position.y][position.z]);
		});

	forEveryChunk([&](std::shared_ptr<Chunk>& chunk, const glm::ivec3& position)
		{
			updateNeighbors(chunk.get());
		});

	addFlushTask(); //To flush saving all of the unloaded chunks
}

void LoadedChunks::loadChunks()
{
	forEveryChunk([this](std::shared_ptr<Chunk>& chunk, const glm::ivec3& position)
		{
			chunk = std::move(loadChunk(position));
		});

	forEveryChunk([this](std::shared_ptr<Chunk>& chunk, const glm::ivec3& position)
		{
			updateNeighbors(chunk.get());
		});
}

void LoadedChunks::updateNeighbors(Chunk* chunk)
{
	if (chunk == nullptr)
		return;

	ChunkPos chunkPos = chunk->GetPosition();
	Chunk* neighbors[6] = { nullptr };
	for (int side = 0; side < 6; ++side)
		neighbors[side] = getChunk(chunkPos + (ChunkPos)GetDirectionVector((Direction)side));

	chunk->UpdateNeighbors(neighbors);
}

std::shared_ptr<Chunk> LoadedChunks::loadChunk(ChunkPos position)
{
	if (m_SubsidiaryChunks.find(position) != m_SubsidiaryChunks.end())
	{
		std::shared_ptr<Chunk> chunk = std::move(m_SubsidiaryChunks.at(position));
		if (!chunk->IsGenerated())
			addLoadTask(chunk);

		m_SubsidiaryChunks.erase(position);
		return std::move(chunk);
	}

	std::shared_ptr<Chunk> newChunk = std::make_shared<Chunk>(m_Renderer, position);
	addLoadTask(newChunk);
	return std::move(newChunk);
}

void LoadedChunks::unloadChunk(std::shared_ptr<Chunk>&& chunk)
{
	addSaveTask(std::move(chunk));
}

void LoadedChunks::updateThreadLoop()
{
	while (!m_UpdateThreadDone)
	{
		m_UpdateQueueMutex.lock();
		if (m_UpdateQueue.empty())
		{
			m_UpdateQueueMutex.unlock();

			std::unique_lock<std::mutex> lock(m_UpdateConditionMutex);
			m_UpdateCondition.wait(lock);
			continue;
		}

		std::shared_ptr<Chunk> chunk = m_UpdateQueue.front().lock();
		m_UpdateQueue.pop_front();
		m_UpdateQueueMutex.unlock();

		if (!chunk) //if it was deleted in meantime
			continue;

		if (!chunk->DoesNeedGeometryUpdate())
			continue;

		chunk->UpdateGeometry();
	}
}

void LoadedChunks::managementThreadLoop()
{
	while (m_ManagementThreadDone)
	{
		m_ManagementQueueMutex.lock();
		if (m_ManagementTaskQueue.empty())
		{
			m_ManagementQueueMutex.unlock();
			std::unique_lock<std::mutex> lock(m_ManagementConditionMutex);
			m_ManagementQueueEmpty.wait(lock);
			continue;
		}
		
		ManagementTask task = std::move(m_ManagementTaskQueue.front());
		m_ManagementTaskQueue.pop_front();

		m_ManagementQueueMutex.unlock();

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