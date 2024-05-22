#include <engine/graphics/Rectangle.hpp>

using namespace graphics;

Rectangle::Rectangle(glm::ivec2 position, glm::ivec2 size, glm::vec2 textureCoords, glm::vec2 textureSize, glm::ivec4 color) :
	m_Position(position),
	m_Size(size),
	m_TextureCoords(textureCoords),
	m_TextureSize(textureSize),
	m_Color(color),
	m_VBO(&m_VAO.CreateVertexBuffer<Vertex2D>(false)),
	m_EBO(&m_VAO.GetElementBuffer()),
	m_VAO(false)
{
	update();
}

Rectangle::Rectangle(glm::ivec2 position, glm::ivec2 size, glm::ivec4 color) :
	Rectangle(position, size, glm::vec2(0), glm::vec2(0), color)
{

}

void Rectangle::SetPosition(glm::ivec2 position)
{
	m_Position = position;
	update();
}

void Rectangle::SetSize(glm::ivec2 size)
{
	m_Size = size;
	update();
}

void Rectangle::SetTextureCoords(glm::vec2 textureCoords, glm::vec2 textureSize)
{
	m_TextureCoords = textureCoords;
	m_TextureSize = textureSize;
	update();
}

void Rectangle::SetColor(glm::ivec4 color)
{
	m_Color = color;
	update();
}

void Rectangle::update()
{
	m_VBO->AddVertex(Vertex2D(m_Position, m_TextureCoords, m_Color));
	m_VBO->AddVertex(Vertex2D(m_Position + glm::ivec2(m_Size.x, 0), m_TextureCoords + glm::vec2(m_TextureSize.x, 0), m_Color));
	m_VBO->AddVertex(Vertex2D(m_Position + m_Size, m_TextureCoords + m_TextureSize, m_Color));
	m_VBO->AddVertex(Vertex2D(m_Position + glm::ivec2(0, m_Size.y), m_TextureCoords + glm::vec2(0, m_TextureSize.y), m_Color));

	m_EBO->AddIndices({ 0, 1, 2, 0, 2, 3 });

	m_VBO->UpdateBuffer();
	m_VBO->ClearData();
	m_EBO->UpdateBuffer();
	m_EBO->ClearData();
}
