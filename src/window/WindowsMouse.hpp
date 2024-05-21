#pragma once

#include <glm/glm.hpp>

#include <cctype>
#include <array>

#include <windows.h>
#include <window/Input.hpp>

namespace window {
	class WindowsMouse : public Mouse
	{
	public:
		WindowsMouse(const HWND& windowHandle);
		~WindowsMouse() override = default;

		void MouseMoved(glm::ivec2 position);
		void ButtonPressed(Button button);
		void ButtonReleased(Button button);

		void SetPosition(glm::ivec2 position) override;
		glm::ivec2 GetPosition() const override { return m_Position; };
		bool IsButtonPressed(Button button) const override { return m_Buttons[(int)button]; };
		void SetCursorVisible(bool visible) override;
	private:
		const HWND& m_WindowHandle;
		glm::ivec2 m_Position;

		std::array<bool, c_ButtonCount> m_Buttons;
	};
}