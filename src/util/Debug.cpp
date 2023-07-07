#include <util/Debug.hpp>


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


Debug::Debug()
{

}

Debug& Debug::GetInstance()
{
	static Debug instance;
	return instance;
}
