#pragma once

#include <graphics/ShaderProgram.hpp>
#include <graphics/AtlasTexture.hpp>
#include <graphics/ImageTexture.hpp>
#include <string>
#include <unordered_map>

class ResourceManager
{
public:
	ShaderProgram* GetShaderProgram(std::string sourceFile);
	AtlasTexture* GetAtlasTexture(std::string name);
	ImageTexture* GetImageTexture(std::string fileName);

private:
	std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> m_ShaderPrograms;
	std::unordered_map<std::string, std::unique_ptr<AtlasTexture>> m_AtlasTextures;
	std::unordered_map<std::string, std::unique_ptr<ImageTexture>> m_ImageTextures;
};