#include <graphics/Texture.hpp>

Texture::Texture()
{
}

void Texture::BindTexture(ShaderProgram& program, std::string name, unsigned short index)
{
	if (index > maxTextureIndex)
		return;

	program.SetUniform(name, index);
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, texture);
}