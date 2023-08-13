#pragma once

#include <glm/glm.hpp>

#include <cctype>
#include <array>

#include <windows.h>

class Window;

class Mouse
{
public:
	friend class Window;

	enum class Button
	{
		Left,
		Right
	};
	static const int c_ButtonCount = 2;

	Mouse(const HWND& windowHandle);

	void SetPosition(glm::ivec2 position);
	glm::ivec2 GetPosition() const { return m_Position; };
	bool IsButtonPressed(Button button) const { return m_Buttons[(int)button]; };
	void SetCursorVisible(bool visible);

private:
	const HWND& m_WindowHandle;
	glm::ivec2 m_Position;

	std::array<bool, 2> m_Buttons;

	void mouseMove(glm::ivec2 position);
	void buttonPressed(Button button);
	void buttonReleased(Button button);
};

class Keyboard
{
public:
	friend class Window;

	struct Key
	{
		enum Code
		{
			Backspace = VK_BACK,
			Tab = VK_TAB,
			Enter = VK_RETURN,
			Shift = VK_SHIFT,
			Control = VK_CONTROL,
			Alt = VK_MENU,
			Capslock = VK_CAPITAL,
			Escape = VK_ESCAPE,
			Spacebar = VK_SPACE,
			PageUp = VK_PRIOR,
			PageDown = VK_NEXT,
			End = VK_END,
			Home = VK_HOME,
			Left = VK_LEFT,
			Up = VK_UP,
			Right = VK_RIGHT,
			Down = VK_DOWN,
			PrintScreen = VK_SNAPSHOT,
			Delete = VK_DELETE,
			F1 = VK_F1,
			F2 = VK_F2,
			F3 = VK_F3,
			F4 = VK_F4,
			F5 = VK_F5,
			F6 = VK_F6,
			F7 = VK_F7,
			F8 = VK_F8,
			F9 = VK_F9,
			F10 = VK_F10,
			F11 = VK_F11,
			F12 = VK_F12,
			NumLock = VK_NUMLOCK,
			ScrollLock = VK_SCROLL
		};
		Key(Code code) : m_Code(code) { }
		Key(char code);
		Key(WPARAM code) : m_Code((char)code) { }

		unsigned char Code() const { return m_Code; }
	private:
		unsigned char m_Code;
	};
	static const int c_KeyCount = 0xFF;

	bool GetKeyState(Key key) const { return m_KeyStates[key.Code()]; }

private:
	std::array<bool, c_KeyCount> m_KeyStates;

	void keyPressed(Key key) { m_KeyStates[key.Code()] = true; }
	void keyReleased(Key key) { m_KeyStates[key.Code()] = false; }
};

bool operator==(const Keyboard::Key& lhs, const Keyboard::Key& rhs);