#include <ui/Font.hpp>

bool Font::setup(const std::string file, unsigned short size)
{
	const std::string filePath = fontPath + file;

	FT_Library ft;
	FT_Face face;

	if (FT_Init_FreeType(&ft))
	{
		Debug::GetInstance().Log("Error: Cant load FreeType library");
		return false;
	}

	if (FT_New_Face(ft, filePath.c_str(), 0, &face))
	{
		Debug::GetInstance().Log("Error: Cant load font");
		return false;
	}

	FT_Set_Pixel_Sizes(face, 0, size);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	int max_dim = (1 + (face->size->metrics.height >> 6)) * ceilf(sqrtf(256));
	int tex_width = 1;
	while (tex_width < max_dim) tex_width <<= 1;
	int tex_height = tex_width;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, tex_width, tex_height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glm::ivec2 actualPosition = glm::ivec2(1.0f);
	for (unsigned char c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			Debug::GetInstance().Log("Error: Failed to load Glyph");
			continue;
		}

		if (actualPosition.x + face->glyph->bitmap.width > tex_width)
		{
			actualPosition.x = 0;
			actualPosition.y += (1 + (face->size->metrics.height >> 6));
		}

		glTexSubImage2D(GL_TEXTURE_2D, 0, actualPosition.x, actualPosition.y, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		const glm::ivec2 size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);

		FontCharacter character = {
			(glm::vec2)actualPosition / glm::vec2(tex_width, tex_height),
			(glm::vec2)size / glm::vec2(tex_width, tex_height),
			size,
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			(unsigned int)face->glyph->advance.x >> 6
		};
		characters[c] = character;

		actualPosition.x += face->glyph->bitmap.width + 1;
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

Font::Font(std::string file, unsigned short size)
{
	if (!setup(file, size))
	{
		Debug::GetInstance().Log(std::string("Error: cant load font: ") + file);
		return;
	}
}

FontCharacter& Font::GetCharacter(char c)
{
	return characters[c];
}