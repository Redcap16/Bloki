#include <world/Chunk.hpp>

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
	checkItemsBoundaries();
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

void Chunk::AddDroppedItem(std::unique_ptr<DroppedItem> item)
{
	m_DroppedItems.push_back(std::move(item));
}

void Chunk::RemoveDroppedItem(const DroppedItem* item)
{
	for (std::vector<std::unique_ptr<DroppedItem>>::const_iterator it = m_DroppedItems.begin(); it != m_DroppedItems.end(); ++it)
		if (it->get() == item)
		{
			m_DroppedItems.erase(it);
			return;
		}
}

void Chunk::GetDroppedItems(std::vector<DroppedItem*>& items)
{
	std::vector<DroppedItem*>().swap(items);
	for (std::unique_ptr<DroppedItem>& item : m_DroppedItems)
		items.push_back(item.get());
}

void Chunk::checkItemsBoundaries()
{
	for (int i = 0; i < m_DroppedItems.size();)
	{
		const glm::vec3& itemPosition = m_DroppedItems[i]->GetPosition();

		if (itemPosition.x < m_Position.x
			&& m_Neighbors[(unsigned int)Direction::Left] != nullptr)
			moveItem(*(m_Neighbors[(unsigned int)Direction::Left]), m_DroppedItems.begin() + i);
		else if (itemPosition.x > m_Position.x + BlockArray::ChunkSize.x 
			&& m_Neighbors[(unsigned int)Direction::Right] != nullptr)
			moveItem(*(m_Neighbors[(unsigned int)Direction::Right]), m_DroppedItems.begin() + i);
		else if (itemPosition.y < m_Position.y
			&& m_Neighbors[(unsigned int)Direction::Bottom] != nullptr)
			moveItem(*(m_Neighbors[(unsigned int)Direction::Bottom]), m_DroppedItems.begin() + i);
		else if (itemPosition.y > m_Position.y + BlockArray::ChunkSize.y
			&& m_Neighbors[(unsigned int)Direction::Top] != nullptr)
			moveItem(*(m_Neighbors[(unsigned int)Direction::Top]), m_DroppedItems.begin() + i);
		else if (itemPosition.z < m_Position.z
			&& m_Neighbors[(unsigned int)Direction::Back] != nullptr)
			moveItem(*(m_Neighbors[(unsigned int)Direction::Back]), m_DroppedItems.begin() + i);
		else if (itemPosition.z > m_Position.z + BlockArray::ChunkSize.z
			&& m_Neighbors[(unsigned int)Direction::Front] != nullptr)
			moveItem(*(m_Neighbors[(unsigned int)Direction::Front]), m_DroppedItems.begin() + i);
		else
			i++; //TODO: Rewrite
	}
}

void Chunk::moveItem(Chunk& destination, std::vector<std::unique_ptr<DroppedItem>>::iterator it)
{
	destination.m_DroppedItems.push_back(std::move(*it));
	m_DroppedItems.erase(it);
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