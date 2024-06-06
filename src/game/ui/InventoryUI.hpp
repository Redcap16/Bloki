#pragma once

#include <engine/graphics/VertexArray.hpp>
#include <engine/graphics/Vertex.hpp>
#include <engine/graphics/ImageTexture.hpp>
#include <engine/graphics/Rectangle.hpp>
#include <engine/ui/Group.hpp>
#include <game/items/Inventory.hpp>
#include <game/ui/ItemQuad.hpp>
#include <game/ui/ItemSlot.hpp>
#include <engine/window/Window.hpp>

class ItemPicture : public Widget
{
public:
	ItemPicture(WidgetParent& parent, glm::ivec2 position, ItemStack& stack);
	void SetHighlighted(bool highlighted);

protected:
	void handleMouseEvent(const MouseEvent& event) override { };
	void render(RenderingParams& params) override;

	const glm::ivec4 c_DefaultBackgroundColor = { 80, 80, 80, 130 }, 
		c_HighlightedBackgroundColor = { 50, 50, 50, 130 };
	const int c_HighlightedBackgroundSizeIncrease = 5;
	graphics::Rectangle m_Background;
	ItemStack& m_Stack;
	ItemQuad m_Picture;
};

#include <engine/ui/Text.hpp>

class PictureBox : public Widget
{
public:
	PictureBox(WidgetParent& parent, std::string filename, glm::ivec2 position, glm::ivec2 size = { 0, 0 });

	glm::ivec2 GetOriginalSize() { return m_Texture->GetSize(); };
public:
	void handleMouseEvent(const MouseEvent& event) override { };
	void render(RenderingParams& params) override;

	Resource<ImageTexture> m_Texture;
	graphics::Rectangle m_Rectangle;
};

class InventoryUI : public WidgetGroup
{
public:
	InventoryUI(WidgetParent& parent, Inventory& inventory);
	~InventoryUI() = default;
	InventoryUI(const InventoryUI&) = delete;
	InventoryUI& operator=(const InventoryUI&) = delete;

	void Update();

protected:
	void handleMouseEvent(const MouseEvent& event) override;

private:
	class InventoryBackground : public Widget
	{
	public:
		InventoryBackground(InventoryUI& inventory);
		~InventoryBackground() = default;
		InventoryBackground(const InventoryBackground&) = delete;
		InventoryBackground& operator=(const InventoryBackground&) = delete;

	protected:
		void handleMouseEvent(const MouseEvent& event) override { };
		void render(RenderingParams& params) override;

	private:
		static constexpr const char* c_TextureFilename = "inventory.png";
		Resource<ImageTexture> m_Texture;

		graphics::Rectangle m_Rectangle;
	};

	Inventory& m_Inventory;
	std::array<std::unique_ptr<ItemSlot>, Inventory::c_Size> m_Slots;

	std::unique_ptr<MouseHolder> m_MouseHolder;
	InventoryBackground m_Background;

	void prepareSlots();
};

class Hotbar : public WidgetGroup, public window::KeyboardListener
{
public:
	Hotbar(WidgetParent& parent, Inventory& inventory, window::Window& window);
	~Hotbar();

	void OnKeyboardEvent(const window::KeyboardEvent& event) override;

private:
	static constexpr const char* c_TextureFilename = "hotbar.png";
	static constexpr glm::ivec2 c_Position = { 0, 30 };
	static const int c_ItemCount = 9;

	window::Window& m_Window;
	Inventory& m_Inventory;
	std::array<std::unique_ptr<ItemPicture>, c_ItemCount> m_Slots;

	PictureBox m_Background;

	void prepareSlots();
};