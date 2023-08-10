#include <ui/Canvas.hpp>

Canvas::Canvas(glm::ivec2 windowSize) :
	m_WindowSize(windowSize),
	m_MouseState({ glm::ivec2(0, 0), false, false}),
	m_Shader(c_ShaderFile)
{
	m_HasTextureLocation = m_Shader->GetUniformLocation("HasTexture");
	updateProjectionMatrix();
}

void Canvas::SetWindowSize(glm::ivec2 windowSize)
{
	m_WindowSize = windowSize;
	for (Widget* widget : m_Widgets)
		widget->UpdatePosition();

	updateProjectionMatrix();
}

void Canvas::MouseClicked(bool LMB)
{
	if (LMB)
		m_MouseState.LMBClicked = true;
	else
		m_MouseState.RMBClicked = true;

	MouseEvent event(MouseEvent::MouseClick, LMB ? MouseEvent::Button::LMB : MouseEvent::Button::RMB, m_MouseState.LMBClicked, m_MouseState.RMBClicked, m_MouseState.Position);
	for (Widget* widget : m_Widgets)
	{
		if (widget->Contains(m_MouseState.Position))
			widget->HandleMouseEvent(event);
	}
}

void Canvas::MouseReleased(bool LMB)
{
	if (LMB)
		m_MouseState.LMBClicked = false;
	else
		m_MouseState.RMBClicked = false;

	MouseEvent event(MouseEvent::MouseRelease, LMB ? MouseEvent::Button::LMB : MouseEvent::Button::RMB, m_MouseState.LMBClicked, m_MouseState.RMBClicked, m_MouseState.Position);
	for (Widget* widget : m_Widgets)
	{
		if (widget->Contains(m_MouseState.Position))
			widget->HandleMouseEvent(event);
	}
}

void Canvas::MouseMoved(glm::ivec2 position)
{
	m_MouseState.Position = position;
	MouseEvent event(MouseEvent::MouseMove, m_MouseState.LMBClicked, m_MouseState.RMBClicked, position);
	for (Widget* widget : m_Widgets)
	{
		if (widget->Contains(position))
		{
			event.Type = MouseEvent::MouseMove;
			widget->HandleMouseEvent(event);
			if (!widget->IsOnHover())
			{
				event.Type = MouseEvent::MouseEnter;
				widget->HandleMouseEvent(event);
			}
		}
		else
		{
			if (widget->IsOnHover())
			{
				event.Type = MouseEvent::MouseExit;
				widget->HandleMouseEvent(event);
			}
			if (widget->IsOnClick())
			{
				event.Type = MouseEvent::MouseRelease;
				widget->HandleMouseEvent(event);
			}
		}
	}
}

void Canvas::Render()
{
	glDisable(GL_DEPTH_TEST);
	m_Shader->UseProgram();

	Widget::WidgetRenderParams params = { *m_Shader.Get(), m_ProjectionMatrix };
	for (Widget* widget : m_Widgets)
	{
		m_Shader->SetUniform(m_HasTextureLocation, 0);
		//m_Shader->SetMVPMatrix(m_ProjectionMatrix * widget->GetModelMatrix());

		widget->Render(params);
	}
}

void Canvas::RemoveWidget(const Widget* widget)
{
	std::vector<Widget*>::const_iterator it = std::find(m_Widgets.begin(), m_Widgets.end(), widget);
	if (it != m_Widgets.end())
		m_Widgets.erase(it);
}

void Canvas::updateProjectionMatrix()
{
	m_ProjectionMatrix = glm::ortho<float>(0, m_WindowSize.x, m_WindowSize.y, 0);
}