#include <world/LoadedChunks.hpp>

LoadedChunks::LoadedChunks(Renderer3D& renderer, ChunkFileLoader& fileLoader, ChunkGenerator& chunkGenerator) :
	m_Renderer(renderer),
	m_FileLoader(fileLoader),
	m_ChunkGenerator(chunkGenerator)
{
	assert(c_LoadedSize.x % 2 == 1);
	assert(c_LoadedSize.y % 2 == 1);
	assert(c_LoadedSize.z % 2 == 1);

	setupThread();
	loadChunks();
}

LoadedChunks::~LoadedChunks()
{
	destroyThread();
}

Block LoadedChunks::GetBlock(glm::ivec3 position) const
{
	const Chunk* const chunk = getChunk(position);
	if (chunk == nullptr)
		return Block::Air;

	return chunk->GetBlock(getInChunkPosition(position));
}

bool LoadedChunks::PlaceBlock(glm::ivec3 position, Block block, bool force = false)
{
	Chunk* const chunk = getChunk(position, true);
	if (chunk == nullptr)
		return;

	if (force)
		chunk->SetBlock(getInChunkPosition(position), block);
	else
	{
		const InChunkPos inChunkPosition = getInChunkPosition(position);
		if (chunk->GetBlock(inChunkPosition).Type == Block::Air)
			chunk->SetBlock(inChunkPosition, block);
	}
}

void LoadedChunks::DestroyBlock(glm::ivec3 position)
{
	Chunk* const chunk = getChunk(position);
	if (chunk == nullptr)
		return;

	chunk->SetBlock(getInChunkPosition(position), Block::Air);
}

void LoadedChunks::Update()
{
	std::deque<Chunk*> newQueue;
	forEveryChunk([&newQueue](std::unique_ptr<Chunk>& chunk, const glm::ivec3& position)
		{
			chunk->Update();
			if (chunk->DoesNeedGeometryUpdate())
				newQueue.push_back(chunk.get());
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
	const ChunkPos newCenter = getChunkPosition(position);
	reloadChunks(newCenter);
	m_CenterChunkPos = newCenter;
}

const Chunk* LoadedChunks::getChunk(glm::ivec3 position) const
{
	glm::ivec3 arrayIndex = getArrayIndex(position);
	if (arrayIndexInBounds(arrayIndex))
		return m_LoadedChunks[arrayIndex.x][arrayIndex.y][arrayIndex.z].get();
}

Chunk* LoadedChunks::getChunk(glm::ivec3 position, bool force)
{
	glm::ivec3 arrayIndex = getArrayIndex(position);
	if (arrayIndexInBounds(arrayIndex))
		return m_LoadedChunks[arrayIndex.x][arrayIndex.y][arrayIndex.z].get();

	if (!force)
		return nullptr;

	ChunkPos chunkPos = getChunkPosition(position);
	if (m_SubsidiaryChunks.find(chunkPos) != m_SubsidiaryChunks.end())
		return m_SubsidiaryChunks.at(chunkPos).Data.get();

	if (m_FileLoader.IsPresent(chunkPos))
	{
		m_FileLoader.LoadChunk(chunkPos, m_SubsidiaryChunks[chunkPos]);
		return m_SubsidiaryChunks.at(chunkPos).Data.get();
	}

	//It not exits, it should be created
	m_SubsidiaryChunks[chunkPos] = { std::make_unique<Chunk>(m_Renderer, chunkPos), false };
	return m_SubsidiaryChunks[chunkPos].Data.get();
}

void LoadedChunks::setupThread()
{
	m_UpdateThreadDone = false;
	m_UpdateThread = std::make_unique<std::thread>(threadLoop);
}

void LoadedChunks::destroyThread()
{
	if (m_UpdateThread.get() == nullptr)
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

	std::unique_ptr<Chunk> newLoadedChunks[c_LoadedSize.x][c_LoadedSize.y][c_LoadedSize.z];

	forEveryChunk(newLoadedChunks, [&](std::unique_ptr<Chunk>& chunk, const glm::ivec3& position)
		{
			glm::ivec3 chunkPositionInOldArray = position + newCenter - m_CenterChunkPos;
			if (arrayIndexInBounds(chunkPositionInOldArray))
			{
				std::unique_ptr<Chunk>& oldChunk = m_LoadedChunks[chunkPositionInOldArray.x][chunkPositionInOldArray.y][chunkPositionInOldArray.z];
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

	forEveryChunk([&](std::unique_ptr<Chunk>& chunk, const::glm::ivec3& position) 
		{
			chunk = std::move(newLoadedChunks[position.x][position.y][position.z]);
		});

	forEveryChunk([&](std::unique_ptr<Chunk>& chunk, const glm::ivec3& position)
		{
			updateNeighbors(chunk.get());
		});
}

void LoadedChunks::loadChunks()
{
	forEveryChunk([this](std::unique_ptr<Chunk>& chunk, const glm::ivec3& position)
		{
			chunk = std::move(loadChunk(position));
		});

	forEveryChunk([this](std::unique_ptr<Chunk>& chunk, const glm::ivec3& position)
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

std::unique_ptr<Chunk> LoadedChunks::loadChunk(ChunkPos position)
{
	if (m_SubsidiaryChunks.find(position) != m_SubsidiaryChunks.end())
	{
		ChunkFileLoader::RawChunk& rawChunk = m_SubsidiaryChunks.at(position);
		if (!rawChunk.Finished)
			m_ChunkGenerator.GenerateChunk(position, *rawChunk.Data);

		std::unique_ptr<Chunk> chunk = std::move(rawChunk.Data);
		m_SubsidiaryChunks.erase(position);
		return std::move(chunk);
	}

	if (m_FileLoader.IsPresent(position))
	{
		ChunkFileLoader::RawChunk rawChunk;
		m_FileLoader.LoadChunk(position, rawChunk);

		if (!rawChunk.Finished)
			m_ChunkGenerator.GenerateChunk(position, *rawChunk.Data);

		return std::move(rawChunk.Data);
	}

	//It not exits, it should be created
	std::unique_ptr<Chunk> newChunk = std::make_unique<Chunk>(m_Renderer, position);
	m_ChunkGenerator.GenerateChunk(position, *newChunk);
	return std::move(newChunk);
}

void LoadedChunks::unloadChunk(std::unique_ptr<Chunk>& chunk)
{
	std::lock_guard<std::mutex> lock(m_UpdateQueueMutex);
	std::deque<Chunk*>::const_iterator it = std::find(m_UpdateQueue.begin(), m_UpdateQueue.end(), chunk.get());
	if (it != m_UpdateQueue.end())
		m_UpdateQueue.erase(it);
}

void LoadedChunks::threadLoop()
{
	while (!m_UpdateThreadDone)
	{
		m_UpdateQueueMutex.lock();
		if (m_UpdateQueue.empty())
		{
			m_UpdateQueueMutex.unlock();

			std::unique_lock<std::mutex> lock(m_ConditionMutex);
			m_UpdateCondition.wait(lock);
			continue;
		}

		Chunk* chunk = m_UpdateQueue.front();
		m_UpdateQueue.pop_front();
		m_UpdateQueueMutex.unlock();

		if (!chunk->DoesNeedGeometryUpdate())
			continue;

		chunk->UpdateGeometry();
	}
}