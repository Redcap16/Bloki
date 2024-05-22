#include <game/graphics/ChunkMesh.hpp>

ChunkMesh::ChunkMesh(glm::vec3& position, const AtlasTexture::SubTexture(&textureCoords)[Block::c_BlockCount]) :
	m_MeshVAO(true),
	m_Position(position),
	m_TextureCoords(textureCoords),
	m_MeshVBO(m_MeshVAO.CreateVertexBuffer<Vertex3DS>(true)),
	m_MeshEBO(m_MeshVAO.GetElementBuffer())
{
}

void ChunkMesh::AddFace(Direction dir, InChunkPos position, Block block, BlockState state)
{
	static const unsigned int faceIndices[6][4] = {
		{ 5, 6, 1, 2 }, //Top
		{ 4, 8, 7, 9 }, //Bottom
		{ 2, 6, 12, 13 }, //Right
		{ 5, 1, 11, 10 }, //Left
		{ 5, 4, 6, 7 }, //Front
		{ 0, 1, 3, 2 } /*Back*/ };

	static const struct
	{
		glm::uvec3 Position;
		glm::vec2 TexCoords;
	} faceVertices[14] = {
		{{0, 0, 0}, {0,		0.3333f}},
		{{0, 1, 0}, {0.25f, 0.3333f}},
		{{1, 1, 0}, {0.25f, 0.6666f}},
		{{1, 0, 0}, {0,		0.6666f}},
		{{0, 0, 1}, {0.75f, 0.3333f}},
		{{0, 1, 1}, {0.5f,	0.3333f}},
		{{1, 1, 1}, {0.5f,	0.6666f}},
		{{1, 0, 1}, {0.75,	0.6666f}},
		{{0, 0, 0}, {1,		0.3333f}},
		{{1, 0, 0}, {1,		0.6666f}},
		{{0, 0, 0}, {0.25f, 0}},
		{{0, 0, 1}, {0.5f,	0}},
		{{1, 0, 0}, {0.25f, 1}},
		{{1, 0, 1}, {0.5f,	1}} };

	static constexpr glm::ivec3 faceNormals[6] = {
		{0, 1, 0},
		{0, -1, 0},
		{1, 0, 0},
		{-1, 0, 0},
		{0, 0, 1},
		{0, 0, -1} };

	const AtlasTexture::SubTexture& subTexture = m_TextureCoords[(int)block.Type];

	Vertex3DS vertex;

	for (int i = 0; i < 4; i++)
	{
		const unsigned int vertexIndex = faceIndices[(unsigned int)dir][i];
		vertex.Position = (InChunkPos)faceVertices[vertexIndex].Position + position;

		vertex.TextureCoords = subTexture.UV;
		vertex.TextureCoords += faceVertices[vertexIndex].TexCoords * subTexture.Size;
		//vertex.TextureCoords = glm::ivec2(1, 1);

		vertex.Normal = faceNormals[(unsigned int)dir];

		vertex.state = state.HighLighted;
		m_MeshVBO.AddVertex(vertex);
	}

	const ElementIndex index = (ElementIndex)m_MeshVBO.GetCurrentIndex();

	//Add two faces
	m_MeshEBO.AddIndex(index - 3);
	m_MeshEBO.AddIndex(index - 1);
	m_MeshEBO.AddIndex(index - 2);

	m_MeshEBO.AddIndex(index - 1);
	m_MeshEBO.AddIndex(index - 0);
	m_MeshEBO.AddIndex(index - 2);
}

glm::mat4 ChunkMesh::GetModelMatrix() const
{
	return glm::translate(glm::mat4(1.0), m_Position);
}

void ChunkMesh::Render(const RenderingContext& context)
{
	if (m_GeometryFinished)
		updateBuffers();

	m_MeshVAO.Draw();
}

void ChunkMesh::updateBuffers()
{
	m_MeshVBO.UpdateBuffer();
	m_MeshVBO.ClearData();

	m_MeshEBO.UpdateBuffer();
	m_MeshEBO.ClearData();

	m_GeometryFinished = false;
}