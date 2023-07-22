#include <graphics/VertexBuffer.hpp>

VertexAttribute::VertexAttribute(size_t offset, GLint size, GLenum type) :
	offset(offset),
	size(size),
	type(type)
{

}