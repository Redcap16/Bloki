#pragma once

#include <string>

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <glm/glm.hpp>

#include <graphics/stb_image.h>
#include <graphics/Texture.hpp>
#include <util/Debug.hpp>

class AtlasTexture : public Texture
{
public:
	struct SubTexture
	{
		glm::vec2 UV;
		glm::vec2 Size;

		SubTexture(glm::vec2 uv, glm::vec2 size);
		SubTexture();
	};
private:
	struct TextureData
	{
		glm::ivec2 Size;
		unsigned char* Data;
		bool AlphaChannel;
		SubTexture& TextureBounds;

		TextureData(SubTexture& textureBounds);
	};

	std::vector<TextureData> m_TextureData;

	void setup();
	void unloadTextures();
public:
	AtlasTexture();
	~AtlasTexture();

	bool Load(const std::string filename, SubTexture& result);
	void Bake();
};