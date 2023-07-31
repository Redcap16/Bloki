#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <glm/glm.hpp>

#include <ft2build.h>
#include <freetype/ftmodapi.h>
#include FT_FREETYPE_H  

#include <util/Debug.hpp>
#include <graphics/ErrorCheck.hpp>

#include <string>
#include <memory>
#include <vector>

class Font
{
public:
	struct Glyph
	{
		glm::vec2 UVCoords,
			UVSize;
		glm::ivec2 Size,
			Bearing;
		int Advance;

		Glyph() :
			UVCoords(0),
			UVSize(0),
			Size(0),
			Bearing(0),
			Advance(0)
		{ }

		Glyph(glm::vec2 uvCoords, glm::vec2 uvSize, glm::ivec2 size, glm::ivec2 bearing, int advance) :
			UVCoords(uvCoords),
			UVSize(uvSize),
			Size(size),
			Bearing(bearing),
			Advance(advance)
		{ }
	};

	Font(const std::string& filename, int letterSize);
	~Font();
	Font(const Font&) = delete;
	Font& operator=(const Font&) = delete;

	const Glyph* GetGlyph(char letter) const;
private:
	static const int c_FirstPrintableChar = ' ', 
		c_LastPrintableChar = '~',
		c_CharsCount = c_LastPrintableChar - c_FirstPrintableChar + 1;

	static FT_Library s_Library;
	static FT_Memory s_LibraryMemory;

	FT_Face m_Face;

	Glyph m_Glyphs[c_CharsCount];
	GLuint m_Texture;

	const int m_LetterSize;

	void setupLibrary();
	void loadGlyphs();
};