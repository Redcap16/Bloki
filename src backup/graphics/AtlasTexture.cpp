#include <graphics/AtlasTexture.hpp>

AtlasTexture::SubTexture::SubTexture(glm::vec2 uv, glm::vec2 size) :
	uv(uv),
	size(size)
{

}

AtlasTexture::SubTexture::SubTexture() :
	SubTexture(glm::vec2(0), glm::vec2(1.0f))
{

}

void AtlasTexture::setup()
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureSize.x, textureSize.y * textureCount, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
}

AtlasTexture::AtlasTexture(glm::ivec2 textureSize, unsigned int textureCount) :
	textureSize(textureSize),
	textureCount(textureCount)
{
	setup();
}

AtlasTexture::~AtlasTexture()
{
	if (texture)
		glDeleteTextures(1, &texture);
}

std::pair<bool, AtlasTexture::SubTexture> AtlasTexture::Load(const std::string filename)
{
	if (textureCounter == textureCount)
		return { false, SubTexture() };

	int width, height, nrChannels;

	const std::string filePath = texturePath + filename;

	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
	if (!data || width != textureSize.x || height != textureSize.y)
	{
		std::cout << "Failed to load texture" << std::endl;
		return { false, SubTexture() };
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, textureSize.y * textureCounter, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);

	textureCounter++;
	if (textureCounter == textureCount)
		glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return { true, SubTexture(glm::vec2(0, (float)(textureCounter - 1) / textureCount), glm::vec2(1, 1.0f / textureCount)) };
}