#pragma once

#include <engine/ui/Widget.hpp>

#include <vector>

class WidgetGroup : public Widget, public WidgetParent
{
public:
	WidgetGroup(WidgetParent& parent, glm::ivec2 position, glm::ivec2 size);
	~WidgetGroup() = default;
	WidgetGroup(const WidgetGroup&) = delete;
	WidgetGroup& operator=(const WidgetGroup&) = delete;

	void UpdatePosition() override;

protected:
	std::vector<Widget*> m_Widgets;

	glm::ivec2 GetAsParentPositition() const override { return m_FinalPosition; };
	glm::ivec2 GetAsParentSize() const override { return m_Size; };

	void AddWidget(Widget* widget) override { m_Widgets.push_back(widget); };
	void RemoveWidget(const Widget* widget) override;

	void handleMouseEvent(const MouseEvent& event) override;
	void render(RenderingParams& shader) override;
};