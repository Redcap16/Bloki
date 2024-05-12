#pragma once

#include <ui/Widget.hpp>
#include <ui/ItemQuad.hpp>
#include <ui/MouseHolder.hpp>

class ItemSlot : public Widget
{
public:
	ItemSlot(WidgetParent& parent, glm::ivec2 position, MouseHolder& mouseHolder, ItemStack& itemStack);
	~ItemSlot() = default;
	ItemSlot(const ItemSlot&) = delete;
	ItemSlot& operator=(const ItemSlot&) = delete;
	ItemSlot(ItemSlot&& other) noexcept = default;
	ItemSlot& operator=(ItemSlot&& other) noexcept = default;

protected:
	void handleMouseEvent(const MouseEvent& event) override;
	void render(RenderingParams& params) override;

	void setHighlighted(bool highlighted);

private:
	ItemStack& m_Stack;
	ItemQuad m_Picture;

	const glm::ivec4 c_BackgroundColor = { 100, 100, 100, 255 };
	const glm::ivec4 c_BackgroundHighlightColor = { 130, 130, 130, 255 };
	graphics::Rectangle m_Background;

	MouseHolder& m_MouseHolder;
};