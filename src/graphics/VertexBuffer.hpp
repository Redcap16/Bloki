#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <vector>
#include <deque>

#include <graphics/Buffer.hpp>
#include <graphics/ErrorCheck.hpp>

struct VertexAttribute
{
	size_t Offset;
	GLint Size;
	GLenum Type;
	GLboolean Normalized;
	VertexAttribute(size_t offset, GLint size, GLenum type, GLboolean normalized = GL_FALSE);
};

class AbstractVertexBuffer
{
public:
	virtual BufferHandle GetHandle() const = 0;
	virtual void GetVertexAttributes(std::vector<VertexAttribute>&) const = 0;
	virtual void Bind() const = 0;
	virtual size_t GetVertexSize() const = 0;
};

template <typename TVertex>
class VertexBuffer : public AbstractVertexBuffer
{
public:
	VertexBuffer(bool dynamic);
	~VertexBuffer();
	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;
	VertexBuffer(VertexBuffer&& other) noexcept;
	VertexBuffer& operator=(VertexBuffer&& other) noexcept;

	inline BufferHandle GetHandle() const override;
	inline void GetVertexAttributes(std::vector<VertexAttribute> &attributes) const override;

	inline void AddVertex(const TVertex &vertex);
	inline void AddVertices(const std::vector<TVertex>& vertices);
	inline void ClearData();
	void UpdateBuffer();
	size_t GetCurrentIndex();
	inline void Bind() const override;
	inline size_t GetVertexSize() const override;
private:
	BufferHandle m_Handle;

	bool m_Dynamic;

	std::vector<TVertex> m_Vertices;

	void setup();
};

template <typename TVertex>
VertexBuffer<TVertex>::VertexBuffer(bool dynamic) :
	m_Dynamic(dynamic),
	m_Handle(0)
{
	setup();
}

template <typename TVertex>
VertexBuffer<TVertex>::~VertexBuffer()
{
	if (m_Handle)
	{
		glDeleteBuffers(1, &m_Handle);
		CHECK_GL_ERROR();
	}
}

template <typename TVertex>
VertexBuffer<TVertex>::VertexBuffer(VertexBuffer&& other) noexcept
{
	*this = std::move(other);
}

template <typename TVertex>
VertexBuffer<TVertex>& VertexBuffer<TVertex>::operator=(VertexBuffer&& other) noexcept
{
	if (&other != this)
	{
		m_Dynamic = other.m_Dynamic;

		if (m_Handle)
		{
			glDeleteBuffers(1, &m_Handle);
			CHECK_GL_ERROR();
		}

		m_Handle = other.m_Handle;
		other.m_Handle = 0;

		m_Vertices.swap(other.m_Vertices);
	}

	return *this;
}

template <typename TVertex>
BufferHandle VertexBuffer<TVertex>::GetHandle() const
{
	return m_Handle;
}

template <typename TVertex>
void VertexBuffer<TVertex>::GetVertexAttributes(std::vector<VertexAttribute> &attributes) const
{
	attributes = TVertex::Attributes;
}

template <typename TVertex>
void VertexBuffer<TVertex>::AddVertex(const TVertex &vertex)
{
	m_Vertices.push_back(vertex);
}

template <typename TVertex>
void VertexBuffer<TVertex>::AddVertices(const std::vector<TVertex>& vertices)
{
	m_Vertices.insert(m_Vertices.end(), vertices.begin(), vertices.end());
}

template <typename TVertex>
void VertexBuffer<TVertex>::ClearData()
{
	std::vector<TVertex>().swap(m_Vertices);
}

template <typename TVertex>
void VertexBuffer<TVertex>::UpdateBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(TVertex), m_Vertices.data(), m_Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	CHECK_GL_ERROR();
}

template <typename TVertex>
size_t VertexBuffer<TVertex>::GetCurrentIndex()
{
	size_t size = m_Vertices.size();
	if (size) size--; //To return an index of the last element
	return size;
}

template <typename TVertex>
void VertexBuffer<TVertex>::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
	CHECK_GL_ERROR();
}

template <typename TVertex>
size_t VertexBuffer<TVertex>::GetVertexSize() const
{
	return sizeof(TVertex);
}

template <typename TVertex>
void VertexBuffer<TVertex>::setup()
{
	glGenBuffers(1, &m_Handle);
	CHECK_GL_ERROR();
}