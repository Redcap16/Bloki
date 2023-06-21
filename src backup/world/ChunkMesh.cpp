#include <world/ChunkMesh.hpp>

bool ChunkMesh::isBlockVisible(const Block& block, const Block& itsNeighbor)
{
	static const Block::BlockType airID = Block::GetIdByName("Air"); //TODO: Remove this string shit
	if (block.type == airID)
		return false;
	if (itsNeighbor.type == airID)
		return true;
	
	return false;
}

void ChunkMesh::processFace(FaceDirection direction, glm::uvec3 position, const Block& block)
{
	static constexpr unsigned int faceIndices[6][4] = {
		{ 2, 6, 12, 13 },
		{ 5, 1, 11, 10 },
		{ 5, 6, 1, 2 },
		{ 4, 8, 7, 9 },
		{ 5, 4, 6, 7 },
		{ 0, 1, 3, 2 } };
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

	AtlasTexture::SubTexture& st = Block::GetSubTexture(block.type);

	static const Block::BlockType waterID = Block::GetIdByName("Water");

	Vertex3D vertex;
	for (int i = 0; i < 4; i++)
	{
		const unsigned int vertexIndex = faceIndices[(unsigned int)direction][i];

		vertex.x = faceVertices[5 * vertexIndex + 0] + position.x;
		vertex.y = faceVertices[5 * vertexIndex + 1] + position.y;
		vertex.z = faceVertices[5 * vertexIndex + 2] + position.z;

		vertex.u = st.uv.x;
		vertex.v = st.uv.y;

		vertex.u += faceVertices[5 * vertexIndex + 3] * st.size.x;
		vertex.v += faceVertices[5 * vertexIndex + 4] * st.size.y;

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
	static const Block::BlockType airID = Block::GetIdByName("Air"); //TODO: Remove this string shit
	static const glm::uvec3 chunkSize = ChunkBlockData::ChunkSize;

	const Block& currentBlock = blockData.GetBlock(position); //TODO: Speed up things, position cant exceed baundaries

	if (currentBlock.type == airID)
		return;

	if (position.x < chunkSize.x - 1)
	{
		const Block& neighbor = blockData.GetBlock(position + glm::uvec3(1, 0, 0));
		if (isBlockVisible(currentBlock, neighbor))
			processFace(FaceDirection::Right, position, currentBlock);//const unsigned int vertexIndices[4] = { 2, 6, 12, 13 };
	}
	if (position.x > 0)
	{
		const Block& neighbor = blockData.GetBlock(position + glm::uvec3(-1, 0, 0));
		if (isBlockVisible(currentBlock, neighbor))
			processFace(FaceDirection::Left, position, currentBlock);//const unsigned int vertexIndices[4] = { 5, 1, 11, 10 };
	}
	if (position.y == chunkSize.y - 1)
	{
		const Block& neighbor = blockData.GetBlock(position + glm::uvec3(0, 1, 0));
		if (isBlockVisible(currentBlock, neighbor))
			processFace(FaceDirection::Top, position, currentBlock);//const unsigned int vertexIndices[4] = { 5, 6, 1, 2 };
	}
	if (position.y > 0)
	{
		const Block& neighbor = blockData.GetBlock(position + glm::uvec3(0, -1, 0));
		if (isBlockVisible(currentBlock, neighbor))
			processFace(FaceDirection::Bottom, position, currentBlock); //const unsigned int vertexIndices[4] = { 4, 8, 7, 9 };
	}
	if (position.z < chunkSize.z - 1)
	{
		const Block& neighbor = blockData.GetBlock(position + glm::uvec3(0, 0, 1));
		if (isBlockVisible(currentBlock, neighbor))
			processFace(FaceDirection::Front, position, currentBlock); //const unsigned int vertexIndices[4] = { 5, 4, 6, 7 };
	}
	if (position.z > 0)
	{
		const Block& neighbor = blockData.GetBlock(position + glm::uvec3(0, 0, -1));
		if (isBlockVisible(currentBlock, neighbor))
			processFace(FaceDirection::Back, position, currentBlock); //const unsigned int vertexIndices[4] = { 0, 1, 3, 2 };
	}
}

ChunkMesh::ChunkMesh(ChunkBlockData &blockData) :
	mesh({ VertexAttribute(3, GL_FLOAT),
		VertexAttribute(2, GL_FLOAT),
		VertexAttribute(1, GL_FLOAT) }, true),
	blockData(blockData)
{

}

ChunkMesh::~ChunkMesh()
{

}

void ChunkMesh::CreateMesh()
{
	const glm::uvec3 chunkSize = ChunkBlockData::ChunkSize;

	mesh.ClearVertices();

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
	mesh.UpdateBuffers();
	mesh.ClearVertices();
}