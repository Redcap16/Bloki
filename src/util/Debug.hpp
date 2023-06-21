#pragma once

#include <string>
#include <iostream>

class Debug
{
private:
	Debug();
public:
	static Debug& GetInstance();
	inline void Log(std::string text)
	{
		std::cout << text << std::endl;
	}
};