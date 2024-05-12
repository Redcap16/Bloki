#include <core/UIManager.hpp>


UIManager::UIManager(Window& window, Inventory& inventory) :
	m_Canvas(window.GetSize()),
	m_Inventory(m_Canvas, inventory),
	m_Hotbar(m_Canvas, inventory, window),
	m_Window(window),
	m_Crosshair(m_Canvas)
{
	m_Window.GetMouse().SetCursorVisible(false);
}

void UIManager::ShowInventory(bool visible)
{
	m_Crosshair.SetVisible(!visible);
	m_Inventory.SetVisible(visible);
	m_Hotbar.SetVisible(!visible);
	m_Window.GetMouse().SetCursorVisible(visible);
}

void UIManager::Update()
{
	m_Inventory.Update();
}