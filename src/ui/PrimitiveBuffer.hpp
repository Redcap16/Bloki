#pragma once

#include <glm/glm.hpp>
#include <ui/UIRender.hpp>
#include <graphics/Vertex.hpp>
#include <graphics/VertexArray.hpp>
#include <graphics/Texture.hpp>

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

class Primitive
{
public:
	virtual std::vector<Vertex2D> GetVertices() const = 0;
	virtual std::vector<ElementIndex> GetIndices(ElementIndex lastIndex) const = 0;
};

class PrimitiveRectangle : public Primitive
{
public:
	PrimitiveRectangle(irect dimensions, frect texDimensions, glm::vec3 color = { 1, 1, 1 });
	PrimitiveRectangle(irect dimensions, glm::vec3 color = { 1, 1, 1 }) :
		PrimitiveRectangle(dimensions, { 0, 0, 0, 0 }, color) { };

	std::vector<Vertex2D> GetVertices() const override;
	std::vector<ElementIndex> GetIndices(ElementIndex lastIndex) const override;

private:
	irect m_Dimensions;
	frect m_TexDimensions;
	glm::vec3 m_Color;
};

class PrimitiveBuffer
{
public:
	PrimitiveBuffer(ColoringType coloringType);

	void SetTexture(const Texture* texture) { m_Texture = texture; };

	void AddPrimitive(Primitive* primitive) { m_Primitives.push_back(primitive); };
	void RemovePrimitive(Primitive* primitive);
	void Update();
	void Clear();
	void Render(RenderingParams params) const;

private:
	VertexArray m_VAO;
	VertexBuffer<Vertex2D>* m_VBO;
	ElementBuffer* m_EBO;

	std::vector<Primitive*> m_Primitives;

	const ColoringType m_ColoringType;
	const Texture* m_Texture;
};

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