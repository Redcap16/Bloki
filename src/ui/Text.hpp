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
#include <ui/Font.hpp>
#include <ui/UIManager.hpp>

#include <graphics/ResourceManager.hpp>

/*class TextShader : public ShaderProgram
{
private:
	static constexpr char shaderFilePath[] = "ui.shader";
public:
	TextShader();

	void SetMVPMatrix(glm::mat4 matrix) override;
	void SetTextureIndex(unsigned int index);
	void SetTextColor(glm::vec3 textColor);
};*/

class Text : public Element
{
private:
	//static TextShader* shader;
	ShaderProgram* m_Shader;

	std::string text = "";
	unsigned int vao = 0,
		vbo = 0,
		ebo = 0,
		indicesCount = 0;
	Font& font;

	glm::vec3 textColor = glm::vec3(0.0f);

	void setupBuffers();
	void updateBuffers();
public:
	Text(ResourceManager& resourceManager, Font& font, std::string text = "", glm::ivec2 position = glm::ivec2(0));

	void SetText(std::string text);
	void Render(RenderingContext &context) override;

	inline void SetColor(glm::vec3 color)
	{
		this->textColor = color;
	}
};