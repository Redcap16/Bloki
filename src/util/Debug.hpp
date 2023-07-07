#pragma once

#include <string>
#include <iostream>

class DebugHandler 
{
public:
	virtual void Log(const char*) = 0;
	virtual void Log(std::string) = 0;
private:
};

class DummyDebugHandler : public DebugHandler
{
	inline void Log(const char* message) override;
	inline void Log(std::string message) override;
};

class DebugProvider
{
public:
	static DebugHandler& Get();
	static void Set(DebugHandler* instance);
private:
	static DebugHandler* m_Instance;
	static DummyDebugHandler m_Dummy;
};

#define DEBUG_LOG(message) DebugProvider::Get().Log(message);

void DummyDebugHandler::Log(const char* message)
{
	//Do nothing
}

void DummyDebugHandler::Log(std::string message)
{
	//Do nothing
}





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