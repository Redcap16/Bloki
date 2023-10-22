#include <ui/PrimitiveBuffer.hpp>

template<typename T>
rect<T>::rect(glm::vec<2, T> position, glm::vec<2, T> size) :
	Position(position),
	Size(size)
{

}

template<typename T>
rect<T>::rect(T x, T y, T w, T h) :
	Position(x, y),
	Size(w, h)
{

}

PrimitiveBuffer::PrimitiveBuffer(ColoringType coloringType) :
	m_VBO(false),
	m_EBO(false),
	m_ColoringType(coloringType)
{
	m_VAO.AddBuffer(&m_VBO);
	m_VAO.SetElementBuffer(&m_EBO);
}

void PrimitiveBuffer::AddRectangle(irect dimensions, frect texDimensions, glm::vec3 color)
{
	m_VBO.AddVertex({ dimensions.Position,										texDimensions.Position,											color });
	m_VBO.AddVertex({ dimensions.Position + glm::ivec2(dimensions.Size.x, 0),	texDimensions.Position + glm::vec2(texDimensions.Size.x, 0),	color });
	m_VBO.AddVertex({ dimensions.Position + dimensions.Size,					texDimensions.Position + texDimensions.Size,					color });
	m_VBO.AddVertex({ dimensions.Position + glm::ivec2(0, dimensions.Size.y),	texDimensions.Position + glm::vec2(0, texDimensions.Size.y),	color });

	ElementIndex index = m_VBO.GetCurrentIndex();
	m_EBO.AddIndex(index - 3);
	m_EBO.AddIndex(index - 2);
	m_EBO.AddIndex(index - 1);

	m_EBO.AddIndex(index - 3);
	m_EBO.AddIndex(index - 1);
	m_EBO.AddIndex(index);

	m_VBO.UpdateBuffer();
	m_EBO.UpdateBuffer();
}

void PrimitiveBuffer::AddRectangle(irect dimensions, glm::vec3 color)
{
	AddRectangle(dimensions, { 0, 0, 0, 0 }, color);
}

void PrimitiveBuffer::Clear()
{
	m_VBO.ClearData();
	m_EBO.ClearData();
}

void PrimitiveBuffer::Render(RenderingParams params)
{
	params.SetColoringType(m_ColoringType);
	m_VAO.Draw();
}