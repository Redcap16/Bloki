#include <graphics/ImageTexture.hpp>

bool ImageTexture::load(const std::string path)
{
	const std::string filePath = texturePath + path;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		Debug::GetInstance().Log("Error: Failed to load texture");
		return false;
	}

	glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}

ImageTexture::ImageTexture(const std::string path)
{
	load(path);
}

ImageTexture::~ImageTexture()
{
	if (texture)
		glDeleteTextures(1, &texture);
}