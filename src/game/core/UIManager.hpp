#pragma once

#include <game/ui/InventoryUI.hpp>
#include <engine/ui/Canvas.hpp>
#include <engine/window/Window.hpp>
#include <game/ui/Crosshair.hpp>
#include <game/graphics/ConcreteItemTextureLaoder.hpp>

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
	class ItemTextureProviderSetter { //TODO: Remove it please...
	public:
		ItemTextureProviderSetter() { game::graphics::ItemTextureProvider::SetLoader(&m_ItemTextureLoader); }
	private:
		game::graphics::ConcreteItemTextureLoader m_ItemTextureLoader;
	};

	Canvas m_Canvas;
	window::Window& m_Window;

	ItemTextureProviderSetter m_Itps;
	InventoryUI m_Inventory;
	Hotbar m_Hotbar;
	Crosshair m_Crosshair;
};