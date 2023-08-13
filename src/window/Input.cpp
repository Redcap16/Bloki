#include <window/Input.hpp>

Mouse::Mouse(const HWND& windowHandle) :
	m_WindowHandle(windowHandle)
{

}

void Mouse::SetPosition(glm::ivec2 position)
{
	if (m_WindowHandle == NULL)
		return;

	POINT xy = { position.x, position.y };
	ClientToScreen(m_WindowHandle, &xy);
	SetCursorPos(xy.x, xy.y);

	m_Position = position;
}

void Mouse::SetCursorVisible(bool visible)
{
	ShowCursor(visible ? TRUE : FALSE);
}

void Mouse::mouseMove(glm::ivec2 position)
{
	m_Position = position;
}

void Mouse::buttonPressed(Button button)
{
	m_Buttons[(int)button] = true;
}

void Mouse::buttonReleased(Button button)
{
	m_Buttons[(int)button] = false;
}

Keyboard::Key::Key(char code)
{
	assert(std::isalnum(code));
	if (std::isdigit(code))
		m_Code = code;
	else
		m_Code = std::toupper(code);
}

bool operator==(const Keyboard::Key& lhs, const Keyboard::Key& rhs)
{
	return lhs.Code() == rhs.Code();
}