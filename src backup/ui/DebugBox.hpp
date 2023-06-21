#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <graphics/Vertex.hpp>
#include <ui/Element.hpp>
#include <ui/Text.hpp>
#include <ui/UIManager.hpp>

class DebugBox
{
private:
	std::vector<std::pair<std::string, std::string>> items;
	std::vector<Text*> texts;
	Font font;
	static const glm::vec3 color;
	static const glm::ivec2 initialPos;
	static const unsigned short fontSize = 20;

	bool show = true;

	DebugBox();
	void addDebugItem(std::string name, std::string value);
public:
	~DebugBox();
	static DebugBox& GetInstance();
	void ChangeDebugValue(std::string name, std::string value);
	inline void Show(bool show) { this->show = show; } //TODO
};