#include <graphics/Texture.hpp>

Texture::Texture()
{
}

void Texture::BindTexture(unsigned short index)
{
	if (index > maxTextureIndex)
		return;

	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, texture);
}