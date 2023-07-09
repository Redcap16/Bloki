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
	class DescriptionFile
	{
	public:
		DescriptionFile(const std::string& filepath);
		bool IsGood() const { return m_Good; }
		const std::string& GetImageFilename() const { return m_ImageFilename; }
		const glm::ivec2& GetDefaultSize() const { return m_DefaultSize; }
		const std::map<std::string, glm::ivec2> GetTextureCoords() const { return m_TextureCoords; }
	private:
		std::string m_Filepath;

		std::string m_ImageFilename;
		glm::ivec2 m_DefaultSize;
		std::map<std::string, glm::ivec2> m_TextureCoords;

		size_t m_CurrentIndex;
		std::string m_CurrentLine;

		bool m_Good;

		void getNextToken(std::string& result);
		bool readCoords(const std::string& errorMessage, glm::ivec2& result);
		bool readFile();
	};

	TextureHandle m_Handle;
	std::string m_Filename;
	std::map<std::string, SubTexture> m_SubTextures;
	
	void load();
	void unload();
};

TextureHandle AtlasTexture::GetHandle() const
{
	return m_Handle;
}