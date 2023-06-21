#pragma once

#include <map>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H  

#include <ui/FontCharacter.hpp>
#include <graphics/Texture.hpp>
#include <util/Debug.hpp>

class Font : public Texture
{
private:
	const std::string fontPath = "assets/fonts/";
	std::map<char, FontCharacter> characters;

	bool setup(const std::string file, unsigned short size);
public:
	Font(std::string file, unsigned short size);
	FontCharacter& GetCharacter(char c);
};