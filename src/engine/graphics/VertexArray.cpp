#include <engine/graphics/VertexArray.hpp>

VertexArray::VertexArray(bool dynamic) :
	m_Handle(0),
	m_CurrentVertexAttribIndex(0),
	m_ElementBuffer(std::make_unique<ElementBuffer>(dynamic))
{
	setup();
}

VertexArray::~VertexArray()
{
	if (m_Handle)
	{
		glDeleteVertexArrays(1, &m_Handle);

		CHECK_GL_ERROR();
	}
}

VertexArray::VertexArray(VertexArray &&other) noexcept
{
	*this = std::move(other);
}

VertexArray& VertexArray::operator=(VertexArray &&other) noexcept
{
	if (this != &other)
	{
		if (m_Handle)
		{
			glDeleteVertexArrays(1, &m_Handle);
			CHECK_GL_ERROR();
		}

		m_Handle = other.m_Handle;
		other.m_Handle = 0;

		m_ElementBuffer = std::move(other.m_ElementBuffer);
		m_CurrentVertexAttribIndex = other.m_CurrentVertexAttribIndex;
		m_Buffers.swap(other.m_Buffers);
	}

	return *this;
}

void VertexArray::Draw() const
{
	glBindVertexArray(m_Handle);
	m_ElementBuffer->Bind();
	glDrawElements(GL_TRIANGLES, m_ElementBuffer->Count(), sizeof(ElementIndex) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 0);
	CHECK_GL_ERROR();
}

void VertexArray::setup() 
{
	CHECK_GL_ERROR();
	glGenVertexArrays(1, &m_Handle);
	CHECK_GL_ERROR();
}

void VertexArray::updateArray(const AbstractVertexBuffer& buffer)
{
	glBindVertexArray(m_Handle);

	std::vector<VertexAttribute> attributes;
	buffer.GetVertexAttributes(attributes);

	buffer.Bind();

	size_t currentOffset = 0;
	for (VertexAttribute& attribute : attributes)
	{
		glEnableVertexAttribArray(m_CurrentVertexAttribIndex);

		if(attribute.Normalized)
			glVertexAttribPointer(m_CurrentVertexAttribIndex, attribute.Size, attribute.Type, attribute.Normalized, (GLsizei)buffer.GetVertexSize(), (const void*)attribute.Offset);
		else
			switch (attribute.Type)
			{
			case GL_HALF_FLOAT:
			case GL_FLOAT:
			case GL_DOUBLE:
				glVertexAttribPointer(m_CurrentVertexAttribIndex, attribute.Size, attribute.Type, attribute.Normalized, (GLsizei)buffer.GetVertexSize(), (const void*)attribute.Offset);
				break;
			case GL_BYTE:
			case GL_UNSIGNED_BYTE:
			case GL_SHORT:
			case GL_UNSIGNED_SHORT:
			case GL_INT:
			case GL_UNSIGNED_INT:
				glVertexAttribIPointer(m_CurrentVertexAttribIndex, attribute.Size, attribute.Type, (GLsizei)buffer.GetVertexSize(), (const void*)attribute.Offset);
				break;
			default:
				DEBUG_LOG("VertexArray use of unknown attribute type");
				break;
			}
		
		m_CurrentVertexAttribIndex++;

		CHECK_GL_ERROR();
	}
}