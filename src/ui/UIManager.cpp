#include <ui/UIManager.hpp>

UIManager::UIManager()
{
}

UIManager& UIManager::GetInstance()
{
	static UIManager instance;
	return instance;
}

void UIManager::Render(RenderingContext &context)
{
	for (auto it = elements.begin(); it != elements.end(); it++)
	{
		(*it)->Render(context);
	}
}

void UIManager::AddElement(Element* newElement)
{
	if (!newElement)
		return;
	elements.push_back(newElement);
}