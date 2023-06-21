#include <core/GameState.hpp>

GameState::GameState(Renderer& renderer, ResourceManager& resourceManager) :
	m_Renderer(renderer),
	m_ResourceManager(resourceManager)
{
}