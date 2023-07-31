#pragma once

#include <graphics/ShaderProgram.hpp>
#include <graphics/AtlasTexture.hpp>
#include <graphics/ImageTexture.hpp>
#include <string>
#include <unordered_map>


//TODO: Seperate creating resources from accesing them.
class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	static ResourceManager& GetInstance();

	ShaderProgram* GetShaderProgram(std::string sourceFile);
	AtlasTexture* GetAtlasTexture(std::string name);
	ImageTexture* GetImageTexture(std::string fileName);

	void FreeResources();

private:
	static ResourceManager* s_Instance;

	std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> m_ShaderPrograms;
	std::unordered_map<std::string, std::unique_ptr<AtlasTexture>> m_AtlasTextures;
	std::unordered_map<std::string, std::unique_ptr<ImageTexture>> m_ImageTextures;
};