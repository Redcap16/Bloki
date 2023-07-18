#pragma once

#include <vector>
#include <stb_image/stb_image.h>
#include <graphics/Texture.hpp>

class CubemapTexture : public Texture
{
public:
	CubemapTexture(const std::string& filename);
	~CubemapTexture();
	CubemapTexture(const CubemapTexture&) = delete;
	CubemapTexture& operator=(const CubemapTexture&) = delete;

	void Bind(GLuint textureUnit) const override;
	inline TextureHandle GetHandle() const override;

private:
	TextureHandle m_Handle;
	std::string m_Filename;

	void load();
	void unload();
	void getSide(unsigned int height, 
		unsigned char* data, 
		unsigned short side, 
		std::vector<unsigned char>& result);
};

TextureHandle CubemapTexture::GetHandle() const
{
	return m_Handle;
}