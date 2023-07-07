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
public:
	ImageTexture(const std::string& filename, bool transparency);
	~ImageTexture();
	ImageTexture(const ImageTexture&) = delete;
	ImageTexture& operator=(const ImageTexture&) = delete;

	void Bind(GLuint textureUnit) const override;
	inline TextureHandle GetHandle() const override;
private:
	TextureHandle m_Handle;
	std::string m_Filename;

	void load(bool transparency);
	void unload();
};

TextureHandle ImageTexture::GetHandle() const
{
	return m_Handle;
}