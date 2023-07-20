#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <deque>
#include <vector>

#include <graphics/Buffer.hpp>
#include <graphics/ErrorCheck.hpp>

typedef unsigned int ElementIndex;

class ElementBuffer
{
public:
	ElementBuffer(bool dynamic);
	~ElementBuffer();
	ElementBuffer(const ElementBuffer&) = delete;
	ElementBuffer& operator=(const ElementBuffer&) = delete;
	ElementBuffer(ElementBuffer &&other) noexcept;
	ElementBuffer& operator=(ElementBuffer &&other) noexcept;


	inline void AddIndex(ElementIndex index);
	inline void AddIndices(std::vector<ElementIndex> indices);
	inline BufferHandle GetHandle() const;
	void UpdateBuffer();
	void ClearData();
	inline size_t Count() const;
	inline void Bind() const;
private:
	BufferHandle m_Handle;
	bool m_Dynamic;
	size_t m_IndicesCount;

	std::vector<ElementIndex> m_Indices;
	
	void setup();
};

void ElementBuffer::AddIndex(ElementIndex index)
{
	m_Indices.push_back(index);
}

void ElementBuffer::AddIndices(std::vector<ElementIndex> indices)
{
	m_Indices.insert(m_Indices.end(), indices.begin(), indices.end());
}

BufferHandle ElementBuffer::GetHandle() const
{
	return m_Handle;
}

size_t ElementBuffer::Count() const
{
	return m_IndicesCount;
}

void ElementBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
}