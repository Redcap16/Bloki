#include <ui/DebugBox.hpp>

const glm::vec3 DebugBox::color = glm::vec3(0);
const glm::ivec2 DebugBox::initialPos = glm::ivec2(10, 30);

DebugBox::DebugBox() :
	font("DejaVuSerif.ttf", fontSize)
{
}

void DebugBox::addDebugItem(std::string name, std::string value)
{
	items.push_back({ name, value });

	const glm::ivec2 textPos = initialPos + glm::ivec2(0, fontSize * texts.size());
	texts.push_back(new Text(font, name + ": " + value, textPos));

	UIManager::GetInstance().AddElement(texts.back());
}

DebugBox::~DebugBox()
{
	for (auto it = texts.begin(); it != texts.end(); it++)
		delete (*it);
}

DebugBox& DebugBox::GetInstance()
{
	static DebugBox instance;
	return instance;
}

void DebugBox::ChangeDebugValue(std::string name, std::string value)
{
	for (auto it = items.begin(); it != items.end(); it++)
		if ((*it).first == name)
		{
			(*it).second = value;
			texts[it - items.begin()]->SetText(name + ": " + value);
			return;
		}

	addDebugItem(name, value);
}