#include <graphics/CubemapTexture.hpp>

CubemapTexture::CubemapTexture(const std::string& filename) :
	m_Handle(0),
	m_Filename(filename)
{
	load();
}

CubemapTexture::~CubemapTexture()
{
	unload();
}

void CubemapTexture::Bind(GLuint textureUnit) const
{
	if (textureUnit >= c_MaxTextureIndex)
		return;

	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_Handle);
	CHECK_GL_ERROR();
}

void CubemapTexture::load()
{
	if (m_Handle)
		return;

	glGenTextures(1, &m_Handle);
	
	if (m_Handle == 0 ||
		CHECK_GL_ERROR())
	{
		DEBUG_LOG("Error: failed to create texture at " + m_Filename);
		unload();
		return;
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, m_Handle);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	CHECK_GL_ERROR();

	std::string filepath = c_TexturePath + m_Filename;

	int width, height, channelCount;
	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channelCount, 0);

	if (data == nullptr)
	{
		DEBUG_LOG("Error: failed to load texture file at " + filepath);
		unload();
		return;

	}

	if (width == height * 6)
	{
		DEBUG_LOG("Error: image at " + filepath + " doesn't contain proper cube map");
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

	const GLenum sideDirection[] = { GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, 
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y};

	std::vector<unsigned char> sideData;
	for (unsigned short side = 0; side < 6; ++side)
	{
		getSide(height, data, side, sideData);
		glTexImage2D(sideDirection[side], 0, GL_RGB, height, height, 0, format, GL_UNSIGNED_BYTE, sideData.data());
	}

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	CHECK_GL_ERROR();

	stbi_image_free(data);
}

void CubemapTexture::unload()
{
	glDeleteTextures(1, &m_Handle);
	CHECK_GL_ERROR();
}

void CubemapTexture::getSide(unsigned int height,
	unsigned char* data,
	unsigned short side,
	std::vector<unsigned char>& result)
{
	result.clear();

	if (side >= 6 || data == nullptr)
		return;

	//Assuming, that the sides are squares, total height is equal to a width of a one side
	const unsigned int width = height * 6;
	result.reserve((size_t)height * height);
	for (unsigned int y = 0; y < height; y++)
	{
		const unsigned int xLimit = height * (side + 1);
		for (unsigned int x = height * side; x < xLimit; x++)
			result.push_back(data[x + y * width]);
	}
}