#include <ui/UIRender.hpp>

RenderingParams::RenderingParams(ShaderProgram& shader, glm::mat4 projection) :
	m_Shader(shader),
	m_ColoringTypeLoc(shader.GetUniformLocation(c_ColoringTypeName)),
	m_Projection(projection)
{

}

void RenderingParams::SetColoringType(ColoringType type)
{
	m_Shader.SetUniform(m_ColoringTypeLoc, (int)type);
}

void RenderingParams::SetModelMatrix(glm::mat4 modelMatrix)
{
	m_Shader.SetMVPMatrix(m_Projection * modelMatrix);
}