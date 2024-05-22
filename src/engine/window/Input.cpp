#include <engine/window/Input.hpp>

namespace window {
	Keyboard::Key::Key(char code)
	{
		assert(std::isalnum(code));
		if (std::isdigit(code))
			m_Code = code;
		else
			m_Code = std::toupper(code);
	}

	bool Keyboard::Key::IsChar() const {
		return std::isalnum(m_Code);
	}

	char Keyboard::Key::GetCharValue() const {
		if (isalnum(m_Code))
			return m_Code;
		return 0;
	}

	bool operator==(const Keyboard::Key& lhs, const Keyboard::Key& rhs)
	{
		return lhs.Code() == rhs.Code();
	}
}