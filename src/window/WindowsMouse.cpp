#include <window/WindowsMouse.hpp>

namespace window {
	WindowsMouse::WindowsMouse(const HWND& windowHandle) :
		m_WindowHandle(windowHandle),
		m_Position(0)
	{

	}

	void WindowsMouse::MouseMoved(glm::ivec2 position) {
		m_Position = position;
		for (auto listener : m_Listeners)
			listener->OnMouseMove(position);
	}

	void WindowsMouse::ButtonPressed(Button button) {
		m_Buttons[(int)button] = true;

		MouseButtonEvent event = { MouseButtonEvent::Type::Pressed, button };
		for (auto listener : m_Listeners)
			listener->OnMouseButtonEvent(event);
	}

	void WindowsMouse::ButtonReleased(Button button) {
		m_Buttons[(int)button] = false;

		MouseButtonEvent event = { MouseButtonEvent::Type::Released, button };
		for (auto listener : m_Listeners)
			listener->OnMouseButtonEvent(event);
	}

	void WindowsMouse::SetPosition(glm::ivec2 position)
	{
		if (m_WindowHandle == NULL)
			return;

		POINT xy = { position.x, position.y };
		ClientToScreen(m_WindowHandle, &xy);
		SetCursorPos(xy.x, xy.y);

		m_Position = position;
	}

	void WindowsMouse::SetCursorVisible(bool visible)
	{
		ShowCursor(visible ? TRUE : FALSE);
	}
};