#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <vector>

#include <graphics/Buffer.hpp>
#include <graphics/ErrorCheck.hpp>

class ElementBuffer
{
public:
	ElementBuffer(bool dynamic);
	~ElementBuffer();
	ElementBuffer(const ElementBuffer&) = delete;
	ElementBuffer& operator=(const ElementBuffer&) = delete;
	ElementBuffer(ElementBuffer &&other) noexcept;
	ElementBuffer& operator=(ElementBuffer &&other) noexcept;


	inline void AddIndex(size_t index);
	inline void AddIndices(std::vector<size_t> indices);
	inline BufferHandle GetHandle() const;
	void UpdateBuffer();
	void ClearData();
	inline size_t Count() const;
	inline void Bind() const;
private:
	BufferHandle m_Handle;
	bool m_Dynamic;

	std::vector<size_t> m_Indices;

	void setup();
};

void ElementBuffer::AddIndex(size_t index)
{
	m_Indices.push_back(index);
}

void ElementBuffer::AddIndices(std::vector<size_t> indices)
{
	m_Indices.insert(m_Indices.end(), indices.begin(), indices.end());
}

BufferHandle ElementBuffer::GetHandle() const
{
	return m_Handle;
}

size_t ElementBuffer::Count() const
{
	return m_Indices.size();
}

void ElementBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
}