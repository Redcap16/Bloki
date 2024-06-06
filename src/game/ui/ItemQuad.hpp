#pragma once

#include <glm/glm.hpp>
#include <engine/graphics/VertexArray.hpp>
#include <engine/graphics/Vertex.hpp>
#include <engine/graphics/ImageTexture.hpp>
#include <engine/graphics/Rectangle.hpp>
#include <engine/graphics/AtlasTexture.hpp>
#include <engine/ui/Group.hpp>
#include <game/items/Item.hpp>
#include <game/items/Inventory.hpp>
#include <engine/ui/Text.hpp>

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
private:
	ItemStack *m_Stack;
	
	graphics::Rectangle m_Rectangle;
	const Texture* m_Texture;

	void update();

	const std::string c_FontFile = "DejaVuSerif.ttf";
	const int c_FontSize = 25;
	const glm::ivec2 c_CountTextPosition = c_Size - glm::ivec2(30, 5);
	const glm::ivec3 c_CountTextColor = glm::ivec3(240, 240, 240);
	Resource<Font> m_Font;
	Text m_CountText;
};