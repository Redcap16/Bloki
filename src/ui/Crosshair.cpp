#include <ui/Crosshair.hpp>

Crosshair::Crosshair(WidgetParent& parent) :
	Widget(parent, glm::ivec2(0), glm::ivec2(0)),
	m_Texture(c_TextureFilename, true),
	m_Rectangle(glm::ivec2(0), glm::ivec2(0), glm::vec2(0), glm::vec2(1, 1))
{
	SetAnchor(AnchorPoint::Center);
	SetRelativeTo(AnchorPoint::Center);

	SetSize(m_Texture->GetSize());
	m_Rectangle.SetSize(m_Texture->GetSize());
}

void Crosshair::render(WidgetRenderParams& params)
{
	UniformLocation hasTexture = params.m_Shader.GetUniformLocation("HasTexture"),
		texture = params.m_Shader.GetUniformLocation("Texture");
	params.m_Shader.SetUniform(hasTexture, 1);
	params.m_Shader.SetUniform(texture, 0);

	m_Texture->Bind(0);

	m_Rectangle.Render();
}