#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <graphics/RenderingContext.hpp>

class Element
{
private:
	glm::ivec2 position;
	void recalculateMatrix();

protected:
	glm::mat4 model = glm::mat4(1.0f);
	inline glm::ivec2 GetPosition()
	{
		return position;
	}

public:
	Element(glm::ivec2 position = glm::ivec2(0));
	void SetPosition(glm::ivec2 position);

	virtual void Render(RenderingContext &context) = 0;
};