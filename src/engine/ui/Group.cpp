#include <engine/ui/Group.hpp>

WidgetGroup::WidgetGroup(WidgetParent& parent, glm::ivec2 position, glm::ivec2 size) :
	Widget(parent, position, size)
{

}

void WidgetGroup::UpdatePosition()
{
	Widget::UpdatePosition();

	for (Widget* widget : m_Widgets)
		widget->UpdatePosition();
}

void WidgetGroup::handleMouseEvent(const MouseEvent& event)
{
	for (Widget* widget : m_Widgets)
	{
		if (widget->Contains(event.Position) &&
			(event.Type == MouseEvent::MouseMove ||
				event.Type == MouseEvent::MouseClick ||
				event.Type == MouseEvent::MouseRelease))
			widget->HandleMouseEvent(event);

		if (widget->IsOnHover() && !widget->Contains(event.Position))
		{
			MouseEvent newEvent = event;
			newEvent.Type = MouseEvent::MouseExit;
			widget->HandleMouseEvent(newEvent);
		}
		else if (!widget->IsOnHover() && widget->Contains(event.Position))
		{
			MouseEvent newEvent = event;
			newEvent.Type = MouseEvent::MouseEnter;
			widget->HandleMouseEvent(newEvent);
		}

		if (widget->IsOnClick() && !widget->Contains(event.Position))
		{
			MouseEvent newEvent = event;
			newEvent.Type = MouseEvent::MouseRelease;
			widget->HandleMouseEvent(newEvent);
		}
	}
}

void WidgetGroup::render(RenderingParams& params)
{
	for (Widget* widget : m_Widgets)
	{
		params.SetModelMatrix(widget->GetModelMatrix());

		widget->Render(params);
	}
}

void WidgetGroup::RemoveWidget(const Widget* widget)
{
	std::vector<Widget*>::const_iterator it = std::find(m_Widgets.begin(), m_Widgets.end(), widget);
	if (it != m_Widgets.end())
		m_Widgets.erase(it);
}