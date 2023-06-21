#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <string>

#include <graphics/ShaderProgram.hpp>

class Texture
{
private:
	static const short maxTextureIndex = 16;
protected:
	const std::string texturePath = "assets/textures/";
	unsigned int texture = 0;
	Texture();
public:
	void BindTexture(unsigned short index);
	inline unsigned int GetTextureID()
	{
		return texture;
	}
};