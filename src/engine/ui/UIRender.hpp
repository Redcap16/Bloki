#pragma once

#include <engine/graphics/ShaderProgram.hpp>

enum class ColoringType
{
	Color = 0,
	ColorTexture = 1,
	AlphaTexture = 2
};

class RenderingParams
{
public:
	RenderingParams(ShaderProgram& shader, glm::mat4 projectionMatrix);

	void SetColoringType(ColoringType type);
	void SetModelMatrix(glm::mat4 modelMatrix);
private:
	static constexpr const char* c_ColoringTypeName = "ColoringType";

	ShaderProgram& m_Shader;
	UniformLocation m_ColoringTypeLoc;

	const glm::mat4 m_Projection;
};