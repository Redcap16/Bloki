#include <graphics/VertexArray.hpp>

VertexArray::VertexArray() :
	m_Handle(0),
	m_CurrentElementBuffer(nullptr),
	m_CurrentVertexAttribIndex(0)
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

		m_CurrentElementBuffer = other.m_CurrentElementBuffer;
		m_CurrentVertexAttribIndex = other.m_CurrentVertexAttribIndex;
		m_Buffers.swap(other.m_Buffers);
	}

	return *this;
}

void VertexArray::AddBuffer(AbstractVertexBuffer* buffer)
{
	if (buffer == nullptr)
		return;

	m_Buffers.push_back(buffer);
	updateArray(buffer);
}

void VertexArray::Draw()
{
	if (m_CurrentElementBuffer == nullptr)
		return;

	glBindVertexArray(m_Handle);
	m_CurrentElementBuffer->Bind();
	glDrawElements(GL_TRIANGLES, m_CurrentElementBuffer->Count() * 3, sizeof(size_t) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 0); //TODO: size_t is probably 8 bytes long, so it shouldnt be used here
	CHECK_GL_ERROR();
}

void VertexArray::setup() 
{
	CHECK_GL_ERROR();
	glGenVertexArrays(1, &m_Handle);
	CHECK_GL_ERROR();
}

void VertexArray::updateArray(AbstractVertexBuffer* buffer)
{
	if (buffer == nullptr)
		return; 

	glBindVertexArray(m_Handle);

	std::vector<VertexAttribute> attributes;
	buffer->GetVertexAttributes(attributes);

	buffer->Bind();

	size_t currentOffset = 0;
	for (VertexAttribute& attribute : attributes)
	{
		glEnableVertexAttribArray(m_CurrentVertexAttribIndex);

		switch (attribute.type)
		{
		case GL_HALF_FLOAT:
		case GL_FLOAT:
		case GL_DOUBLE:
			glVertexAttribPointer(m_CurrentVertexAttribIndex, attribute.size, attribute.type, GL_FALSE, (GLsizei)buffer->GetVertexSize(), (const void*)currentOffset);
			break;
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
		case GL_INT:
		case GL_UNSIGNED_INT:
			glVertexAttribIPointer(m_CurrentVertexAttribIndex, attribute.size, attribute.type, (GLsizei)buffer->GetVertexSize(), (const void*)currentOffset);
			break;
		default:
			DEBUG_LOG("VertexArray use of unknown attribute type");
			break;
		}

		switch (attribute.type)
		{
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
			currentOffset += 1 * attribute.size;
			break;
		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
		case GL_HALF_FLOAT:
			currentOffset += 2 * attribute.size;
			break;
		case GL_INT:
		case GL_UNSIGNED_INT:
		case GL_FLOAT:
			currentOffset += 4 * attribute.size;
			break;
		case GL_DOUBLE:
			currentOffset += 8 * attribute.size;
			break;
		default:
			DEBUG_LOG("VertexArray use of unknown attribute type");
			break;
		}

		m_CurrentVertexAttribIndex++;

		CHECK_GL_ERROR();
	}
}