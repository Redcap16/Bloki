#pragma once

#include <engine/graphics/VertexBuffer.hpp>
#include <engine/graphics/ElementBuffer.hpp>

#include <engine/util/Debug.hpp>
#include <cassert>

typedef GLuint VertexArrayHandle;

class VertexArray
{
public:
	VertexArray(bool dynamic);
	~VertexArray();
	VertexArray(const VertexArray&) = delete;
	VertexArray& operator=(const VertexArray&) = delete;
	VertexArray(VertexArray &&other) noexcept;
	VertexArray& operator=(VertexArray &&other) noexcept;

	template <typename TVertex>
	VertexBuffer<TVertex>& CreateVertexBuffer(bool dynamic);
	ElementBuffer& GetElementBuffer() { return *m_ElementBuffer.get(); };
	void Draw() const;
private:
	std::vector<std::unique_ptr<AbstractVertexBuffer>> m_Buffers;
	std::unique_ptr<ElementBuffer> m_ElementBuffer;

	VertexArrayHandle m_Handle;

	unsigned int m_CurrentVertexAttribIndex;

	void setup();
	void updateArray(const AbstractVertexBuffer& buffer);
};

template <typename TVertex>
VertexBuffer<TVertex>& VertexArray::CreateVertexBuffer(bool dynamic)
{
	m_Buffers.push_back(std::unique_ptr<AbstractVertexBuffer>(new VertexBuffer<TVertex>(dynamic)));

	updateArray(*m_Buffers.back());
	return *static_cast<VertexBuffer<TVertex>*>(m_Buffers.back().get());
}