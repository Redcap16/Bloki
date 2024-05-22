#pragma once

#include <glm/glm.hpp>
#include <set>
#include <cctype>
#include <assert.h>

#define NOMINMAX
#include <windows.h>

namespace window {
	class MouseListener;

	class Mouse { 
	public:
		enum class Button
		{
			Left,
			Middle,
			Right
		};
		static const int c_ButtonCount = 3;

		virtual ~Mouse() {};

		virtual void AddMouseListener(MouseListener& listener) { m_Listeners.insert(&listener); };
		virtual void RemoveMouseListener(MouseListener& listener) { m_Listeners.erase(&listener); };

		virtual void SetPosition(glm::ivec2 position) = 0;
		virtual glm::ivec2 GetPosition() const = 0;
		virtual bool IsButtonPressed(Button button) const = 0;
		virtual void SetCursorVisible(bool visible) = 0;

	protected:
		std::set<MouseListener*> m_Listeners;
	};

	struct MouseButtonEvent
	{
		enum Type
		{
			Pressed,
			Released
		};

		Type EventType;
		Mouse::Button ButtonType;
	};

	class MouseListener {
	public:
		virtual void OnMouseButtonEvent(const MouseButtonEvent& event) = 0;
		virtual void OnMouseMove(glm::ivec2 position) = 0;
		virtual void OnMouseWheelMove(int movement) = 0;
	};

	class KeyboardListener;

	class Keyboard {
	public:
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
			bool IsChar() const;
			char GetCharValue() const;
		private:
			unsigned char m_Code;
		};

		virtual ~Keyboard() {};

		void AddKeyboardListener(KeyboardListener& listener) { m_Listeners.insert(&listener); }
		void RemoveKeyboardListener(KeyboardListener& listener) { m_Listeners.erase(&listener); }

		virtual bool GetKeyState(Key key) const = 0;

		static const int c_KeyCount = 0xFF;

	protected:
		std::set<KeyboardListener*> m_Listeners;
	};

	bool operator==(const Keyboard::Key& lhs, const Keyboard::Key& rhs);

	struct KeyboardEvent
	{
		enum Type
		{
			KeyPressed,
			KeyReleased
		};

		Type EventType;
		Keyboard::Key KeyCode;
		int RepeatCount;
	};

	class KeyboardListener
	{
	public:
		virtual void OnKeyboardEvent(const KeyboardEvent& event) = 0;
	};
}