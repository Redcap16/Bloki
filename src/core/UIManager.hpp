#pragma once

#include <ui/InventoryUI.hpp>
#include <ui/Canvas.hpp>

class UIManager
{
public:
	UIManager(glm::ivec2 windowSize, Inventory& inventory);

	void ShowInventory(bool visible) { m_Inventory.SetVisible(visible); }
	bool IsInventoryVisible() const { return m_Inventory.IsVisible(); }

	void SetWindowSize(glm::ivec2 windowSize) { m_Canvas.SetWindowSize(windowSize); }

	void MouseClicked(bool LMB) { m_Canvas.MouseClicked(LMB); }
	void MouseReleased(bool LMB) { m_Canvas.MouseReleased(LMB); }
	void MouseMoved(glm::ivec2 position) { m_Canvas.MouseMoved(position); }

	void Render() { m_Canvas.Render(); }

	void Update();
private:
	Canvas m_Canvas;

	InventoryUI m_Inventory;
};