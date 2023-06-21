#pragma once

#include <string>

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <glm/glm.hpp>

#include <graphics/stb_image.h>
#include <graphics/Texture.hpp>
#include <util/Debug.hpp>

class ImageTexture : public Texture
{
private:
	bool load(const std::string path);
public:
	ImageTexture(const std::string path);

	~ImageTexture();
};
