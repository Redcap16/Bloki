#pragma once

#include <game/ui/InventoryUI.hpp>
#include <engine/ui/Canvas.hpp>
#include <engine/window/Window.hpp>
#include <game/ui/Crosshair.hpp>

class UIManager
{
public:
	UIManager(window::Window& window, Inventory& inventory);

	void ShowInventory(bool visible);
	bool IsInventoryVisible() const { return m_Inventory.IsVisible(); }

	void SetWindowSize(glm::ivec2 windowSize) { m_Canvas.SetWindowSize(windowSize); }

	void MouseClicked(bool LMB) { m_Canvas.MouseClicked(LMB); }
	void MouseReleased(bool LMB) { m_Canvas.MouseReleased(LMB); }
	void MouseMoved(glm::ivec2 position) { m_Canvas.MouseMoved(position); }

	void Render() { m_Canvas.Render(); }

	void Update();
private:
	Canvas m_Canvas;
	window::Window& m_Window;

	InventoryUI m_Inventory;
	Hotbar m_Hotbar;
	Crosshair m_Crosshair;
};