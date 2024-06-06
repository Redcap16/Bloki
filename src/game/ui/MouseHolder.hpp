#pragma once

#include <engine/ui/Widget.hpp>
#include <game/ui/ItemQuad.hpp>
#include <game/items/Inventory.hpp>

class MouseHolder : public Widget
{
public:
	MouseHolder(WidgetParent& parent, glm::ivec2 position);

	ItemStack& GetStack() { return m_Stack; }
	const ItemStack& GetStack() const { return m_Stack; }

protected:
	void handleMouseEvent(const MouseEvent& event) override;
	void render(RenderingParams& params) override;

private:
	ItemStack m_Stack;
	ItemQuad m_Picture;
};