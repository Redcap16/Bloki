#include <graphics/ImageTexture.hpp>

ImageTexture::ImageTexture(const std::string& filename, bool transparency) :
	m_Handle(0),
	m_Filename(filename)
{
	load(transparency);
}

ImageTexture::~ImageTexture()
{
	unload();
}

void ImageTexture::Bind(GLuint textureUnit) const
{
	if (textureUnit >= c_MaxTextureIndex)
		return;

	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, m_Handle);
	CHECK_GL_ERROR();
}

void ImageTexture::load(bool transparency)
{
	if (m_Handle)
		return;

	glGenTextures(1, &m_Handle);
	if (m_Handle == 0 ||
		CHECK_GL_ERROR())
	{
		DEBUG_LOG("Error: Can't create texture for: " + m_Filename);
		unload();
		return;
	}

	glBindTexture(GL_TEXTURE_2D, m_Handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	CHECK_GL_ERROR();

	std::string filepath = c_TexturePath + m_Filename;

	int width, height, channelCount;
	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channelCount, 0);

	if (data == nullptr)
	{
		DEBUG_LOG("Error: Can't open specified file: " + filepath);
		unload();
		return;
	}

	GLenum format = GetFormatFromChannelCount(channelCount);
	if (!format)
	{
		DEBUG_LOG("Error: invalid texture channel count at " + filepath);
		unload();
		return;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, transparency ? GL_RGBA : GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data); 
	glGenerateMipmap(GL_TEXTURE_2D);
	CHECK_GL_ERROR();

	stbi_image_free(data);
}

void ImageTexture::unload()
{
	glDeleteTextures(1, &m_Handle);
}