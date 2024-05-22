#include <engine/ui/Text.hpp>

Text::Text(const Font& font, glm::ivec2 position, const std::string text, glm::ivec3 color) :
	m_Font(font),
	m_Position(position),
	m_Text(text),
	m_Buffer(ColoringType::AlphaTexture),
	m_Color(color)
{
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

void Text::Render(RenderingParams& params) const
{
	m_Font.Bind();
	m_Buffer.Render(params);
}

void Text::updateMesh()
{
	m_Letters.clear();
	m_Buffer.Clear();

	int offset = 0;
	for (char c : m_Text)
		addLetterQuad(offset, c);

	m_Buffer.Update();
}

void Text::addLetterQuad(int& offset, char letter)
{
	const Font::Glyph* glyph = m_Font.GetGlyph(letter);
	if (glyph == nullptr)
		return;

	glm::ivec2 topLeftCorner = m_Position + glm::ivec2(offset, 0) + glyph->Bearing;

	m_Letters.push_back(std::make_unique<PrimitiveRectangle>(irect(topLeftCorner, glyph->Size), frect(glyph->UVCoords, glyph->UVSize), m_Color));
	m_Buffer.AddPrimitive(m_Letters.back().get());

	offset += glyph->Advance;
}