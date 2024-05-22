#include <engine/graphics/AtlasTexture.hpp>

AtlasTexture::SubTexture::SubTexture(glm::vec2 uv, glm::vec2 size) :
	UV(uv),
	Size(size) { }

AtlasTexture::SubTexture::SubTexture() :
	SubTexture(glm::vec2(0), glm::vec2(1.0f)) { }

AtlasTexture::AtlasTexture(const std::string& filename) :
	m_Handle(0),
	m_Filename(filename)
{
	load();
}

AtlasTexture::~AtlasTexture()
{
	unload();
}

const AtlasTexture::SubTexture* AtlasTexture::GetSubTexture(const std::string& name) const
{
	std::map<std::string, SubTexture>::const_iterator it = m_SubTextures.find(name);
	if (it == m_SubTextures.end())
	{
		DEBUG_LOG("Error: cant find " + name + " in " + m_Filename);
		return nullptr;
	}

	return &(it->second);
}

void AtlasTexture::Bind(GLuint textureUnit) const
{
	if (textureUnit >= c_MaxTextureIndex)
		return;

	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, m_Handle);
	CHECK_GL_ERROR();
}

AtlasTexture::DescriptionFile::DescriptionFile(const std::string& filepath) :
	m_Filepath(filepath),
	m_CurrentIndex(0),
	m_Good(false)
{
	if (readFile())
		m_Good = true;
}

void AtlasTexture::DescriptionFile::getNextToken(std::string& result)
{
	while (m_CurrentIndex < m_CurrentLine.size() && std::isspace(m_CurrentLine[m_CurrentIndex])) m_CurrentIndex++;
	size_t beginIndex = m_CurrentIndex;
	while (m_CurrentIndex < m_CurrentLine.size() && !std::isspace(m_CurrentLine[m_CurrentIndex])) m_CurrentIndex++;
	result = m_CurrentLine.substr(beginIndex, m_CurrentIndex - beginIndex);
	m_CurrentIndex++;
}

bool AtlasTexture::DescriptionFile::readCoords(const std::string& errorMessage, glm::ivec2& result)
{
	std::string token;

	getNextToken(token);
	try
	{
		result.x = std::stoi(token);
	}
	catch (std::invalid_argument e)
	{
		DEBUG_LOG(errorMessage);
		return false;
	}
	catch (std::out_of_range e)
	{
		DEBUG_LOG(errorMessage);
		return false;
	}

	getNextToken(token);
	try
	{
		result.y = std::stoi(token);
	}
	catch (std::invalid_argument e)
	{
		DEBUG_LOG(errorMessage);
		return false;
	}
	catch (std::out_of_range e)
	{
		DEBUG_LOG(errorMessage);
		return false;
	}

	return true;
}

bool AtlasTexture::DescriptionFile::readFile()
{
	std::ifstream descFile(m_Filepath, std::ios::in);

	if (!descFile.is_open())
	{
		DEBUG_LOG("Error: cant open atlas texture description file.");
		return false;
	}

	while (descFile.good())
	{
		std::getline(descFile, m_CurrentLine);

		m_CurrentIndex = 0;
		std::string token;
		getNextToken(token);

		if (token == "#file")
		{
			getNextToken(m_ImageFilename);
			if (m_ImageFilename.empty())
			{
				DEBUG_LOG("Error: missing image path in " + m_Filepath);
				return false;
			}

			continue;
		}
		if (token == "#dsize")
		{
			if (!readCoords("Error: invalid default size in " + m_Filepath, m_DefaultSize))
				return false;

			continue;
		}

		const std::string& name = token;
		glm::ivec2 coords;

		if (!readCoords("Error: invalid coords for " + name + " in " + m_Filepath, coords))
			return false;

		m_TextureCoords[name] = coords;
	}

	if (m_ImageFilename.empty())
	{
		DEBUG_LOG("Error: missing image file path in " + m_Filepath);
		return false;
	}

	if (m_DefaultSize == glm::ivec2(0, 0))
	{
		DEBUG_LOG("Error: missing default sub texture size in " + m_Filepath);
		return false;
	}

	return true;
}

void AtlasTexture::load()
{
	if (m_Handle)
		return;

	DescriptionFile descFile(c_TexturePath + m_Filename);
	if (!descFile.IsGood())
		return;

	glGenTextures(1, &m_Handle);

	if (m_Handle == 0 ||
		CHECK_GL_ERROR())
	{
		DEBUG_LOG("Error: can't create texture for " + m_Filename);
		unload();
		return;
	}

	glBindTexture(GL_TEXTURE_2D, m_Handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	CHECK_GL_ERROR();

	std::string filepath = c_TexturePath + descFile.GetImageFilename();
	int width, height, channelCount;
	unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channelCount, 0);

	if (data == nullptr)
	{
		DEBUG_LOG("Error: can't open file at " + filepath);
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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	CHECK_GL_ERROR();

	stbi_image_free(data);

	glm::vec2 subSize = (glm::vec2)descFile.GetDefaultSize() / glm::vec2(width, height);
	for (const std::pair<const std::string, glm::ivec2>& coords : descFile.GetTextureCoords())
		m_SubTextures[coords.first] = SubTexture(glm::vec2((float)coords.second.x / width, (float)coords.second.y / height), subSize);
}

void AtlasTexture::unload()
{
	glDeleteTextures(1, &m_Handle);
	CHECK_GL_ERROR();
}