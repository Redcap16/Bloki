#pragma once

#include <graphics/ShaderProgram.hpp>
#include <graphics/RenderingContext.hpp>
#include <graphics/Camera2D.hpp>
#include <ui/Element.hpp>

class UIManager
{
private:
	std::vector <Element*> elements;

public:
	UIManager();

	static UIManager& GetInstance();

	void Render(RenderingContext &context);

	void AddElement(Element* newElement);
};