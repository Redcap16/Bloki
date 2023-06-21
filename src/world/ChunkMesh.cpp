#include <world/ChunkMesh.hpp>

/*ChunkMeshShader::ChunkMeshShader() :
	ShaderProgram(shaderFilePath)
{

}

void ChunkMeshShader::SetMVPMatrix(glm::mat4 matrix)
{
	setUniform("mvpMatrix", matrix);
}

void ChunkMeshShader::SetTextureIndex(unsigned int index)
{
	setUniform("textureAtlas", (int)index);
}

ChunkMeshShader* ChunkMesh::shaderProgram;*/


bool ChunkMesh::isBlockVisible(const Block& block, const Block& itsNeighbor)
{
	if (block.type == Block::BlockType::Air)
		return false;
	if (block.type == Block::BlockType::Water &&
		itsNeighbor.type == Block::BlockType::Water)
		return false;
	if (itsNeighbor.type == Block::BlockType::Air ||
		Block::blockData[(int)itsNeighbor.type].TransType != TransparencyType::Opaque)
		return true;
	
	return false;
}

bool ChunkMesh::isBlockOnBorderVisible(ChunkBlockData* neighboringChunk, glm::uvec3 neighborBlockPosition, const Block& block)
{
	static const glm::uvec3& chunkSize = ChunkBlockData::ChunkSize;

	if (neighboringChunk == nullptr)
		return true;
	
	const Block& neighbor = neighboringChunk->GetBlock(neighborBlockPosition);
	if (isBlockVisible(block, neighbor))
		return true;

	return false;
}

void ChunkMesh::processFace(FaceDirection direction, glm::uvec3 position, const Block& block, bool highlighted)
{
	static constexpr unsigned int faceIndices[6][4] = {
		{ 5, 6, 1, 2 }, //Top
		{ 4, 8, 7, 9 }, //Bottom
		{ 2, 6, 12, 13 }, //Right
		{ 5, 1, 11, 10 }, //Left
		{ 5, 4, 6, 7 }, //Front
		{ 0, 1, 3, 2 } /*Back*/ };
	static constexpr float faceVertices[14 * 5] = {
	   0, 0, 0, 0, 0.333333333f,
	   0, 1, 0, 0.25f, 0.333333f,
	   1, 1, 0, 0.25f, 0.66666f,
	   1, 0, 0, 0, 0.666666f,
	   0, 0, 1, 0.75f, 0.33333f,
	   0, 1, 1, 0.5f, 0.33333f,
	   1, 1, 1, 0.5f, 0.66666f,
	   1, 0, 1, 0.75, 0.66666f,
	   0, 0, 0, 1, 0.333333f,
	   1, 0, 0, 1, 0.666666f,
	   0, 0, 0, 0.25f, 0,
	   0, 0, 1, 0.5f, 0,
	   1, 0, 0, 0.25f, 1,
	   1, 0, 1, 0.5f, 1 };
	//static constexpr unsigned int faceIndices[6][4] = {
	//	{ 16, 17, 18, 19 }, //Top
	//	{ 8, 9, 10, 11 }, //Bottom
	//	{ 20, 21, 22, 23}, //Right
	//	{ 12, 13, 14, 15 }, //Left
	//	{ 5, 4, 6, 7 }, //Front
	//	{ 0, 1, 3, 2 } /*Back*/ };
	//static constexpr float faceVertices[24 * 8] = {
	//   0, 0, 0, 0, 0, -1, 0, 0.333333333f,
	//   0, 1, 0, 0, 0, -1, 0.25f, 0.333333f,
	//   1, 1, 0, 0, 0, -1, 0.25f, 0.66666f,
	//   1, 0, 0, 0, 0, -1, 0, 0.666666f,
	//   0, 0, 1, 0, 0, 1, 0.75f, 0.33333f,
	//   0, 1, 1, 0, 0, 1, 0.5f, 0.33333f,
	//   1, 1, 1, 0, 0, 1, 0.5f, 0.66666f,
	//   1, 0, 1, 0, 0, 1, 0.75, 0.66666f,
	//   0, 0, 0, 0, -1, 0, 1, 0.333333f,
	//   1, 0, 0, 0, -1, 0, 1, 0.666666f,
	//   1, 0, 1, 0, -1, 0, 0.75, 0.66666f,
	//   0, 0, 1, 0, -1, 0, 0.75f, 0.33333f,
	//   0, 0, 0, -1, 0, 0, 0.25f, 0,
	//   0, 0, 1, -1, 0, 0, 0.5f, 0,
	//   0, 1, 0, -1, 0, 0, 0.25f, 0.333333f,
	//   0, 1, 1, -1, 0, 0, 0.5f, 0.33333f,
	//   0, 1, 0, 0, 1, 0, 0.25f, 0.333333f,
	//   1, 1, 0, 0, 1, 0, 0.25f, 0.66666f,
	//   1, 1, 1, 0, 1, 0, 0.5f, 0.6666f,
	//   0, 1, 1, 0, 1, 0, 0.5f, 0.3333f,
	//   1, 0, 0, 1, 0, 0, 0.25f, 1,
	//   1, 0, 1, 1, 0, 0, 0.5f, 1,
	//   1, 1, 1, 1, 0, 0, 0.5f, 0.6666f,
	//   1, 1, 0, 1, 0, 0, 0.25f, 0.6666f};

	static constexpr glm::vec3 faceNormals[6] = {
		{0, 1, 0},
		{0, -1, 0},
		{1, 0, 0},
		{-1, 0, 0},
		{0, 0, 1},
		{0, 0, -1} };

	const AtlasTexture::SubTexture& st = m_BlockSubtextures.TextureCoords[(int)block.type];

	Vertex3D vertex;

	VertexBuffer<Vertex3D>& mesh = Block::blockData[(int)block.type].TransType == TransparencyType::PartialTransparency ? m_TransparentMesh : m_OpaqueMesh;

	for (int i = 0; i < 4; i++)
	{
		const unsigned int vertexIndex = faceIndices[(unsigned int)direction][i];

		vertex.Position.x = faceVertices[5 * vertexIndex + 0] + position.x;
		vertex.Position.y = faceVertices[5 * vertexIndex + 1] + position.y;
		vertex.Position.z = faceVertices[5 * vertexIndex + 2] + position.z;

		vertex.TextureCoords.x = st.UV.x;
		vertex.TextureCoords.y = st.UV.y;

		vertex.TextureCoords.x += faceVertices[5 * vertexIndex + 3] * st.Size.x;
		vertex.TextureCoords.y += faceVertices[5 * vertexIndex + 4] * st.Size.y;

		vertex.state = highlighted;

		vertex.Normal = faceNormals[(int)direction];

		mesh.AddVertex(vertex);
	}

	const unsigned int index = mesh.GetCurrentIndex();

	//Add two faces
	mesh.AddIndex(index - 4);
	mesh.AddIndex(index - 2);
	mesh.AddIndex(index - 3);

	mesh.AddIndex(index - 2);
	mesh.AddIndex(index - 1);
	mesh.AddIndex(index - 3);
}

void ChunkMesh::processBlock(glm::uvec3 position)
{
	static const glm::uvec3 chunkSize = ChunkBlockData::ChunkSize;

	const Block& currentBlock = blockData.GetBlock(position); //TODO: Speed up things, position cant exceed baundaries

	if (currentBlock.type == Block::BlockType::Air)
		return;

	const bool highlighted = position == this->highlightedPos && this->isHighlighted;

	if (position.x < chunkSize.x - 1)
	{
		const Block& neighbor = blockData.GetBlock(position + glm::uvec3(1, 0, 0));
		if (isBlockVisible(currentBlock, neighbor))
			processFace(FaceDirection::Right, position, currentBlock, highlighted);
	}
	else if (position.x == chunkSize.x - 1 && 
		isBlockOnBorderVisible(neighborArray.XPlus, position - glm::uvec3(chunkSize.x - 1, 0, 0), currentBlock))
		processFace(FaceDirection::Right, position, currentBlock, highlighted);

	if (position.x > 0)
	{
		const Block& neighbor = blockData.GetBlock(position - glm::uvec3(1, 0, 0));
		if (isBlockVisible(currentBlock, neighbor))
			processFace(FaceDirection::Left, position, currentBlock, highlighted);
	}
	else if (position.x == 0 && 
		isBlockOnBorderVisible(neighborArray.XMinus, position + glm::uvec3(chunkSize.x - 1, 0, 0), currentBlock))
		processFace(FaceDirection::Left, position, currentBlock, highlighted);

	if (position.y < chunkSize.y - 1)
	{
		const Block& neighbor = blockData.GetBlock(position + glm::uvec3(0, 1, 0));
		if (isBlockVisible(currentBlock, neighbor))
			processFace(FaceDirection::Top, position, currentBlock, highlighted);
	}
	else if (position.y == chunkSize.y - 1 && 
		isBlockOnBorderVisible(neighborArray.YPlus, position - glm::uvec3(0, chunkSize.y - 1, 0), currentBlock))
		processFace(FaceDirection::Top, position, currentBlock, highlighted);

	if (position.y > 0)
	{
		const Block& neighbor = blockData.GetBlock(position - glm::uvec3(0, 1, 0));
		if (isBlockVisible(currentBlock, neighbor))
			processFace(FaceDirection::Bottom, position, currentBlock, highlighted);
	}
	else if (position.y == 0 && 
		isBlockOnBorderVisible(neighborArray.YMinus, position + glm::uvec3(0, chunkSize.y - 1, 0), currentBlock))
		processFace(FaceDirection::Bottom, position, currentBlock, highlighted);

	if (position.z < chunkSize.z - 1)
	{
		const Block& neighbor = blockData.GetBlock(position + glm::uvec3(0, 0, 1));
		if (isBlockVisible(currentBlock, neighbor))
			processFace(FaceDirection::Front, position, currentBlock, highlighted);
	}
	else if (position.z == chunkSize.z - 1 && 
		isBlockOnBorderVisible(neighborArray.ZPlus, position - glm::uvec3(0, 0, chunkSize.z - 1), currentBlock))
		processFace(FaceDirection::Front, position, currentBlock, highlighted);

	if (position.z > 0)
	{
		const Block& neighbor = blockData.GetBlock(position - glm::uvec3(0, 0, 1));
		if (isBlockVisible(currentBlock, neighbor))
			processFace(FaceDirection::Back, position, currentBlock, highlighted);
	}
	else if (position.z == 0 && 
		isBlockOnBorderVisible(neighborArray.ZMinus, position + glm::uvec3(0, 0, chunkSize.z - 1), currentBlock))
		processFace(FaceDirection::Back, position, currentBlock, highlighted);
}

ChunkMesh::ChunkMesh(ChunkBlockData& blockData, const BlockSubtextures& blockSubtextures) :
	m_OpaqueMesh(true),
	m_TransparentMesh(true),
	blockData(blockData),
	m_BlockSubtextures(blockSubtextures)
{
}

ChunkMesh::~ChunkMesh()
{

}

void ChunkMesh::SetHighlighted(glm::uvec3 position)
{
	highlightedPos = position;
	isHighlighted = true;
}

void ChunkMesh::UnsetHighlight()
{
	isHighlighted = false;
}

void ChunkMesh::CreateMesh()
{
	const glm::uvec3 chunkSize = ChunkBlockData::ChunkSize;

	m_OpaqueMesh.ClearVertices();
	m_TransparentMesh.ClearVertices();

	for (int x = 0; x < chunkSize.x; x++)
		for (int y = 0; y < chunkSize.y; y++)
			for (int z = 0; z < chunkSize.z; z++)
			{
				const glm::uvec3 currentBlockPosition = { x, y, z };
				processBlock(currentBlockPosition);
			}
}

void ChunkMesh::UpdateBuffers()
{
	m_OpaqueMesh.UpdateBuffers();
	m_OpaqueMesh.ClearVertices();
	m_TransparentMesh.UpdateBuffers();
	m_TransparentMesh.ClearVertices();
}

void ChunkMesh::UpdateNeighbors(const ChunkNeighbors& neighbors)
{
	neighborArray = neighbors;
}

void ChunkMesh::Render(bool transparent)
{
	if (transparent)
		m_TransparentMesh.Render();
	else
		m_OpaqueMesh.Render();
}