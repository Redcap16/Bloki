#pragma once

#include <string>
#include <map>
#include <fstream>

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <glm/glm.hpp>

#include <graphics/stb_image.h>
#include <graphics/Texture.hpp>
#include <util/Debug.hpp>

class AtlasTexture : public Texture
{
public:
	struct SubTexture
	{
		glm::vec2 UV;
		glm::vec2 Size;

		SubTexture(glm::vec2 uv, glm::vec2 size);
		SubTexture();
	};

	AtlasTexture(const std::string& filename);
	~AtlasTexture();
	AtlasTexture(const AtlasTexture&) = delete;
	AtlasTexture& operator=(const AtlasTexture&) = delete;

	const SubTexture* GetSubTexture(const std::string& name) const;
	void Bind(GLuint textureUnit) const override;
	inline TextureHandle GetHandle() const override;

private:
	struct DescriptionFileContents
	{
		std::string ImageFilepath;
		glm::ivec2 DefaultSize;
		std::map<std::string, glm::ivec2> SubTextureCoords;
	};

	TextureHandle m_Handle;
	std::string m_Filename;
	std::map<std::string, SubTexture> m_SubTextures;

	void getNextToken(const std::string& contents, size_t start, size_t &end, std::string& result);
	bool readCoords(const std::string& firstVal, const std::string& secondVal, const std::string& errorMessage, glm::ivec2& result);
	bool readDescFile(DescriptionFileContents& contents);
	void load();
	void unload();
};

TextureHandle AtlasTexture::GetHandle() const
{
	return m_Handle;
}