#include <world/ChunkRenderer.hpp>

AtlasTexture::SubTexture ChunkRenderer::s_TextureCoords[Block::c_BlockCount];
bool ChunkRenderer::s_TextureCoordsCreated = false;

ChunkRenderer::ChunkRenderer(Renderer3D& renderer, std::shared_ptr<const Chunk> chunk) :
	m_Chunk(chunk),
	m_Position(chunk->GetPosition()* BlockArray::ChunkSize),
	m_OpaqueMesh(m_Position, s_TextureCoords),
	m_TransparentMesh(m_Position, s_TextureCoords),
	m_AnythingHighlighted(false),
	m_HighlightedPosition(glm::ivec3(0)),
	m_Neighbors(chunk->GetNeighbors()),
	m_CurrentBlockAccess(nullptr),
	m_Renderer(renderer),
	m_BlockTexture(c_TextureFilename),
	m_OpaqueShader(c_OpaqueShaderFilename),
	m_TransparentShader(c_TransparentShaderFilename)
{
	loadTextureCoords();

	renderer.RegisterRenderable(&m_OpaqueMesh, RenderableParameters(false, m_OpaqueShader.Get(), m_BlockTexture.Get()));
	renderer.RegisterRenderable(&m_TransparentMesh, RenderableParameters(true, m_TransparentShader.Get(), m_BlockTexture.Get()));
}

ChunkRenderer::~ChunkRenderer()
{
	m_Renderer.RemoveRenderable(&m_OpaqueMesh);
	m_Renderer.RemoveRenderable(&m_TransparentMesh);
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

void ChunkRenderer::processBlock(const InChunkPos& position)
{
	assert(m_CurrentBlockAccess != nullptr);

	const Block& block = m_CurrentBlockAccess->GetBlock(position);
	for (int side = 0; side < 6; ++side)
	{
		if (block.Type != Block::Air &&
			isBlockVisible(block, position, (Direction)side))
		{
			bool&& highlighted = m_AnythingHighlighted && position == m_HighlightedPosition;

			if (block.GetTransparencyType() == TransparencyType::PartialTransparency)
				m_TransparentMesh.AddFace((Direction)side, position, block, { highlighted });
			else 
				m_OpaqueMesh.AddFace((Direction)side, position, block, { highlighted });
		}
	}
}

bool ChunkRenderer::isBlockVisible(Block block, const InChunkPos& position, Direction direction) 
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

	if (neighborBlock.GetTransparencyType() == TransparencyType::Opaque || 
		neighborBlock.Type == block.Type)
		return false;

	return true;
}

const Chunk* ChunkRenderer::getNeighbor(const glm::ivec3& position) const
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

void ChunkRenderer::loadTextureCoords()
{
	if (s_TextureCoordsCreated)
		return;

	for (int i = 0; i < Block::c_BlockCount; ++i)
	{
		if (i == Block::Air)
			continue;

		const std::string& blockName = Block::GetBlockName((Block::BlockType)i);
		const AtlasTexture::SubTexture* subTexture = m_BlockTexture->GetSubTexture(blockName);
		if (subTexture == nullptr)
		{
			DEBUG_LOG("Error: cant load sub textures");
		}
		s_TextureCoords[i] = *subTexture;
	}

	s_TextureCoordsCreated = true;
}