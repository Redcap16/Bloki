#include <world/ChunkRenderer.hpp>

ChunkResources& ChunkResources::GetInstance()
{
	static ChunkResources instance;
	return instance;
}

ChunkResources::ChunkResources() :
	m_IsOpen(true),
	BlockTexture(nullptr)
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
	{
		if (i == Block::Air)
			continue;
		const std::string& blockName = Block::GetBlockName((Block::BlockType)i);
		const AtlasTexture::SubTexture* subTexture = BlockTexture->GetSubTexture(blockName);
		if (subTexture == nullptr)
		{
			DEBUG_LOG("Error: cant load sub textures");
			return false;
		}
		TextureCoords[i] = *subTexture;
	}

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

ChunkRenderer::ChunkRenderer(Renderer3D& renderer, std::shared_ptr<const Chunk> chunk) :
	m_Chunk(chunk),
	m_Position(chunk->GetPosition() * BlockArray::ChunkSize),
	m_OpaqueMesh(m_Position, ChunkResources::GetInstance().TextureCoords),
	m_TransparentMesh(m_Position, ChunkResources::GetInstance().TextureCoords),
	m_AnythingHighlighted(false),
	m_HighlightedPosition(glm::ivec3(0)),
	m_Neighbors(chunk->GetNeighbors()),
	m_CurrentBlockAccess(nullptr)
{
	ChunkResources& resources = ChunkResources::GetInstance();
	if (resources.IsOpen())
	{
		renderer.RegisterRenderable(&m_OpaqueMesh, RenderableParameters(false, resources.OpaqueShader, resources.BlockTexture));
		renderer.RegisterRenderable(&m_TransparentMesh, RenderableParameters(true, resources.TransparentShader, resources.BlockTexture));
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

void ChunkRenderer::UpdateGeometry()
{
	Chunk::BlockAccess<const BlockArray> blockAccess = m_Chunk->GetBlockAccess();
	m_CurrentBlockAccess = &blockAccess;

	for (int x = 0; x < BlockArray::ChunkSize.x; ++x)
		for (int y = 0; y < BlockArray::ChunkSize.y; ++y)
			for (int z = 0; z < BlockArray::ChunkSize.z; ++z)
				processBlock({ x, y, z });

	m_CurrentBlockAccess = nullptr;
	m_OpaqueMesh.FinishGeometry();
	m_TransparentMesh.FinishGeometry();
}

void ChunkRenderer::processBlock(InChunkPos position)
{
	assert(m_CurrentBlockAccess != nullptr);

	const Block& block = m_CurrentBlockAccess->GetBlock(position);
	for (int side = 0; side < 6; ++side)
	{
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
	assert(m_CurrentBlockAccess != nullptr);

	glm::ivec3 neighborBlockPos = (glm::ivec3)position + (glm::ivec3)GetDirectionVector(direction);

	Block neighborBlock;
	if (BlockArray::PositionInBounds(neighborBlockPos))
		neighborBlock = m_CurrentBlockAccess->GetBlock(neighborBlockPos);
	else
	{
		const Chunk* neighbor = getNeighbor(neighborBlockPos);
		if (neighbor == nullptr)
			return true; //Always visible on borders

		neighborBlock = neighbor->GetBlock(Math::Mod(neighborBlockPos, (glm::ivec3)BlockArray::ChunkSize));
	}

	if (neighborBlock.GetTransparencyType() == TransparencyType::Opaque)
		return false;

	return true;
}

const Chunk* ChunkRenderer::getNeighbor(glm::ivec3 position) const
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
	return nullptr;
}