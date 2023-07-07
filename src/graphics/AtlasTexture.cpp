#include <graphics/AtlasTexture.hpp>

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

void AtlasTexture::getNextToken(const std::string& contents, size_t start, size_t &end, std::string& result)
{
	while (start < contents.size() && std::isspace(contents[start])) start++;
	size_t tokenBegin = start;
	while (start < contents.size() && !std::isspace(contents[start])) start++;
	result = contents.substr(tokenBegin, start - tokenBegin);
	end = start + 1;
}

bool AtlasTexture::readCoords(const std::string& firstVal, const std::string& secondVal, const std::string& errorMessage, glm::ivec2& result)
{
	try
	{
		result.x = std::stoi(firstVal);
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

	try
	{
		result.y = std::stoi(secondVal);
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

bool AtlasTexture::readDescFile(DescriptionFileContents& contents)
{
	std::string descPath = c_TexturePath + m_Filename;
	std::ifstream descFile(descPath, std::ios::in);

	if (!descFile.is_open())
	{
		DEBUG_LOG("Error: cant open atlas texture description file.");
		return false;
	}

	std::string line, imageFile;
	glm::ivec2 defaultSize;
	std::map<std::string, glm::ivec2> subCoords;
	while (descFile.good())
	{
		std::getline(descFile, line);

		size_t index = 0;
		std::string token;
		getNextToken(line, 0, index, token);

		if (token == "#file")
		{
			getNextToken(line, index, index, imageFile);
			if (imageFile.empty())
			{
				DEBUG_LOG("Error: missing image path in " + m_Filename);
				return false;
			}
			continue;
		}
		if (token == "#dsize")
		{
			std::string first, second;
			getNextToken(line, index, index, first);
			getNextToken(line, index, index, second);

			if (!readCoords(first, second, "Error: invalid default size in " + m_Filename, defaultSize))
				return false;

			continue;
		}

		const std::string& name = token;
		glm::ivec2 coords;

		std::string first, second;
		getNextToken(line, index, index, first);
		getNextToken(line, index, index, second);

		if (!readCoords(first, second, "Error: invalid coords for " + name + " in " + m_Filename, coords))
			return false;

		subCoords[name] = coords;
	}

	if (imageFile.empty())
	{
		DEBUG_LOG("Error: missing image file path in " + m_Filename);
		return false;
	}
	if (defaultSize == glm::ivec2(0, 0))
	{
		DEBUG_LOG("Error: missing default sub texture size in " + m_Filename);
		return false;
	}

	contents.ImageFilepath = imageFile;
	contents.DefaultSize = defaultSize;
	contents.SubTextureCoords = subCoords;

	return true;
}

void AtlasTexture::load()
{
	if (m_Handle)
		return;

	DescriptionFileContents descFileContents;
	if (!readDescFile(descFileContents))
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

	std::string filepath = c_TexturePath + descFileContents.ImageFilepath;
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

	glm::vec2 subSize = (glm::vec2)descFileContents.DefaultSize / glm::vec2(width, height);
	for (std::pair<const std::string, glm::ivec2>& coords : descFileContents.SubTextureCoords)
		m_SubTextures[coords.first] = SubTexture(glm::vec2((float)coords.second.x / width, (float)coords.second.y / height), subSize);
}

void AtlasTexture::unload()
{
	glDeleteTextures(1, &m_Handle);
}