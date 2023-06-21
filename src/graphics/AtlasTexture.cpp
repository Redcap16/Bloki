#include <graphics/AtlasTexture.hpp>

AtlasTexture::SubTexture::SubTexture(glm::vec2 uv, glm::vec2 size) :
	UV(uv),
	Size(size)
{

}

AtlasTexture::SubTexture::SubTexture() :
	SubTexture(glm::vec2(0), glm::vec2(1.0f))
{

}

AtlasTexture::TextureData::TextureData(SubTexture& textureBounds) :
	TextureBounds(textureBounds)
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
}

void AtlasTexture::unloadTextures()
{
	for (TextureData& textureData : m_TextureData)
		stbi_image_free(textureData.Data);

	std::vector<TextureData>().swap(m_TextureData);
}

AtlasTexture::AtlasTexture()
{
	setup();
}

AtlasTexture::~AtlasTexture()
{

	if (texture)
		glDeleteTextures(1, &texture);
}

bool AtlasTexture::Load(const std::string filename, AtlasTexture::SubTexture& result)
{
	int nrChannels;
	const std::string filePath = texturePath + filename;

	TextureData textureData(result);

	textureData.Data = stbi_load(filePath.c_str(), &textureData.Size.x, &textureData.Size.y, &nrChannels, 0);
	if (!textureData.Data)
	{
		std::cout << "Failed to load texture" << std::endl;
		return false;
	}
	
	textureData.AlphaChannel = nrChannels == 4;
	
	m_TextureData.push_back(textureData);

	return true;
}

void AtlasTexture::Bake()
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glm::ivec2 textureSize = glm::ivec2(0);

	for (TextureData& textureData : m_TextureData)
	{
		if (textureSize.x < textureData.Size.x)
			textureSize.x = textureData.Size.x;

		textureSize.y += textureData.Size.y;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSize.x, textureSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	int currentY = 0;
	for (TextureData& textureData : m_TextureData)
	{
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 
			currentY, 
			textureData.Size.x, 
			textureData.Size.y, 
			textureData.AlphaChannel ? GL_RGBA : GL_RGB, 
			GL_UNSIGNED_BYTE, 
			textureData.Data);

		textureData.TextureBounds.Size = glm::vec2(1.0f, (float)textureData.Size.y / textureSize.y);
		textureData.TextureBounds.UV = glm::vec2(0, (float)currentY / textureSize.y);

		currentY += textureData.Size.y;
	}

	unloadTextures();

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}