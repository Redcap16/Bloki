#pragma once

#include <graphics/Font.hpp>
#include <graphics/VertexArray.hpp>
#include <graphics/Vertex.hpp>

#include <string>

class Text
{
public:
	Text(const Font& font, glm::ivec2 position, const std::string text, glm::ivec3 color);

	void SetText(const std::string text);
	void SetPosition(glm::ivec2 position);

	void Render() const;
private:
	VertexArray m_VAO;
	VertexBuffer<Vertex2D> m_VBO;
	ElementBuffer m_EBO;

	std::string m_Text;
	glm::ivec2 m_Position;
	const Font& m_Font;
	glm::ivec3 m_Color;

	void setupMesh();
	void updateMesh();
	void addLetterQuad(int& offset, char letter);
};