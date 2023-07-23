#include <graphics/VertexBuffer.hpp>

VertexAttribute::VertexAttribute(size_t offset, GLint size, GLenum type, GLboolean normalized) :
	Offset(offset),
	Size(size),
	Type(type),
	Normalized(normalized)
{

}