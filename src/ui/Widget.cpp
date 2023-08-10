#include <ui/Widget.hpp>

Widget::Widget(WidgetParent& parent, glm::ivec2 position, glm::ivec2 size) :
	m_Parent(&parent),
	m_RelativePosition(position),
	m_Size(size),
	m_Anchor(AnchorPoint::LeftTop),
	m_RelativeTo(AnchorPoint::LeftTop),
	m_Visible(true)
{
	m_Parent->AddWidget(this);
	UpdatePosition();
}

Widget::~Widget()
{
	m_Parent->RemoveWidget(this);
}

Widget::Widget(const Widget& other) :
	m_Parent(other.m_Parent),
	m_Anchor(other.m_Anchor),
	m_RelativeTo(other.m_RelativeTo),
	m_OnHover(other.m_OnHover),
	m_OnClick(other.m_OnClick),
	m_Visible(other.m_Visible),
	m_RelativePosition(other.m_RelativePosition),
	m_FinalPosition(other.m_FinalPosition),
	m_Size(other.m_Size),
	m_ModelMatrix(other.m_ModelMatrix)
{
	m_Parent->AddWidget(this);
}

Widget& Widget::operator=(const Widget& other)
{
	if (this == &other)
		return *this;

	if (m_Parent != other.m_Parent)
	{
		m_Parent->RemoveWidget(this);
		m_Parent = other.m_Parent;
		m_Parent->AddWidget(this);
	}

	m_Anchor = other.m_Anchor;
	m_RelativeTo = other.m_RelativeTo;
	m_OnHover = other.m_OnHover;
	m_OnClick = other.m_OnClick;
	m_Visible = other.m_Visible;
	m_RelativePosition = other.m_RelativePosition;
	m_FinalPosition = other.m_FinalPosition;
	m_Size = other.m_Size;
	m_ModelMatrix = other.m_ModelMatrix;

	return *this;
}

void Widget::SetPosition(glm::ivec2 position)
{
	m_RelativePosition = position;
	UpdatePosition();
}

void Widget::SetSize(glm::ivec2 size)
{
	m_Size = size;
	UpdatePosition();
}

void Widget::UpdatePosition()
{
	m_FinalPosition = m_Parent->GetAsParentPositition() + m_RelativePosition;
	switch (m_Anchor)
	{
	case AnchorPoint::CenterTop:
	case AnchorPoint::Center:
	case AnchorPoint::CenterBottom:
		m_FinalPosition.x -= m_Size.x / 2;
		break;
	case AnchorPoint::RightTop:
	case AnchorPoint::RightCenter:
	case AnchorPoint::RightBottom:
		m_FinalPosition.x -= m_Size.x;
		break;
	}

	switch (m_Anchor)
	{
	case AnchorPoint::LeftCenter:
	case AnchorPoint::Center:
	case AnchorPoint::RightCenter:
		m_FinalPosition.y -= m_Size.y / 2;
		break;
	case AnchorPoint::LeftBottom:
	case AnchorPoint::CenterBottom:
	case AnchorPoint::RightBottom:
		m_FinalPosition.y -= m_Size.y;
		break;
	}

	glm::ivec2 parentSize = m_Parent->GetAsParentSize();
	switch (m_RelativeTo)
	{
	case AnchorPoint::CenterTop:
	case AnchorPoint::Center:
	case AnchorPoint::CenterBottom:
		m_FinalPosition.x += parentSize.x / 2;
		break;
	case AnchorPoint::RightTop:
	case AnchorPoint::RightCenter:
	case AnchorPoint::RightBottom:
		m_FinalPosition.x += parentSize.x;
		break;
	}

	switch (m_RelativeTo)
	{
	case AnchorPoint::LeftCenter:
	case AnchorPoint::Center:
	case AnchorPoint::RightCenter:
		m_FinalPosition.y += parentSize.y / 2;
		break;
	case AnchorPoint::LeftBottom:
	case AnchorPoint::CenterBottom:
	case AnchorPoint::RightBottom:
		m_FinalPosition.y += parentSize.y;
		break;
	}

	updateModelMatrix();
}

void Widget::SetRelativeTo(AnchorPoint relativeTo)
{
	m_RelativeTo = relativeTo;
	UpdatePosition();
}

void Widget::SetAnchor(AnchorPoint anchor)
{
	m_Anchor = anchor;
	UpdatePosition();
}

void Widget::HandleMouseEvent(const MouseEvent& event)
{
	switch (event.Type)
	{
	case MouseEvent::MouseEnter:
		m_OnHover = true;
		break;
	case MouseEvent::MouseExit:
		m_OnHover = false;
		break;
	case MouseEvent::MouseClick:
		m_OnClick = true;
		break;
	case MouseEvent::MouseRelease:
		m_OnClick = false;
		break;
	}

	if (!m_Visible)
		return;

	handleMouseEvent(event);
}

void Widget::Render(WidgetRenderParams& params)
{
	if (!m_Visible)
		return;
	render(params);
}

bool Widget::Contains(glm::ivec2 position) const
{
	return m_FinalPosition.x < position.x &&
		m_FinalPosition.x + m_Size.x > position.x &&
		m_FinalPosition.y < position.y &&
		m_FinalPosition.y + m_Size.y > position.y;
}

void Widget::updateModelMatrix()
{
	m_ModelMatrix = glm::translate(glm::mat4(1), glm::vec3(m_FinalPosition.x, m_FinalPosition.y, 0));
}