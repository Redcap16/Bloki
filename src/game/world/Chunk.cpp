#include <game/world/Chunk.hpp>

Chunk::Chunk(const ChunkPos& position) :
	m_Position(position),
	m_Generated(false),
	m_Neighbors()
{

}

void Chunk::SetBlock(InChunkPos position, Block block)
{
	std::lock_guard<std::mutex> lock(m_GeometryMutex);
	m_BlockArray.Set(position, block);
	m_UpdateEvent.Invoke(&ChunkUpdateListener::ChunkUpdated, m_Position);

	if (BlockArray::PositionOnBorder(position)) //TODO: Think of better way of noticing them
	{
		if (position.x == 0)
		{
			Chunk* const chunk = m_Neighbors[(unsigned int)Direction::Left];
			if (chunk != nullptr) chunk->m_UpdateEvent.Invoke(&ChunkUpdateListener::ChunkUpdated, chunk->m_Position);
		}
		else if (position.x == BlockArray::ChunkSize.x - 1)
		{
			Chunk* const chunk = m_Neighbors[(unsigned int)Direction::Right];
			if (chunk != nullptr) chunk->m_UpdateEvent.Invoke(&ChunkUpdateListener::ChunkUpdated, chunk->m_Position);
		}
		if (position.y == 0)
		{
			Chunk* const chunk = m_Neighbors[(unsigned int)Direction::Bottom];
			if (chunk != nullptr) chunk->m_UpdateEvent.Invoke(&ChunkUpdateListener::ChunkUpdated, chunk->m_Position);
		}
		else if (position.y == BlockArray::ChunkSize.y - 1)
		{
			Chunk* const chunk = m_Neighbors[(unsigned int)Direction::Top];
			if (chunk != nullptr) chunk->m_UpdateEvent.Invoke(&ChunkUpdateListener::ChunkUpdated, chunk->m_Position);
		}
		if (position.z == 0)
		{
			Chunk* const chunk = m_Neighbors[(unsigned int)Direction::Back];
			if (chunk != nullptr) chunk->m_UpdateEvent.Invoke(&ChunkUpdateListener::ChunkUpdated, chunk->m_Position);
		}
		else if (position.z == BlockArray::ChunkSize.z - 1)
		{
			Chunk* const chunk = m_Neighbors[(unsigned int)Direction::Front];
			if (chunk != nullptr) chunk->m_UpdateEvent.Invoke(&ChunkUpdateListener::ChunkUpdated, chunk->m_Position);
		}
	}
}

Block Chunk::GetBlock(InChunkPos position) const
{
	std::lock_guard<std::mutex> lock(m_GeometryMutex);
	return m_BlockArray.Get(position);
}

void Chunk::SwapBlockArray(BlockArray& blockArray)
{
	std::lock_guard<std::mutex> lock(m_GeometryMutex);
	m_BlockArray.Swap(blockArray);
	m_UpdateEvent.Invoke(&ChunkUpdateListener::ChunkUpdated, m_Position);
	for (int i = 0; i < 6; i++)
		if (m_Neighbors[i])
			m_Neighbors[i]->m_UpdateEvent.Invoke(&ChunkUpdateListener::ChunkUpdated, m_Neighbors[i]->m_Position); //TODO: Think of better way of noticing them
}

void Chunk::Update()
{
}

void Chunk::UpdateNeighbors(const std::array<Chunk*, 6>& neighbors)
{
	std::lock_guard<std::mutex> lock(m_GeometryMutex);

	for (int i = 0; i < 6; i++)
		if (m_Neighbors[i] != neighbors[i])
		{
			assert(neighbors[i] != this); //Important, otherwise double locking mutex error occurs

			m_Neighbors[i] = neighbors[i];
			if (m_Neighbors[i] != nullptr)
				m_Neighbors[i]->m_UpdateEvent.Invoke(&ChunkUpdateListener::ChunkUpdated, m_Neighbors[i]->m_Position);
			m_UpdateEvent.Invoke(&ChunkUpdateListener::ChunkUpdated, m_Position);
		}
}

void Chunk::Deserialize(const std::vector<char>& data)
{
	std::lock_guard<std::mutex> lock(m_GeometryMutex);
	if (data.size() != (size_t)BlockArray::ChunkSize.x * BlockArray::ChunkSize.y * BlockArray::ChunkSize.z + 1)
	{
		DEBUG_LOG("Error: Corrupted data in file");
		return;
	}

	m_Generated = (bool)data[0];
	size_t index = 0;
	for (int y = 0; y < BlockArray::ChunkSize.y; ++y)
		for (int x = 0; x < BlockArray::ChunkSize.x; ++x)
			for (int z = 0; z < BlockArray::ChunkSize.z; ++z)
				m_BlockArray.Set({ x, y, z }, (Block::BlockType)data[++index]);

	m_UpdateEvent.Invoke(&ChunkUpdateListener::ChunkUpdated, m_Position);
}

void Chunk::Serialize(std::vector<char>& result) const
{
	std::lock_guard<std::mutex> lock(m_GeometryMutex);
	result.clear();
	result.reserve((size_t)BlockArray::ChunkSize.x * BlockArray::ChunkSize.y * BlockArray::ChunkSize.z + 1);

	result.push_back((char)m_Generated);
	for (int y = 0; y < BlockArray::ChunkSize.y; ++y)
		for (int x = 0; x < BlockArray::ChunkSize.x; ++x)
			for (int z = 0; z < BlockArray::ChunkSize.z; ++z)
				result.push_back((char)m_BlockArray.Get({ x, y, z }).Type);
}