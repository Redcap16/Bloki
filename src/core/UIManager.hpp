#pragma once

#include <ui/InventoryUI.hpp>
#include <ui/Canvas.hpp>
#include <window/Window.hpp>
#include <ui/Crosshair.hpp>

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