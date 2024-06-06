#include <engine/ui/PrimitiveBuffer.hpp>

PrimitiveBuffer::PrimitiveBuffer(ColoringType coloringType) :
	m_VAO(false),
	m_VBO(&m_VAO.CreateVertexBuffer<Vertex2D>(false)),
	m_EBO(&m_VAO.GetElementBuffer()),
	m_ColoringType(coloringType),
	m_Texture(nullptr)
{
}

PrimitiveRectangle::PrimitiveRectangle(irect dimensions, frect texDimensions, glm::vec3 color) :
	m_Dimensions(dimensions),
	m_TexDimensions(texDimensions),
	m_Color(color)
{

}

std::vector<Vertex2D> PrimitiveRectangle::GetVertices() const
{
	return {{ m_Dimensions.Position,										m_TexDimensions.Position,											glm::ivec4(m_Color, 255) },
			{ m_Dimensions.Position + glm::ivec2(m_Dimensions.Size.x, 0),	m_TexDimensions.Position + glm::vec2(m_TexDimensions.Size.x, 0),	glm::ivec4(m_Color, 255) },
			{ m_Dimensions.Position + m_Dimensions.Size,					m_TexDimensions.Position + m_TexDimensions.Size,					glm::ivec4(m_Color, 255) },
			{ m_Dimensions.Position + glm::ivec2(0, m_Dimensions.Size.y),	m_TexDimensions.Position + glm::vec2(0, m_TexDimensions.Size.y),	glm::ivec4(m_Color, 255) }};
}

std::vector<ElementIndex> PrimitiveRectangle::GetIndices(ElementIndex lastIndex) const
{
	return { lastIndex - 3, lastIndex - 2, lastIndex - 1,
			lastIndex - 3, lastIndex - 1, lastIndex - 0 };
}

void PrimitiveBuffer::RemovePrimitive(Primitive* primitive)
{
	auto it = std::find(m_Primitives.begin(), m_Primitives.end(), primitive);
}

void PrimitiveBuffer::Update()
{
	for (auto primitive : m_Primitives)
	{
		m_VBO->AddVertices(primitive->GetVertices());
		m_EBO->AddIndices(primitive->GetIndices(m_VBO->GetCurrentIndex()));
	}
	m_VBO->UpdateBuffer();
	m_EBO->UpdateBuffer();

	m_VBO->ClearData();
	m_EBO->ClearData();
}

void PrimitiveBuffer::Clear()
{
	m_Primitives.clear();
	Update();
}

void PrimitiveBuffer::Render(RenderingParams params) const
{
	params.SetColoringType(m_ColoringType);
	if (m_Texture != nullptr &&
		(m_ColoringType == ColoringType::ColorTexture || m_ColoringType == ColoringType::AlphaTexture))
		m_Texture->Bind(0);
	m_VAO.Draw();
}