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
		glm::vec2 uv;
		glm::vec2 size;

		SubTexture(glm::vec2 uv, glm::vec2 size);
		SubTexture();
	};
private:
	glm::ivec2 textureSize;
	unsigned int textureCount,
		textureCounter = 0;

	void setup();
public:
	AtlasTexture(glm::ivec2 textureSize, unsigned int textureCount);
	~AtlasTexture();

	std::pair<bool, SubTexture> Load(const std::string filename);
};