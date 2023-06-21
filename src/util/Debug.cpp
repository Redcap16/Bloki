#include <util/Debug.hpp>

Debug::Debug()
{

}

Debug& Debug::GetInstance()
{
	static Debug instance;
	return instance;
}
