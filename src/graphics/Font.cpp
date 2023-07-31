#include <graphics/Font.hpp>

FT_Library Font::s_Library;
FT_Memory Font::s_LibraryMemory;

Font::Font(const std::string& filename, int letterSize) :
	m_LetterSize(letterSize)
{
	setupLibrary();

	FT_Error error = FT_Reference_Library(s_Library);
	if (error)
	{
		DEBUG_LOG("Error: Cannot reference FreeType library");
		return;
	}

	error = FT_New_Face(s_Library, filename.c_str(), 0, &m_Face);
	if (error)
	{
		DEBUG_LOG("Error: Cant load FreeType face");
		return;
	}

	error = FT_Set_Pixel_Sizes(m_Face, m_LetterSize, 0);
	if (error)
	{
		DEBUG_LOG("Error: FreeType cant set char size");
		return;
	}

	loadGlyphs();

	error = FT_Done_Face(m_Face);
	if (error)
		DEBUG_LOG("Error: Cant release FreeType face");
}

Font::~Font()
{
	FT_Error error = FT_Done_Library(s_Library);
	if (error)
		DEBUG_LOG("Error: Cant release FreeType library");
}

const Font::Glyph* Font::GetGlyph(char letter) const
{
	if (letter < c_FirstPrintableChar ||
		letter > c_LastPrintableChar)
		return nullptr;

	if (m_Glyphs[letter - c_FirstPrintableChar].Size.x == 0)
		return nullptr;

	return &m_Glyphs[letter - c_FirstPrintableChar];
}

void Font::setupLibrary()
{
	if (s_Library != nullptr)
		return;

	FT_Error error = FT_New_Library(s_LibraryMemory, &s_Library);
	if (error)
	{
		DEBUG_LOG("Error: cannot create FreeType library instance");
		return;
	}

	FT_Add_Default_Modules(s_Library);
	FT_Set_Default_Properties(s_Library);
}

void Font::loadGlyphs()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	CHECK_GL_ERROR();

	std::vector<unsigned char> buffers[c_CharsCount];
	glm::ivec2 textureSize,
		rowSize;
	int rowLenght = std::ceil(sqrt(c_CharsCount));

	for (int i = 0; i < c_CharsCount; ++i)
	{
		if (i % rowLenght == 0)
		{
			textureSize.x = std::max(textureSize.x, rowSize.x);
			textureSize.y += rowSize.y;
		}

		FT_UInt glyphIndex = FT_Get_Char_Index(m_Face, c_FirstPrintableChar + i);
		if (glyphIndex == 0)
			continue;

		FT_Error error = FT_Load_Glyph(m_Face, glyphIndex, FT_LOAD_DEFAULT);
		if (error)
			DEBUG_LOG("Error: Cant load FreeType glyph");

		if (m_Face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
		{
			error = FT_Render_Glyph(m_Face->glyph, FT_RENDER_MODE_NORMAL);
			if (error)
				DEBUG_LOG("Error: Cant render FreeType glyph");
		}
		
		m_Glyphs[i].Size = glm::ivec2(m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows);
		m_Glyphs[i].Bearing = glm::ivec2(m_Face->glyph->bitmap_left, m_Face->glyph->bitmap_top);
		m_Glyphs[i].Advance = m_Face->glyph->advance.x;

		buffers[i].reserve((size_t)m_Glyphs[i].Size.x * m_Glyphs[i].Size.y);
		std::memcpy(buffers[i].data(), m_Face->glyph->bitmap.buffer, (size_t)m_Glyphs[i].Size.x * m_Glyphs[i].Size.y);

		rowSize.x += m_Glyphs[i].Size.x;
		rowSize.y = std::max(rowSize.y, m_Glyphs[i].Size.y);
	}
	textureSize.x = std::max(textureSize.x, rowSize.x);
	textureSize.y += rowSize.y;

	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureSize.x, textureSize.y, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	CHECK_GL_ERROR();

	glm::ivec2 currentOffset;
	int rowHeight = 0;

	for (int i = 0; i < c_CharsCount; ++i)
	{
		if (i % rowLenght == 0)
		{
			currentOffset.x = 0;
			currentOffset.y += rowHeight;
			rowHeight = 0;
		}

		glTexSubImage2D(GL_TEXTURE_2D, 0, currentOffset.x, currentOffset.y, m_Glyphs[i].Size.x, m_Glyphs[i].Size.y, GL_RED, GL_UNSIGNED_BYTE, buffers[i].data());
		CHECK_GL_ERROR();
		m_Glyphs[i].UVCoords = (glm::vec2)currentOffset / (glm::vec2)textureSize;
		m_Glyphs[i].UVSize = (glm::vec2)m_Glyphs[i].Size / (glm::vec2)textureSize;

		rowHeight = std::max(rowHeight, m_Glyphs[i].Size.y);
		currentOffset.x += m_Glyphs[i].Size.x;
	}
}