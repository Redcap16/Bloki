#pragma once

#include <string>
#include <iostream>
#include <mutex>

class DebugHandler 
{
public:
	virtual void Log(const char*) = 0;
	virtual void Log(std::string) = 0;
private:
};

class DummyDebugHandler : public DebugHandler
{
public:
	void Log(const char* message) override {}; //Do nothing
	void Log(std::string message) override {}; //Do nothing
};

class ConsoleDebugHandler : public DebugHandler
{
public:
	void Log(const char* message) override;
	void Log(std::string message) override;
private:
	std::mutex m_Mutex;
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