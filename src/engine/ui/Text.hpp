#pragma once

#include <engine/graphics/Font.hpp>
#include <engine/graphics/VertexArray.hpp>
#include <engine/graphics/Vertex.hpp>

#include <string>
#include <engine/ui/PrimitiveBuffer.hpp>

class Text
{
public:
	Text(const Font& font, glm::ivec2 position, const std::string text, glm::ivec3 color);

	void SetText(const std::string text);
	void SetPosition(glm::ivec2 position);

	void Render(RenderingParams& params) const;
private:
	PrimitiveBuffer m_Buffer;
	std::vector<std::unique_ptr<PrimitiveRectangle>> m_Letters;

	std::string m_Text;
	glm::ivec2 m_Position;
	const Font& m_Font;
	glm::ivec3 m_Color;

	void updateMesh();
	void addLetterQuad(int& offset, char letter);
};