#pragma once

class Keyboard
{
public:
	enum
	{
		KeyShift,
		KeySpace
	};

	virtual bool GetKeyState(char key) = 0;
};

class Mouse
{
public:
	virtual void SetMousePosition(glm::ivec2 position) = 0;
};