#include <core/UIManager.hpp>


UIManager::UIManager(glm::ivec2 windowSize, Inventory& inventory) :
	m_Canvas(windowSize),
	m_Inventory(m_Canvas, inventory)
{

}

void UIManager::Update()
{
	m_Inventory.Update();
}