#include <world/LoadedChunks.hpp>

LoadedChunks::LoadedChunks(Renderer3D& renderer, const std::string& savePath) :
	m_Renderer(renderer),
	m_FileLoader(savePath),
	m_ChunkGenerator(*this),
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
	const Chunk* const chunk = getLoadedChunk(Chunk::GetChunkPosition(position));
	if (chunk == nullptr)
		return false;
	
	return chunk->IsGenerated();
}

Block LoadedChunks::GetBlock(WorldPos position) const
{
	const Chunk* const chunk = getLoadedChunk(Chunk::GetChunkPosition(position));
	if (chunk == nullptr)
		return Block::Air;

	return chunk->GetBlock(Chunk::GetInChunkPosition(position));
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

void LoadedChunks::Update() {
	forEveryChunk([](std::shared_ptr<Chunk>& chunk, const glm::ivec3& position) {
		chunk->Update();
	});
}

void LoadedChunks::SetCenter(WorldPos position) {
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

const Chunk* LoadedChunks::getLoadedChunk(ChunkPos position) const
{
	glm::ivec3 arrayIndex = getArrayIndex(position);
	if (arrayIndexInBounds(arrayIndex))
		return m_LoadedChunks[arrayIndex.x][arrayIndex.y][arrayIndex.z].get();

	return nullptr;
}

Chunk* LoadedChunks::getLoadedChunk(ChunkPos position)
{
	glm::ivec3 arrayIndex = getArrayIndex(position);
	if (arrayIndexInBounds(arrayIndex))
		return m_LoadedChunks[arrayIndex.x][arrayIndex.y][arrayIndex.z].get();

	return nullptr;
}

Chunk* LoadedChunks::getChunk(ChunkPos position, bool force)
{
	if (!force)
		return getLoadedChunk(position);

	if (m_SubsidiaryChunks.find(position) != m_SubsidiaryChunks.end())
		return m_SubsidiaryChunks.at(position).get();

	//It not exits, it should be created
	m_SubsidiaryChunks[position] = std::make_unique<Chunk>(position);

	if (m_FileLoader.IsPresent(position))
		m_FileLoader.LoadChunk(*m_SubsidiaryChunks[position]);

	return m_SubsidiaryChunks[position].get();
}

void LoadedChunks::setupThreads() {
	if (m_ManagementThreadDone) {
		m_ManagementThreadDone = false;
		m_ManagementThread = std::thread(&LoadedChunks::managementThreadLoop, this);
	}
}

void LoadedChunks::destroyThreads() {
	if (m_ManagementThread.joinable()) {
		m_ManagementThreadDone = true;
		m_ManagementQueueEmpty.notify_one(); //In case its waiting

		m_ManagementThread.join();
	}
}

void LoadedChunks::reloadChunks(ChunkPos newCenter)
{
	std::lock_guard<std::mutex> mLock(m_ManagementMutex);

	std::shared_ptr<Chunk> newLoadedChunks[c_LoadedSize.x][c_LoadedSize.y][c_LoadedSize.z];

	forEveryChunk(newLoadedChunks, [&](std::shared_ptr<Chunk>& chunk, const glm::ivec3& position) {
			glm::ivec3 chunkPositionInOldArray = position + newCenter - m_CenterChunkPos;
			if (arrayIndexInBounds(chunkPositionInOldArray)) {
				std::shared_ptr<Chunk>& oldChunk = m_LoadedChunks[chunkPositionInOldArray.x][chunkPositionInOldArray.y][chunkPositionInOldArray.z];
				chunk = std::move(oldChunk);
			}
			else {
				const ChunkPos newChunkPos = position + newCenter - ((glm::ivec3)c_LoadedSize - glm::ivec3(1, 1, 1)) / 2;
				chunk = std::move(loadChunk(newChunkPos));
				m_ChunkEvent.Invoke(&ChunkEventListener::ChunkLoaded, newChunkPos);
			}
		}); 

	forEveryChunk([&](std::shared_ptr<Chunk>& chunk, const::glm::ivec3& position) 
		{
			if (chunk)
				unloadChunk(std::move(chunk));
			chunk = std::move(newLoadedChunks[position.x][position.y][position.z]);
		});

	m_CenterChunkPos = newCenter;

	forEveryChunk([&](std::shared_ptr<Chunk>& chunk, const glm::ivec3& position)
		{
			updateNeighbors(chunk.get());
		});

	addFlushTask(); //To flush saving all of the unloaded chunks
}

void LoadedChunks::loadChunks()
{
	std::lock_guard<std::mutex> lock(m_ManagementMutex);

	forEveryChunk([this](std::shared_ptr<Chunk>& chunk, const glm::ivec3& position)
		{
			const glm::ivec3 worldChunkPosition = position + m_CenterChunkPos - (c_LoadedSize - glm::ivec3(1, 1, 1)) / 2;
			chunk = std::move(loadChunk(worldChunkPosition));
			m_ChunkEvent.Invoke(&ChunkEventListener::ChunkLoaded, worldChunkPosition);
		});

	forEveryChunk([this](std::shared_ptr<Chunk>& chunk, const glm::ivec3& position)
		{
			updateNeighbors(chunk.get());
		});
}

void LoadedChunks::unloadChunks()
{
	forEveryChunk([this](std::shared_ptr<Chunk>& chunk, const glm::ivec3& position) {
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

std::shared_ptr<Chunk> LoadedChunks::loadChunk(ChunkPos position) {
	if (m_SubsidiaryChunks.find(position) != m_SubsidiaryChunks.end()) {
		std::shared_ptr<Chunk> chunk = std::move(m_SubsidiaryChunks.at(position));
		if (!chunk->IsGenerated())
			addLoadTask(chunk);

		m_SubsidiaryChunks.erase(position);
		return chunk;
	}

	std::shared_ptr<Chunk> newChunk = std::make_shared<Chunk>(position);
	addLoadTask(newChunk);
	return newChunk;
}

void LoadedChunks::unloadChunk(std::shared_ptr<Chunk>&& chunk)
{
	m_ChunkEvent.Invoke(&ChunkEventListener::ChunkUnloaded, chunk->GetPosition());
	addSaveTask(std::move(chunk));
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

void LoadedChunks::addSaveTask(std::shared_ptr<Chunk>&& chunk)
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