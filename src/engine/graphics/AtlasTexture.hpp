#pragma once

#include <string>
#include <map>
#include <unordered_set>
#include <fstream>

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <glm/glm.hpp>

#include <stb_image/stb_image.h>
#include <engine/graphics/Texture.hpp>
#include <engine/core/Resource.hpp>
#include <engine/util/Debug.hpp>
#include <engine/util/Math.hpp>



class AtlasTexture : public Texture
{
public:
	struct SubTextureHash;
	struct SubTexture : public Texture //TODO: Migrate it to a proper class (not struct)
	{
	public:
		friend SubTextureHash;
		friend bool operator==(const AtlasTexture::SubTexture& lhs, const AtlasTexture::SubTexture& rhs);

		glm::vec2 UV;
		glm::vec2 Size;

		SubTexture(glm::vec2 uv, glm::vec2 size, GLuint handle = NULL);
		SubTexture();

		void Bind(GLuint textureUnit) const override;
		TextureHandle GetHandle() const override { return m_Handle; };
		glm::vec2 GetUVSize() const override { return Size; };
		glm::vec2 GetUVPosition() const override { return UV; };
	private:
		GLuint m_Handle;
	};

	struct SubTextureHash {
		size_t operator()(const AtlasTexture::SubTexture& key) const;
	};

	AtlasTexture(const std::string& filename);
	~AtlasTexture();
	AtlasTexture(const AtlasTexture&) = delete;
	AtlasTexture& operator=(const AtlasTexture&) = delete;

	const SubTexture* GetSubTexture(const std::string& name) const;
	const SubTexture* GetSubTexture(glm::vec2 position, glm::vec2 size);
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
	std::unordered_set<SubTexture, SubTextureHash> m_CustomSubtextures;
	
	void load();
	void unload();
};

TextureHandle AtlasTexture::GetHandle() const
{
	return m_Handle;
}

template <>
struct ResourceParams<AtlasTexture>
{
	std::string Filename;

	ResourceParams(const std::string& filename) :
		Filename(filename) { }
};

inline bool operator==(const ResourceParams<AtlasTexture>& lhs, const ResourceParams<AtlasTexture>& rhs)
{
	return lhs.Filename == rhs.Filename;
}

template <>
struct std::hash<ResourceParams<AtlasTexture>>
{
public:
	size_t operator()(const ResourceParams<AtlasTexture>& key) const
	{
		return std::hash<std::string>()(key.Filename);
	}
};

bool operator==(const AtlasTexture::SubTexture& lhs, const AtlasTexture::SubTexture& rhs);