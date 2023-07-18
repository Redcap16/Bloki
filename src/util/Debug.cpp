#include <util/Debug.hpp>


void ConsoleDebugHandler::Log(const char* message)
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	std::cout << message << std::endl;
}

void ConsoleDebugHandler::Log(std::string message)
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	std::cout << message << std::endl;
}

DebugHandler* DebugProvider::m_Instance;
DummyDebugHandler DebugProvider::m_Dummy;

DebugHandler& DebugProvider::Get()
{
	if (m_Instance != nullptr)
		return *m_Instance;
	return m_Dummy;
}

void DebugProvider::Set(DebugHandler* instance)
{
	m_Instance = instance;
}