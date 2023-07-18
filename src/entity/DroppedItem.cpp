#include <entity/DroppedItem.hpp>

DroppedItem::DroppedItem(BlockManager& world, glm::vec3 position) :
	m_Rigidbody(world, {position, glm::vec3(0), glm::vec3(0.5, 0.5, 0.5)}),
	m_Mesh(false)
{
	setupMesh();
}

void DroppedItem::Update(float deltaTime)
{
	m_Rigidbody.Update(deltaTime);
}

void DroppedItem::Render()
{
	//m_Mesh.Render();
}

void DroppedItem::setupMesh()
{
	for(int i = 0; i < 6; ++i)
		setupFace((Direction)i);
}

void DroppedItem::setupFace(Direction direction)
{
	/*
	static constexpr unsigned int faceIndices[6][4] = {
		{ 5, 6, 1, 2 }, //Top
		{ 4, 8, 7, 9 }, //Bottom
		{ 2, 6, 12, 13 }, //Right
		{ 5, 1, 11, 10 }, //Left
		{ 5, 4, 6, 7 }, //Front
		{ 0, 1, 3, 2 } /*Back*/ /* };
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

	static constexpr glm::vec3 faceNormals[6] = {
		{0, 1, 0},
		{0, -1, 0},
		{1, 0, 0},
		{-1, 0, 0},
		{0, 0, 1},
		{0, 0, -1} };

	Vertex3D vertex;

	for (int i = 0; i < 4; i++)
	{
		const unsigned int vertexIndex = faceIndices[(unsigned int)direction][i];

		vertex.Position.x = faceVertices[5 * vertexIndex + 0] * 0.3;
		vertex.Position.y = faceVertices[5 * vertexIndex + 1] * 0.3;
		vertex.Position.z = faceVertices[5 * vertexIndex + 2] * 0.3;

		vertex.Normal = faceNormals[(int)direction];

		m_Mesh.AddVertex(vertex);
	}

	const unsigned int index = m_Mesh.GetCurrentIndex();

	//Add two faces
	m_Mesh.AddIndex(index - 4);
	m_Mesh.AddIndex(index - 2);
	m_Mesh.AddIndex(index - 3);

	m_Mesh.AddIndex(index - 2);
	m_Mesh.AddIndex(index - 1);
	m_Mesh.AddIndex(index - 3);*/
}