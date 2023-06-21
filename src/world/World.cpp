#include <world/World.hpp>

bool World::inBounds(glm::ivec3 local)
{
	if (local.x < 0 || local.x >= ChunkArray::Size.x ||
		local.y < 0 || local.y >= ChunkArray::Size.y ||
		local.z < 0 || local.z >= ChunkArray::Size.z)
		return false;
	return true;
}

void World::updateChunksEntry()
{
	stopUpdatingFlag = false;
	for (unsigned int x = 0; x < ChunkArray::Size.x; x++)
		for (unsigned int y = 0; y < ChunkArray::Size.y; y++)
			for (unsigned int z = 0; z < ChunkArray::Size.z; z++)
			{
				if (stopUpdatingFlag)
				{
					stopUpdatingFlag = false;
					return;
				}
				if (m_ChunkArray.Chunks[x][y][z])
					m_ChunkArray.Chunks[x][y][z]->UpdateMesh();
			}
}

void World::UpdateChunks()
{
	if (updateThread.joinable())
	{
		stopUpdatingFlag = true;
		updateThread.join();
	}

	updateThread = std::thread(&World::updateChunksEntry, this);
	//updateThread.detach();
}

void World::getNeighborList(glm::uvec3 chunkPosition, Chunk* (&neighborArray)[ChunkBlockData::NeighborCount])
{
	if (!inBounds(chunkPosition))
		return;

	unsigned int& x = chunkPosition.x,
		&y = chunkPosition.y,
		&z = chunkPosition.z;

	if (x != 0) neighborArray[(unsigned int)FaceDirection::Left] = m_ChunkArray.Chunks[x - 1][y][z];
	if (x < ChunkArray::Size.x - 1) neighborArray[(unsigned int)FaceDirection::Right] = m_ChunkArray.Chunks[x + 1][y][z];
	if (y != 0) neighborArray[(unsigned int)FaceDirection::Bottom] = m_ChunkArray.Chunks[x][y - 1][z];
	if (y < ChunkArray::Size.y - 1) neighborArray[(unsigned int)FaceDirection::Top] = m_ChunkArray.Chunks[x][y + 1][z];
	if (z != 0) neighborArray[(unsigned int)FaceDirection::Back] = m_ChunkArray.Chunks[x][y][z - 1];
	if (z < ChunkArray::Size.z - 1) neighborArray[(unsigned int)FaceDirection::Front] = m_ChunkArray.Chunks[x][y][z + 1];
}

void World::reloadWorld(glm::ivec3 newCenterPosition)
{
	if (updateThread.joinable())
	{
		stopUpdatingFlag = true;
		updateThread.join();
	}

	Chunk* newChunkArray[ChunkArray::Size.x][ChunkArray::Size.y][ChunkArray::Size.z] = {nullptr};

	for (unsigned int x = 0; x < ChunkArray::Size.x; x++)
		for (unsigned int y = 0; y < ChunkArray::Size.y; y++)
			for (unsigned int z = 0; z < ChunkArray::Size.z; z++)
			{
				const glm::ivec3 localPos = { x, y, z };
				const glm::ivec3 newLocalPos = localPos + currentCenterPosition - newCenterPosition;

				if (inBounds(newLocalPos))
				{
					newChunkArray[newLocalPos.x][newLocalPos.y][newLocalPos.z] = m_ChunkArray.Chunks[x][y][z];
					continue;
				}

				deleteChunk(m_ChunkArray.Chunks[x][y][z]);
				m_ChunkArray.Chunks[x][y][z] = nullptr;
			}

	currentCenterPosition = newCenterPosition;

	for (unsigned int x = 0; x < ChunkArray::Size.x; x++)
		for (unsigned int y = 0; y < ChunkArray::Size.y; y++)
			for (unsigned int z = 0; z < ChunkArray::Size.z; z++)
			{
				if (newChunkArray[x][y][z] == nullptr)
				{
					m_ChunkArray.Chunks[x][y][z] = createNewChunk(ToGlobalPosition({ x, y, z }));
					continue;
				}
				m_ChunkArray.Chunks[x][y][z] = newChunkArray[x][y][z];
			}

	for (unsigned int x = 0; x < ChunkArray::Size.x; x++)
		for (unsigned int y = 0; y < ChunkArray::Size.y; y++)
			for (unsigned int z = 0; z < ChunkArray::Size.z; z++)
			{
				Chunk* neighborArray[ChunkBlockData::NeighborCount] = { nullptr };
				getNeighborList({ x, y, z }, neighborArray);
				m_ChunkArray.Chunks[x][y][z]->UpdateNeighbors(neighborArray);
			}

	UpdateChunks();
}

Chunk* World::createNewChunk(glm::ivec3 position)
{
	Chunk* newChunk = new Chunk(m_ChunkRenderer.GetBlockSubtextures(), position * (glm::ivec3)ChunkBlockData::ChunkSize);

	m_Generator.Generate(*newChunk, position);

	return newChunk;
}

void World::deleteChunk(Chunk* chunk)
{
	delete chunk;
}

void World::LoadWorld()
{
	for (unsigned int x = 0; x < ChunkArray::Size.x; x++)
		for (unsigned int y = 0; y < ChunkArray::Size.y; y++)
			for (unsigned int z = 0; z < ChunkArray::Size.z; z++)
			{
				const glm::vec3 position = { x, y, z };
				m_ChunkArray.Chunks[x][y][z] = createNewChunk(ToGlobalPosition(position));
			}

	for (unsigned int x = 0; x < ChunkArray::Size.x; x++)
		for (unsigned int y = 0; y < ChunkArray::Size.y; y++)
			for (unsigned int z = 0; z < ChunkArray::Size.z; z++)
			{
				Chunk* neighborArray[ChunkBlockData::NeighborCount] = { nullptr };
				getNeighborList({ x, y, z }, neighborArray);
				m_ChunkArray.Chunks[x][y][z]->UpdateNeighbors(neighborArray);
			}

	UpdateChunks();
}

World::World(GameState& gameState) :
	m_Generator(*this),
	m_GameState(gameState),
	m_ChunkRenderer(m_ChunkArray, resourceManager)
{
}

World::~World()
{
	if (updateThread.joinable())
	{
		stopUpdatingFlag = true;
		updateThread.join();
	}
	for (unsigned int x = 0; x < ChunkArray::Size.x; x++)
		for (unsigned int y = 0; y < ChunkArray::Size.y; y++)
			for (unsigned int z = 0; z < ChunkArray::Size.z; z++)
				if(m_ChunkArray.Chunks[x][y][z])
					delete m_ChunkArray.Chunks[x][y][z];
}

void World::Update()
{
	/*for (unsigned int x = 0; x < ChunkArray::Size.x; x++)
		for (unsigned int y = 0; y < ChunkArray::Size.y; y++)
			for (unsigned int z = 0; z < ChunkArray::Size.z; z++)
				m_ChunkArray.Chunks[x][y][z]->UpdateMeshBuffers();*/
}

void World::Render(RenderingContext &context)
{
	m_ChunkRenderer.RenderChunks(context, (glm::vec3)(currentCenterPosition * (glm::ivec3)ChunkBlockData::ChunkSize));
}

Chunk* World::GetChunk(glm::ivec3 position)
{
	const glm::ivec3 local = ToLocalPosition(position);
	if (inBounds(local))
		return m_ChunkArray.Chunks[local.x][local.y][local.z];
	return nullptr;
}

const Block* World::GetBlock(glm::ivec3 position)
{
	glm::ivec3 chunkPosition = glm::floor((glm::vec3)position / (glm::vec3)ChunkBlockData::ChunkSize),
		blockPosition = position - chunkPosition * (glm::ivec3)ChunkBlockData::ChunkSize;

	Chunk* chunk = GetChunk(chunkPosition);
	if (chunk == nullptr)
		return nullptr;

	return &chunk->GetBlock(blockPosition);
}

void World::DestroyBlock(glm::ivec3 position)
{
	glm::ivec3 chunkPosition = glm::floor((glm::vec3)position / (glm::vec3)ChunkBlockData::ChunkSize),
		blockPosition = position - chunkPosition * (glm::ivec3)ChunkBlockData::ChunkSize;

	Chunk* chunk = GetChunk(chunkPosition);
	if (chunk == nullptr)
		return;

	Block block(Block::BlockType::Air);
	chunk->SetBlock(blockPosition, block);
}

void World::PlaceBlock(glm::ivec3 position, const Block& block, bool force)
{

	glm::ivec3 chunkPosition = glm::floor((glm::vec3)position / (glm::vec3)ChunkBlockData::ChunkSize),
		blockPosition = position - chunkPosition * (glm::ivec3)ChunkBlockData::ChunkSize;

	Chunk* chunk = GetChunk(chunkPosition);
	if (chunk == nullptr)
		return;
	
	if (force || chunk->GetBlock(blockPosition).type == Block::BlockType::Air)
		chunk->SetBlock(blockPosition, block);
}

//bool World::Raytrace(glm::vec3 origin, glm::vec3 direction, glm::ivec3 &finalPosition)
//{
//	glm::ivec3 position = glm::floor(origin);
//	glm::vec3 tdelta, tmax = glm::vec3(0), step;
//	step.x = (direction.x > 0) - (direction.x < 0);
//	step.y = (direction.y > 0) - (direction.y < 0);
//	step.z = (direction.z > 0) - (direction.z < 0);
//
//	tdelta.x = abs(1 / direction.x);
//	tdelta.y = abs(1 / direction.y);
//	tdelta.z = abs(1 / direction.z);
//	if (direction.x > 0)
//	{
//		step.x = 1;
//		tmax.x = (ceil(origin.x) - origin.x) / abs(direction.x);
//	}
//	else if (direction.x < 0)
//	{
//		step.x = -1;
//		tmax.x = (origin.x - floor(origin.x)) / abs(direction.x);
//	}
//
//	if (direction.y > 0)
//	{
//		step.y = 1;
//		tmax.y = (ceil(origin.y) - origin.y) / abs(direction.y);
//	}
//	else if (direction.y < 0)
//	{
//		step.y = -1;
//		tmax.y = (origin.y - floor(origin.y)) / abs(direction.y);
//	}
//
//	if (direction.z > 0)
//	{
//		step.z = 1;
//		tmax.z = (ceil(origin.z) - origin.z) / abs(direction.z);
//	}
//	else if (direction.z < 0)
//	{
//		step.z = -1;
//		tmax.z = (origin.z - floor(origin.z)) / abs(direction.z);
//	}
//
//	for(int steps = 0; steps < 100; ++steps)
//	{
//		if ((direction.z == 0 || tmax.x < tmax.z) && (direction.y == 0 || tmax.x < tmax.y))
//		{
//			tmax.x += tdelta.x;
//			position.x += step.x;
//		}
//		else if ((direction.z == 0 || tmax.y < tmax.z) && (direction.x == 0 || tmax.y < tmax.x))
//		{
//			tmax.y += tdelta.y;
//			position.y += step.y;
//		}
//		else if ((direction.x == 0 || tmax.z < tmax.x) && (direction.y == 0 || tmax.z < tmax.y))
//		{
//			tmax.z += tdelta.z;
//			position.z += step.z;
//		}
//
//		glm::ivec3 chunkPos = glm::floor((glm::vec3)position / (glm::vec3)ChunkBlockData::ChunkSize),
//			blockPos = position - chunkPos * (glm::ivec3)ChunkBlockData::ChunkSize;
//
//		DebugBox::GetInstance().ChangeDebugValue("raycast", std::to_string(position.x) + " " + std::to_string(position.y) + " " + std::to_string(position.z));
//		Chunk* chunk = GetChunk(chunkPos);
//		if (chunk)
//		{
//			const Block& block = chunk->GetBlock((glm::uvec3)blockPos);
//			if (block.type != Block::BlockType::Air)
//			{
//				finalPosition = position;
//
//				//chunk->SetBlock(blockPos, Block(Block::GetIdByName("Air")), true);
//				return true;
//			}
//
//		}
//	}
//	return false;
//}

void World::SetWorldCenter(glm::ivec3 position)
{
	if (position == currentCenterPosition)
		return;

	reloadWorld(position);
}

Chunk* World::getChunk(glm::ivec3 position, bool force)
{
	return GetChunk(position);
}

bool World::isChunkGenerated(glm::ivec3 position)
{
	return inBounds(ToLocalPosition(position)); //TODO: If chunk is present in file, that return true;
}