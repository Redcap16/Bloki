#pragma once

#include <string>

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <glm/glm.hpp>

#include <stb_image/stb_image.h>
#include <graphics/Texture.hpp>
#include <core/Resource.hpp>
#include <util/Math.hpp>
#include <util/Debug.hpp>

class ImageTexture : public Texture
{
public:
	ImageTexture(const std::string& filename, bool transparency);
	~ImageTexture();
	ImageTexture(const ImageTexture&) = delete;
	ImageTexture& operator=(const ImageTexture&) = delete;

	glm::ivec2 GetSize() const { return m_Size; }

	void Bind(GLuint textureUnit) const override;
	inline TextureHandle GetHandle() const override;
private:
	TextureHandle m_Handle;
	glm::ivec2 m_Size;
	std::string m_Filename;

	void load(bool transparency);
	void unload();
};

TextureHandle ImageTexture::GetHandle() const
{
	return m_Handle;
}

template <>
struct ResourceParams<ImageTexture>
{
	std::string Filename;
	bool Transparency;

	ResourceParams(const std::string& filename, bool transparency) :
		Filename(filename),
		Transparency(transparency) {}
};

inline bool operator==(const ResourceParams<ImageTexture>& lhs, const ResourceParams<ImageTexture>& rhs)
{
	return lhs.Filename == rhs.Filename &&
		lhs.Transparency == lhs.Transparency;
}

template <>
class std::hash<ResourceParams<ImageTexture>>
{
public:
	size_t operator()(const ResourceParams<ImageTexture>& key) const
	{
		size_t result = 0;
		hash_combine(result, key.Filename, key.Transparency);
		return result;
	}
};