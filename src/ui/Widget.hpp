#pragma once

#include <glm/glm.hpp>
#include <graphics/ShaderProgram.hpp>
#include <ui/UIRender.hpp>

enum class AnchorPoint
{
	LeftTop,
	CenterTop,
	RightTop,
	LeftCenter,
	Center,
	RightCenter,
	LeftBottom,
	CenterBottom,
	RightBottom
};

struct MouseEvent
{
	enum EventType
	{
		MouseEnter,
		MouseExit,
		MouseMove,
		MouseClick,
		MouseRelease //It gets invoked when mouse button is released while hovering on widget OR when mouse exits boundaries of given widget. It can be distinguished by the value of OnHover. It'll be true in first case and false in the second one
	};

	enum class Button
	{
		LMB,
		RMB
	};

	EventType Type;
	Button ButtonValue;
	bool LMBPressed,
		RMBPressed;
	glm::ivec2 Position;

	MouseEvent(EventType type, bool lmb, bool rmb, glm::ivec2 position) : 
		Type(type),
		LMBPressed(lmb),
		RMBPressed(rmb),
		Position(position),
		ButtonValue(Button::LMB) {}

	MouseEvent(EventType type,Button button, bool lmb, bool rmb, glm::ivec2 position) :
		Type(type),
		LMBPressed(lmb),
		RMBPressed(rmb),
		Position(position),
		ButtonValue(button) {}
};

class Widget;

class WidgetParent
{
protected:
	friend class Widget;

	virtual glm::ivec2 GetAsParentPositition() const = 0;
	virtual glm::ivec2 GetAsParentSize() const = 0;

	virtual void AddWidget(Widget* widget) = 0;
	virtual void RemoveWidget(const Widget* widget) = 0;
};

class Widget
{
public:
	

	Widget(WidgetParent& parent, glm::ivec2 position, glm::ivec2 size);
	~Widget();
	Widget(const Widget& other);
	Widget& operator=(const Widget& other);

	void SetPosition(glm::ivec2 position);
	void SetSize(glm::ivec2 size);

	glm::ivec2 GetParentPosition() { return m_Parent->GetAsParentPositition(); }
	glm::ivec2 GetParentSize() { return m_Parent->GetAsParentSize(); }

	glm::ivec2 GetPosition() const { return m_RelativePosition; }
	glm::ivec2 GetSize() const { return m_Size; }

	const glm::mat4& GetModelMatrix() const { return m_ModelMatrix; }

	virtual void UpdatePosition();
	void SetRelativeTo(AnchorPoint relativeTo);
	void SetAnchor(AnchorPoint anchor);

	void HandleMouseEvent(const MouseEvent& event);
	void Render(RenderingParams& params);

	bool Contains(glm::ivec2 position) const;

	bool IsOnHover() const { return m_OnHover; }
	bool IsOnClick() const { return m_OnClick; }

	void SetVisible(bool visible) { m_Visible = visible; }
	bool IsVisible() const { return m_Visible; }
protected:

	WidgetParent* m_Parent;
	AnchorPoint m_Anchor,
		m_RelativeTo;

	bool m_OnHover,
		m_OnClick,
		m_Visible;
	glm::ivec2 m_RelativePosition,
		m_FinalPosition, //Position of top left corner, relative to the window top left corner
		m_Size;

	virtual void handleMouseEvent(const MouseEvent& event) = 0;
	virtual void render(RenderingParams& params) = 0;

private:
	glm::mat4 m_ModelMatrix;

	void updateModelMatrix();
};