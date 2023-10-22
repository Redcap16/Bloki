#pragma once

#include <glm/glm.hpp>
#include <ui/UIRender.hpp>
#include <graphics/Vertex.hpp>
#include <graphics/VertexArray.hpp>

template <typename T>
struct rect
{
	glm::vec<2, T> Position,
		Size;

	rect(glm::vec<2, T> position, glm::vec<2, T> size);
	rect(T x, T y, T w, T h);
};

using irect = rect<int>;
using frect = rect<float>;

class PrimitiveBuffer
{
public:
	PrimitiveBuffer(ColoringType coloringType);

	void AddRectangle(irect dimensions, frect texDimensions, glm::vec3 color = { 1, 1, 1 });
	void AddRectangle(irect dimensions, glm::vec3 color = { 1, 1, 1 });
	void Clear();
	void Render(RenderingParams params);

private:
	VertexArray m_VAO;
	VertexBuffer<Vertex2D> m_VBO;
	ElementBuffer m_EBO;

	const ColoringType m_ColoringType;
};