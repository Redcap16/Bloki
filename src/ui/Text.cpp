#include <ui/Text.hpp>

Text::Text(const Font& font, glm::ivec2 position, const std::string text, glm::ivec3 color) :
	m_Font(font),
	m_Position(position),
	m_Text(text),
	m_VBO(false),
	m_EBO(false),
	m_Color(color)
{
	setupMesh();
	if(text != "")
		updateMesh();
}

void Text::SetText(const std::string text)
{
	m_Text = text;
	updateMesh();
}

void Text::SetPosition(glm::ivec2 position)
{
	m_Position = position;
	updateMesh();
}

void Text::Render() const
{
	m_Font.Bind();
	m_VAO.Draw();
}

void Text::setupMesh()
{
	m_VAO.SetElementBuffer(&m_EBO);
	m_VAO.AddBuffer(&m_VBO);
}

void Text::updateMesh()
{
	int offset = 0;
	for (char c : m_Text)
		addLetterQuad(offset, c);

	m_VBO.UpdateBuffer();
	m_EBO.UpdateBuffer();
	m_VBO.ClearData();
	m_EBO.ClearData();
}

void Text::addLetterQuad(int& offset, char letter)
{
	const Font::Glyph* glyph = m_Font.GetGlyph(letter);
	if (glyph == nullptr)
		return;

	glm::ivec2 topLeftCorner = m_Position + glm::ivec2(offset, 0) + glyph->Bearing;

	m_VBO.AddVertex({ topLeftCorner,								glyph->UVCoords,									m_Color });
	m_VBO.AddVertex({ topLeftCorner + glm::ivec2(glyph->Size.x, 0), glyph->UVCoords + glm::vec2(glyph->UVSize.x, 0),	m_Color });
	m_VBO.AddVertex({ topLeftCorner + glyph->Size,					glyph->UVCoords + glyph->UVSize,					m_Color });
	m_VBO.AddVertex({ topLeftCorner + glm::ivec2(0, glyph->Size.y), glyph->UVCoords + glm::vec2(0, glyph->UVSize.y),	m_Color });

	ElementIndex index = m_VBO.GetCurrentIndex();
	m_EBO.AddIndex(index - 3);
	m_EBO.AddIndex(index - 2);
	m_EBO.AddIndex(index - 1);

	m_EBO.AddIndex(index - 3);
	m_EBO.AddIndex(index - 1);
	m_EBO.AddIndex(index);

	offset += glyph->Advance;
}