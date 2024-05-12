#include <window/WindowsKeyboard.hpp>

namespace window {
	void WindowsKeyboard::KeyPressed(Key key, int repeatCount) {
		m_KeyStates[key.Code()] = true;
		
		KeyboardEvent event = { KeyboardEvent::Type::KeyPressed, key, repeatCount };
		for (auto listener : m_Listeners) {
			listener->OnKeyboardEvent(event);
		}
	}

	void WindowsKeyboard::KeyReleased(Key key, int repeatCount) {
		m_KeyStates[key.Code()] = false; 
		
		KeyboardEvent event = { KeyboardEvent::Type::KeyReleased, key, repeatCount };
		for (auto listener : m_Listeners) {
			listener->OnKeyboardEvent(event);
		}
	}
}