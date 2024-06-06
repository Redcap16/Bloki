#include <engine/graphics/ElementBuffer.hpp>

ElementBuffer::ElementBuffer(bool dynamic) : 
	m_Dynamic(dynamic),
	m_Handle(0),
	m_IndicesCount(0)
{
	setup();
}

ElementBuffer::~ElementBuffer()
{
	if (!m_Handle)
		return;

	glDeleteBuffers(1, &m_Handle);
	CHECK_GL_ERROR();
}

ElementBuffer::ElementBuffer(ElementBuffer &&other) noexcept
{
	*this = std::move(other);
}

ElementBuffer& ElementBuffer::operator=(ElementBuffer &&other) noexcept
{
	if (this != &other)
	{
		if (m_Handle)
		{
			glDeleteBuffers(1, &m_Handle);
			CHECK_GL_ERROR();
		}

		m_Handle = other.m_Handle;
		other.m_Handle = 0;

		m_Dynamic = other.m_Dynamic;
		m_Indices.swap(other.m_Indices);
	}

	return *this;
}

void ElementBuffer::UpdateBuffer()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(ElementIndex), m_Indices.data(), m_Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	m_IndicesCount = m_Indices.size();
	CHECK_GL_ERROR();
}

void ElementBuffer::ClearData()
{
	std::vector<ElementIndex>().swap(m_Indices);
}

void ElementBuffer::setup()
{
	glGenBuffers(1, &m_Handle);
	CHECK_GL_ERROR();
}