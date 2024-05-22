#pragma once

#include <game/ui/MouseHolder.hpp>

MouseHolder::MouseHolder(WidgetParent& parent, glm::ivec2 position) :
	Widget(parent, position, ItemQuad::c_Size)
{
	SetAnchor(AnchorPoint::Center);
	m_Picture.SetItem(&m_Stack);
}

void MouseHolder::handleMouseEvent(const MouseEvent& event) {

}

void MouseHolder::render(RenderingParams& params)
{
	if (m_Stack.Empty())
		return;

	m_Picture.Render(params);
}