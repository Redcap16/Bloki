#include <game/graphics/ChunkMesh.hpp>
#include <game/graphics/BlockTextureProvider.hpp>

ChunkMesh::ChunkMesh(glm::vec3& position, bool transparent) :
	m_Transparent(transparent),
	m_MeshVAO(true),
	m_Position(position),
	m_MeshVBO(m_MeshVAO.CreateVertexBuffer<Vertex3DS>(true)),
	m_MeshEBO(m_MeshVAO.GetElementBuffer()) {
}

void ChunkMesh::ClearMesh() {
	std::unique_lock<std::mutex> lock(m_FaceBufferMutex);
	m_FaceBuffer.clear();
}

void ChunkMesh::AddFace(Direction dir, InChunkPos position, Block block, BlockState state)
{
	std::unique_lock<std::mutex> lock(m_FaceBufferMutex);
	m_FaceBuffer.push_back({ dir, position, block, state });
}

void ChunkMesh::FinishGeometry() 
{
	if(!m_Transparent) //Doesnt need sorting
		processFaces();

	m_GeometryFinished = true;
}

glm::mat4 ChunkMesh::GetModelMatrix() const
{
	return glm::translate(glm::mat4(1.0), m_Position);
}

void ChunkMesh::Render(const RenderingContext& context)
{
	if (m_GeometryFinished) {
		if (m_Transparent) { //Need sorting
			sortFaces(context.CurrentCamera3D->GetPosition());
			processFaces();
		}

		if (m_BuffersNeedUpdate)
			updateBuffers();
	}

	m_MeshVAO.Draw();
}

bool ChunkMesh::FaceCompare::operator()(const Face& a, const Face& b) {
	return glm::distance(a.GetFaceCenterPosition(), c_CameraPosition) < glm::distance(b.GetFaceCenterPosition(), c_CameraPosition);
}

glm::vec3 ChunkMesh::Face::GetFaceCenterPosition() const {
	return glm::vec3(position) + glm::vec3(0.5, 0.5, 0.5) + glm::vec3(s_FaceNormals[(int)direction]) * 0.5f;
}

void ChunkMesh::updateBuffers()
{
	m_MeshVBO.UpdateBuffer();
	m_MeshVBO.ClearData();

	m_MeshEBO.UpdateBuffer();
	m_MeshEBO.ClearData();

	m_BuffersNeedUpdate = false;
}

void ChunkMesh::sortFaces(glm::vec3 cameraPosition) {
	std::unique_lock<std::mutex> lock(m_FaceBufferMutex);
	std::sort(m_FaceBuffer.begin(), m_FaceBuffer.end(), FaceCompare(cameraPosition - m_Position));
}

void ChunkMesh::processFaces() {
	std::unique_lock<std::mutex> lock(m_FaceBufferMutex);
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

	for (const auto& face : m_FaceBuffer) {
		const Texture& subTexture = game::graphics::BlockTextureProvider::GetLoader().GetBlockTexture(face.block.Type);

		Vertex3DS vertex;

		for (int i = 0; i < 4; i++)
		{
			const unsigned int vertexIndex = faceIndices[(unsigned int)face.direction][i];
			vertex.Position = (InChunkPos)faceVertices[vertexIndex].Position + face.position;

			vertex.TextureCoords = subTexture.GetUVPosition();
			vertex.TextureCoords += faceVertices[vertexIndex].TexCoords * subTexture.GetUVSize();
			//vertex.TextureCoords = glm::ivec2(1, 1);

			vertex.Normal = s_FaceNormals[(unsigned int)face.direction];

			vertex.state = face.state.HighLighted;
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

	m_BuffersNeedUpdate = true;
}