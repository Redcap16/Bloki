#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <glm/glm.hpp>

#include <ft2build.h>
#include <freetype/ftmodapi.h>
#include FT_FREETYPE_H  

#include <engine/util/Debug.hpp>
#include <engine/graphics/ErrorCheck.hpp>
#include <engine/core/Resource.hpp>
#include <engine/util/Math.hpp>

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
	~Font() = default;
	Font(const Font&) = delete;
	Font& operator=(const Font&) = delete;

	void Bind() const { glBindTexture(GL_TEXTURE_2D, m_Texture); };
	const Glyph* GetGlyph(char letter) const;
private:
	static constexpr const char* c_FontPath = "assets/fonts/";
	static const int c_FirstPrintableChar = ' ', 
		c_LastPrintableChar = '~',
		c_CharsCount = c_LastPrintableChar - c_FirstPrintableChar + 1;

	static std::weak_ptr<FT_LibraryRec_> s_Library;
	std::shared_ptr<FT_LibraryRec_> m_Library;

	FT_Face m_Face;

	Glyph m_Glyphs[c_CharsCount];
	GLuint m_Texture;

	const int m_LetterSize;

	void setupLibrary();
	void loadGlyphs();
};

template <>
struct ResourceParams<Font>
{
	std::string Filename;
	int LetterSize;

	ResourceParams(const std::string& filename, int letterSize) :
		Filename(filename),
		LetterSize(letterSize) { }
};

inline bool operator==(const ResourceParams<Font>& lhs, const ResourceParams<Font>& rhs)
{
	return lhs.Filename == rhs.Filename &&
		lhs.LetterSize == lhs.LetterSize;
}

template <>
class std::hash<ResourceParams<Font>>
{
public:
	size_t operator()(const ResourceParams<Font>& key) const
	{
		size_t result;
		hash_combine(result, key.Filename, key.LetterSize);
		return result;
	}
};