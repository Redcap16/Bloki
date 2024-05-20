#pragma once

#include <glm/glm.hpp>
#include <graphics/VertexArray.hpp>
#include <graphics/Vertex.hpp>
#include <graphics/ImageTexture.hpp>
#include <graphics/Rectangle.hpp>
#include <graphics/AtlasTexture.hpp>
#include <ui/Group.hpp>
#include <items/Item.hpp>
#include <items/Inventory.hpp>
#include <ui/Text.hpp>

class ItemQuad : public StackUpdateListener
{
public:
	static const constexpr glm::ivec2 c_Size = { 64, 64 };

	ItemQuad();
	ItemQuad(ItemStack* stack);
	~ItemQuad();
	ItemQuad(const ItemQuad&) = delete;
	ItemQuad& operator=(const ItemQuad&) = delete;
	ItemQuad(ItemQuad&& other) noexcept;
	ItemQuad& operator=(ItemQuad&& other) noexcept;

	void SetItem(ItemStack* stack);
	void StackUpdated() override;

	void Render(RenderingParams& shader) const;

	static glm::vec2 GetItemTextureSize();
	static glm::vec2 GetItemTexturePosition(const Item& item);
private:
	ItemStack *m_Stack;
	
	graphics::Rectangle m_Rectangle;

	static constexpr const char* c_TextureFilepath = "items.td";
	Resource<AtlasTexture> m_Texture;

	void update();

	const std::string c_FontFile = "DejaVuSerif.ttf";
	const int c_FontSize = 25;
	const glm::ivec2 c_CountTextPosition = c_Size - glm::ivec2(30, 5);
	const glm::ivec3 c_CountTextColor = glm::ivec3(240, 240, 240);
	Resource<Font> m_Font;
	Text m_CountText;
};