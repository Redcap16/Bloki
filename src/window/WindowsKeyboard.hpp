#pragma once

#include <glm/glm.hpp>
#include <array>

#include <windows.h>
#include <window/Input.hpp>

namespace window {
	class WindowsKeyboard : public Keyboard
	{
	public:
		~WindowsKeyboard() override = default;

		void KeyPressed(Key key, int repeatCount);
		void KeyReleased(Key key, int repeatCount);

		bool GetKeyState(Key key) const override { return m_KeyStates[key.Code()]; }

	private:
		std::array<bool, c_KeyCount> m_KeyStates;
	};
}