#pragma once

#include <graphics/VertexArray.hpp>
#include <graphics/Vertex.hpp>
#include <graphics/ImageTexture.hpp>
#include <graphics/Rectangle.hpp>
#include <ui/Group.hpp>
#include <items/Inventory.hpp>
#include <graphics/AtlasTexture.hpp>

class ItemQuad
{
public:
	static const constexpr glm::ivec2 c_Size = { 64, 64 };
	
	ItemQuad();
	~ItemQuad() = default;
	ItemQuad(const ItemQuad&) = delete;
	ItemQuad& operator=(const ItemQuad&) = delete;
	ItemQuad(ItemQuad&& other) noexcept;
	ItemQuad& operator=(ItemQuad&& other) noexcept;

	void SetItem(const ItemStack& stack);

	void Render(RenderingParams& shader) const;
private:
	ItemStack m_Stack;
	graphics::Rectangle m_Rectangle;

	static std::array<AtlasTexture::SubTexture, Item::c_ItemCount> m_TextureCoords; //TODO: Change atlas textures to operate on ids, not on names
	static constexpr const char* c_TextureFilepath = "items.td";
	Resource<AtlasTexture> m_Texture;

	void update();
	void loadTextureCoords();
};

class MouseHolder : public Widget
{
public:
	MouseHolder(WidgetParent& parent, glm::ivec2 position);

	const ItemStack& GetStack() const { return m_Stack; }
	void SetStack(const ItemStack& stack);

protected:
	void handleMouseEvent(const MouseEvent& event) override;
	void render(RenderingParams& params) override;

private:
	ItemStack m_Stack;
	ItemQuad m_Mesh;
};

class ItemSlot : public Widget
{
public:
	ItemSlot(WidgetParent& parent, glm::ivec2 position, MouseHolder& mouseHolder, Inventory& inventory, int index);
	~ItemSlot() = default;
	ItemSlot(const ItemSlot&) = delete;
	ItemSlot& operator=(const ItemSlot&) = delete;
	ItemSlot(ItemSlot&& other) noexcept = default;
	ItemSlot& operator=(ItemSlot&& other) noexcept = default;

	void Update(const ItemStack& stack);

protected:
	void handleMouseEvent(const MouseEvent& event) override;
	void render(RenderingParams& params) override;

private:
	Inventory* m_Inventory;

	ItemStack m_Stack;
	ItemQuad m_Mesh;

	MouseHolder* m_MouseHolder;
	int m_Index;
};

class ItemPicture : public Widget
{
public:
	ItemPicture(WidgetParent& parent, glm::ivec2 position);

	void SetItem(const ItemStack& stack);

protected:
	void handleMouseEvent(const MouseEvent& event) override { };
	void render(RenderingParams& params) override;

	ItemQuad m_Mesh;
};

#include <ui/Text.hpp>

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

	Font m_Font;
	Text m_Test;
};

class InventoryUI : public WidgetGroup, public InventoryUpdateListener
{
public:
	InventoryUI(WidgetParent& parent, Inventory& inventory);
	~InventoryUI() = default;
	InventoryUI(const InventoryUI&) = delete;
	InventoryUI& operator=(const InventoryUI&) = delete;

	void Update();
	void Updated(int index) override;

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
	std::vector<ItemSlot> m_Slots;

	std::unique_ptr<MouseHolder> m_MouseHolder;
	InventoryBackground m_Background;

	void prepareSlots();
};

class Hotbar : public WidgetGroup, public InventoryUpdateListener
{
public:
	Hotbar(WidgetParent& parent, Inventory& inventory);

	void Updated(int index);
private:
	static constexpr const char* c_TextureFilename = "hotbar.png";
	static constexpr glm::ivec2 c_Position = { 0, 30 };
	static const int c_ItemCount = 9;

	Inventory& m_Inventory;
	std::vector<ItemPicture> m_Slots;

	PictureBox m_Background;

	void prepareSlots();
};