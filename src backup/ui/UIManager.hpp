#pragma once

#include <graphics/ShaderProgram.hpp>
#include <graphics/Camera2D.hpp>
#include <ui/Element.hpp>

class UIManager
{
private:
	ShaderProgram shader;
	std::vector <Element*> elements;

public:
	UIManager();

	static UIManager& GetInstance();

	void Render(Camera2D& camera);

	void AddElement(Element* newElement);

	inline ShaderProgram& GetShaderProgram()
	{
		return shader;
	}
};