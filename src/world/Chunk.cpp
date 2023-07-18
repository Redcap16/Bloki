#include <world/Chunk.hpp>

Chunk::Chunk(Renderer3D& renderer, const ChunkPos& position) :
	m_Position(position),
	m_ChunkRenderer(renderer, m_BlockArray, (glm::vec3)(position * (glm::ivec3)BlockArray::ChunkSize)),
	m_AnythingHighlighted(false),
	m_GeometryUpdateNeeded(false),
	m_Generated(false),
	m_HighlightedPosition(glm::ivec3()),
	m_Neighbors()
{

}

void Chunk::SetHighlight(InChunkPos position)
{
	std::lock_guard<std::mutex> lock(m_GeometryMutex);
	m_HighlightedPosition = position;
	m_AnythingHighlighted = true;
	m_GeometryUpdateNeeded = true;
}

void Chunk::ResetHighlight()
{
	std::lock_guard<std::mutex> lock(m_GeometryMutex);
	m_AnythingHighlighted = false;
	m_GeometryUpdateNeeded = true;
}

void Chunk::SetBlock(InChunkPos position, Block block)
{
	std::lock_guard<std::mutex> lock(m_GeometryMutex);
	m_BlockArray.Set(position, block);
	m_GeometryUpdateNeeded = true;
}

const Block& Chunk::GetBlock(InChunkPos position) const
{
	std::lock_guard<std::mutex> lock(m_GeometryMutex);
	return m_BlockArray.Get(position);
}

void Chunk::SwapBlockArray(BlockArray&& blockArray)
{
	std::lock_guard<std::mutex> lock(m_GeometryMutex);
	m_BlockArray = std::move(blockArray);
	m_GeometryUpdateNeeded = true;
}

void Chunk::Update()
{
	checkItemsBoundaries();
}

void Chunk::UpdateNeighbors(const std::array<Chunk*, 6>& neighbors)
{
	std::array<const BlockArray*, 6>neighborsArrays;
	for (int i = 0; i < 6; i++)
	{
		if (m_Neighbors[i] != neighbors[i])
		{
			m_Neighbors[i] = neighbors[i];
			m_Neighbors[i]->m_GeometryUpdateNeeded = true;
			m_GeometryUpdateNeeded = true;
		}
		neighborsArrays[i] = &neighbors[i]->m_BlockArray;
	}

	m_ChunkRenderer.UpdateNeighbors(neighborsArrays);
}

void Chunk::UpdateGeometry()
{
	if (m_GeometryUpdateNeeded)
	{
		std::lock_guard<std::mutex> lock(m_GeometryMutex);
		m_ChunkRenderer.UpdateGeometry();

		m_GeometryUpdateNeeded = false;
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
			i++;
	}
}

void Chunk::moveItem(Chunk& destination, std::vector<std::unique_ptr<DroppedItem>>::iterator it)
{
	destination.m_DroppedItems.push_back(std::move(*it));
	m_DroppedItems.erase(it);
}

void Chunk::Deserialize(const std::vector<char>& data)
{
	//Remember locking mutex
	//TODO
}

void Chunk::Serialize(std::vector<char>& result) const
{
	//Remember locking mutex
	//TODO
}