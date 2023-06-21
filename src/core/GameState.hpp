#pragma once

#include <core/Renderer.hpp>
#include <graphics/ResourceManager.hpp>

struct GameState
{
public:
	GameState(Renderer& renderer, ResourceManager& resourceManager);
	~GameState() = default;
	GameState(const GameState&) = delete;
	GameState& operator=(const GameState&) = delete;

	inline Renderer& GetRenderer();
	inline ResourceManager& GetResourceManager();
private:
	Renderer& m_Renderer;
	ResourceManager& m_ResourceManager;
};

Renderer& GameState::GetRenderer()
{
	return m_Renderer;
}

ResourceManager& GameState::GetResourceManager()
{
	return m_ResourceManager;
}