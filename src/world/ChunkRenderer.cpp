#include <world/ChunkRenderer.hpp>

ChunkResources::ChunkResources() :
	m_IsOpen(true)
{
	if (!setupTexture())
		m_IsOpen = false;
	if (!setupShaders())
		m_IsOpen = false;
}

bool ChunkResources::setupTexture()
{
	if (BlockTexture != nullptr)
		return true;

	ResourceManager& resourceManager = ResourceManager::GetInstance();
	BlockTexture = resourceManager.GetAtlasTexture(c_TextureFilename);
	
	if (BlockTexture == nullptr)
	{
		DEBUG_LOG("Error: cant load block texture");
		return false;
	}

	for (int i = 0; i < Block::c_BlockCount; ++i)
		TextureCoords[i] = *BlockTexture->GetSubTexture(Block::GetBlockName((Block::BlockType)i));

	return true;
}

bool ChunkResources::setupShaders()
{
	ResourceManager& resourceManager = ResourceManager::GetInstance();

	OpaqueShader = resourceManager.GetShaderProgram(c_OpaqueShaderFilename);
	TransparentShader = resourceManager.GetShaderProgram(c_TransparentShaderFilename);

	if (OpaqueShader == nullptr ||
		TransparentShader == nullptr)
		return false;

	return true;
}

ChunkRenderer::ChunkRenderer(Renderer3D& renderer, const BlockArray& blockArray, glm::vec3& position) :
	m_BlockArray(blockArray),
	m_OpaqueMesh(position),
	m_TransparentMesh(position),
	m_Position(position)
{
	if (s_Resources.get() == nullptr)
		s_Resources = std::make_unique<ChunkResources>();

	if (s_Resources->IsOpen())
	{
		renderer.RegisterRenderable(&m_OpaqueMesh, RenderableParameters(false, s_Resources->OpaqueShader, s_Resources->BlockTexture));
		renderer.RegisterRenderable(&m_TransparentMesh, RenderableParameters(true, s_Resources->TransparentShader, s_Resources->BlockTexture));
	}
}

void ChunkRenderer::SetHighlight(InChunkPos position)
{
	m_HighlightedPosition = position;
	m_AnythingHighlighted = true;
}

void ChunkRenderer::ResetHighlight()
{
	m_AnythingHighlighted = false;
}

void ChunkRenderer::UpdateNeighbors(const BlockArray* neighbors[6])
{
	for (int i = 0; i < 6; ++i)
		m_Neighbors[i] = neighbors[i];
}

void ChunkRenderer::UpdateGeometry()
{
	for (int x = 0; x < BlockArray::ChunkSize.x; ++x)
		for (int y = 0; y < BlockArray::ChunkSize.y; ++y)
			for (int z = 0; z < BlockArray::ChunkSize.z; ++z)
				processBlock({ x, y, z });

	m_OpaqueMesh.FinishGeometry();
	m_TransparentMesh.FinishGeometry();
}

void ChunkRenderer::processBlock(InChunkPos position)
{
	for (int side = 0; side < 6; ++side)
	{
		const Block& block = m_BlockArray.Get(position);
		if (block.Type != Block::Air &&
			isBlockVisible(position, (Direction)side))
		{
			bool&& highlighted = m_AnythingHighlighted && position == m_HighlightedPosition;

			if (block.GetTransparencyType() == TransparencyType::FullTransparency)
				m_TransparentMesh.AddFace((Direction)side, position, block, { highlighted });
			else 
				m_OpaqueMesh.AddFace((Direction)side, position, block, { highlighted });
		}
	}
}

bool ChunkRenderer::isBlockVisible(InChunkPos position, Direction direction) 
{
	glm::ivec3 neighborBlockPos = (glm::ivec3)position + (glm::ivec3)GetDirectionVector(direction);

	const Block* neighborBlock = nullptr;
	if (m_BlockArray.PositionInBounds(neighborBlockPos))
		neighborBlock = &m_BlockArray.Get(position);
	else
	{
		const BlockArray* neighbor = getNeighbor(neighborBlockPos);
		if (neighbor == nullptr)
			return true; //Always visible on borders

		neighborBlock = &neighbor->Get(glm::mod(neighborBlockPos, (glm::ivec3)BlockArray::ChunkSize));
	}

	if (neighborBlock->GetTransparencyType() == TransparencyType::Opaque)
		return false;

	return true;
}

const BlockArray* ChunkRenderer::getNeighbor(glm::ivec3 position)
{
	if (position.x >= BlockArray::ChunkSize.x)
		return m_Neighbors[(unsigned int)Direction::Right];
	if (position.x < 0)
		return m_Neighbors[(unsigned int)Direction::Left];
	if (position.y >= BlockArray::ChunkSize.y)
		return m_Neighbors[(unsigned int)Direction::Top];
	if (position.y < 0)
		return m_Neighbors[(unsigned int)Direction::Bottom];
	if (position.z >= BlockArray::ChunkSize.z)
		return m_Neighbors[(unsigned int)Direction::Front];
	if (position.z < 0)
		return m_Neighbors[(unsigned int)Direction::Back];
}