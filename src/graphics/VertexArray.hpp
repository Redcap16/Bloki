#pragma once

#include <graphics/VertexBuffer.hpp>
#include <graphics/ElementBuffer.hpp>

#include <util/Debug.hpp>

typedef GLuint VertexArrayHandle;

class VertexArray
{
public:
	VertexArray();
	~VertexArray();
	VertexArray(const VertexArray&) = delete;
	VertexArray& operator=(const VertexArray&) = delete;
	VertexArray(VertexArray &&other) noexcept;
	VertexArray& operator=(VertexArray &&other) noexcept;

	void AddBuffer(AbstractVertexBuffer* buffer);
	inline void SetElementBuffer(ElementBuffer* buffer);
	void Draw() const;
private:
	std::vector<AbstractVertexBuffer*> m_Buffers;
	ElementBuffer* m_CurrentElementBuffer;

	VertexArrayHandle m_Handle;

	unsigned int m_CurrentVertexAttribIndex;

	void setup();
	void updateArray(AbstractVertexBuffer* buffer);
};

void VertexArray::SetElementBuffer(ElementBuffer* buffer)
{
	m_CurrentElementBuffer = buffer;
}