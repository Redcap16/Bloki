#include <world/Chunk.hpp>

Chunk::Chunk(const BlockSubtextures& blockSubtextures, glm::vec3 globalPosition) :
	m_ChunkMesh(m_BlockData, blockSubtextures),
	m_GlobalPosition(globalPosition)
{
}

void Chunk::UpdateMesh()
{
	if (m_MeshChanged)
	{
		m_BuffersChanged = false;
		m_ChunkMesh.CreateMesh();

		m_MeshChanged = false;
		m_BuffersChanged = true;
	}
}


void Chunk::ResetHighlight()
{
	if (!m_IsHighlighted)
		return;
	m_IsHighlighted = false;
	m_ChunkMesh.UnsetHighlight();
	
	changed();
}

void Chunk::SetHighlight(glm::uvec3 position)
{
	if (m_IsHighlighted && m_HighlightedPos == position)
		return;

	m_HighlightedPos = position;
	m_IsHighlighted = true;
	m_ChunkMesh.SetHighlighted(position);

	changed();
}

void Chunk::UpdateNeighbors(Chunk* neighbors[6])
{
	ChunkNeighbors newNeighbors;

	for (int i = 0; i < 6; ++i)
	{
		if (m_Neighbors[i] != neighbors[i])
		{
			m_Neighbors[i] = neighbors[i];
			m_MeshChanged = true;
		}
	}


	/*for (int i = 0; i < 6; ++i)
	{
		Chunk* chunk = container.GetChunk(position + (glm::ivec3)GetDirectionVector((FaceDirection)i));

		if (this->neighbors[i] != chunk)
		{
			this->neighbors[i] = chunk;
			m_MeshChanged = true;
		}

		if (chunk)
			neighbors.XPlus = &chunk->blockData;
	}*/
	
	if (m_Neighbors[0] != nullptr)
		newNeighbors.YPlus = &m_Neighbors[0]->m_BlockData;
	if (m_Neighbors[1] != nullptr)
		newNeighbors.YMinus = &m_Neighbors[1]->m_BlockData;
	if (m_Neighbors[2] != nullptr)
		newNeighbors.XPlus = &m_Neighbors[2]->m_BlockData;
	if (m_Neighbors[3] != nullptr)
		newNeighbors.XMinus = &m_Neighbors[3]->m_BlockData;
	if (m_Neighbors[4] != nullptr)
		newNeighbors.ZPlus = &m_Neighbors[4]->m_BlockData;
	if (m_Neighbors[5] != nullptr)
		newNeighbors.ZMinus = &m_Neighbors[5]->m_BlockData;


	m_ChunkMesh.UpdateNeighbors(newNeighbors);
}

void Chunk::Render(bool transparent)
{
	updateMeshBuffers();
	m_ChunkMesh.Render(transparent);

	if(!transparent)
		for (std::unique_ptr<DroppedItem>& item : m_DroppedItems)
			item->Render(); //THATS NOT WORKING BECAUSE ALL SETTINGS NEEDED TO PROPERLY DRAW THIS SHIT ARE SET IN CHUNK RENDERER, FOR CHUNK NOT FOR ITEM
}

const Block& Chunk::GetBlock(glm::uvec3 position)
{
	return m_BlockData.GetBlock(position);
}

void Chunk::SetBlock(glm::uvec3 position, const Block& block)
{
	if (m_BlockData.GetBlock(position).type == block.type)
		return;

	m_BlockData.SetBlock(position, block);
	m_MeshChanged = true;

	if (m_BlockData.PositionOnBorder(position))
		for (int i = 0; i < 6; ++i)
			if (m_Neighbors[i])
				m_Neighbors[i]->changed();
}

void Chunk::Update(float deltaTime)
{
	for (int index = 0; index < m_DroppedItems.size(); ++index)
	{
		std::unique_ptr<DroppedItem>& item = m_DroppedItems[index];
		item->Update(deltaTime);
		checkItemBoundaries(item.get(), index);
	}
}

void Chunk::checkItemBoundaries(DroppedItem* item, int index)
{
	if (item == nullptr)
		return;

	const glm::vec3& itemPosition = item->GetPosition();

	if (itemPosition.x < m_GlobalPosition.x)
		moveItemToNeighbor(index, m_Neighbors[(int)FaceDirection::Left]);
	else if (itemPosition.x > m_GlobalPosition.x + ChunkBlockData::ChunkSize.x)
		moveItemToNeighbor(index, m_Neighbors[(int)FaceDirection::Right]);
	else if (itemPosition.y < m_GlobalPosition.y)
		moveItemToNeighbor(index, m_Neighbors[(int)FaceDirection::Bottom]);
	else if (itemPosition.y > m_GlobalPosition.y + ChunkBlockData::ChunkSize.y)
		moveItemToNeighbor(index, m_Neighbors[(int)FaceDirection::Top]);
	else if (itemPosition.z < m_GlobalPosition.z)
		moveItemToNeighbor(index, m_Neighbors[(int)FaceDirection::Back]);
	else if (itemPosition.z > m_GlobalPosition.z + ChunkBlockData::ChunkSize.z)
		moveItemToNeighbor(index, m_Neighbors[(int)FaceDirection::Front]);
}

void Chunk::moveItemToNeighbor(int index, Chunk* chunk)
{
	if (chunk == nullptr)
		return;

	chunk->m_DroppedItems.push_back(std::move(m_DroppedItems[index]));
	m_DroppedItems.erase(m_DroppedItems.begin() + index);
}

void Chunk::updateMeshBuffers()
{
	if (m_BuffersChanged)
	{
		m_ChunkMesh.UpdateBuffers();
		m_BuffersChanged = false;
	}
}