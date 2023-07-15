#pragma once

class Keyboard
{
public:
	enum
	{
		KeyShift,
		KeySpace
	};

	bool GetKeyState(char key);
};