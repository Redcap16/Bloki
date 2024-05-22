#include <game/ui/ItemSlot.hpp>

ItemSlot::ItemSlot(WidgetParent& parent, glm::ivec2 position, MouseHolder& mouseHolder, ItemStack& itemStack) :
	Widget(parent, position, ItemQuad::c_Size),
	m_MouseHolder(mouseHolder),
	m_Stack(itemStack),
	m_Background(glm::ivec2(0), ItemQuad::c_Size, c_BackgroundColor)
{
	m_Picture.SetItem(&m_Stack);
}


void ItemSlot::handleMouseEvent(const MouseEvent& event) {
	if (event.Type == MouseEvent::MouseRelease &&
		m_OnHover)
	{
		ItemStack& mouseStack = m_MouseHolder.GetStack();
		if (event.ButtonValue == MouseEvent::Button::LMB) {
			if (!mouseStack.Empty() && !m_Stack.Empty() &&
				mouseStack.GetItemHeld() == m_Stack.GetItemHeld()) {
				mouseStack.MoveTo(m_Stack);
			}
			else
				mouseStack.SwapContents(m_Stack);
		}
		else {
			if (!mouseStack.Empty() &&
				(m_Stack.Empty() || mouseStack.GetItemHeld() == m_Stack.GetItemHeld())) {
				mouseStack.MoveTo(m_Stack, 1);
			}
			else if (!m_Stack.Empty() && mouseStack.Empty())
				m_Stack.MoveTo(mouseStack, m_Stack.GetCount() / 2);

		}
	}
	if (event.Type == MouseEvent::MouseEnter)
		setHighlighted(true);
	else if (event.Type == MouseEvent::MouseExit)
		setHighlighted(false);
}

void ItemSlot::render(RenderingParams& params)
{
	params.SetColoringType(ColoringType::Color);
	m_Background.Render();

	m_Picture.Render(params);
}

void ItemSlot::setHighlighted(bool highlighted) {
	m_Background.SetColor(highlighted ? c_BackgroundHighlightColor : c_BackgroundColor);
}