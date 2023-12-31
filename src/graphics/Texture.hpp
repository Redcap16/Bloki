#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <string>
#include <util/Debug.hpp>
#include <graphics/ErrorCheck.hpp>

typedef GLuint TextureHandle;

class Texture
{
public:
	static constexpr GLenum GetFormatFromChannelCount(unsigned short channelCount);
	virtual void Bind(GLuint textureUnit) const = 0;
	virtual TextureHandle GetHandle() const = 0;

protected:
	static constexpr short c_MaxTextureIndex = 16;
	static constexpr const char* c_TexturePath = "assets/textures/";
};

constexpr GLenum Texture::GetFormatFromChannelCount(unsigned short channelCount)
{
	switch (channelCount)
	{
	case 1:
		return GL_RED;
	case 2:
		return GL_RG;
	case 3:
		return GL_RGB;
	case 4:
		return GL_RGBA;
	}

	return 0;
}