#include <graphics/ResourceManager.hpp>

ResourceManager* ResourceManager::s_Instance = nullptr;

ResourceManager::ResourceManager()
{
	s_Instance = this;
}

ResourceManager::~ResourceManager()
{
	s_Instance = nullptr;
}

ResourceManager& ResourceManager::GetInstance()
{
	assert(s_Instance != nullptr);
	return *s_Instance;
}

ShaderProgram* ResourceManager::GetShaderProgram(std::string sourceFile)
{
	if (m_ShaderPrograms.find(sourceFile) == m_ShaderPrograms.end())
		m_ShaderPrograms[sourceFile] = std::make_unique<ShaderProgram>(sourceFile);

	return m_ShaderPrograms[sourceFile].get();
}

AtlasTexture* ResourceManager::GetAtlasTexture(std::string name)
{
	if (m_AtlasTextures.find(name) == m_AtlasTextures.end())
		m_AtlasTextures[name] = std::make_unique<AtlasTexture>(name);

	return m_AtlasTextures[name].get();
}

ImageTexture* ResourceManager::GetImageTexture(std::string fileName)
{
	if (m_ImageTextures.find(fileName) == m_ImageTextures.end())
		m_ImageTextures[fileName] = std::make_unique<ImageTexture>(fileName, true);

	return m_ImageTextures[fileName].get();
}