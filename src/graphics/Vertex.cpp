#include <graphics/Vertex.hpp>

const std::vector<VertexAttribute> Vertex2D::Attributes = { VertexAttribute(2, GL_FLOAT),
		VertexAttribute(2, GL_FLOAT) };

const std::vector<VertexAttribute> Vertex3D::Attributes = { VertexAttribute(3, GL_FLOAT),
		VertexAttribute(2, GL_FLOAT),
		VertexAttribute(3, GL_FLOAT),
		VertexAttribute(1, GL_BYTE) };

const std::vector<VertexAttribute> Vertex3DS::Attributes = { VertexAttribute(3, GL_BYTE),
		VertexAttribute(2, GL_FLOAT),
		VertexAttribute(3, GL_BYTE),
		VertexAttribute(1, GL_BYTE) };