#include <graphics/Vertex.hpp>

const VertexAttribute Vertex2D::s_Attributes[s_AttributeCount] = { VertexAttribute(2, GL_FLOAT),
		VertexAttribute(2, GL_FLOAT)};

const VertexAttribute Vertex3D::s_Attributes[s_AttributeCount] = { VertexAttribute(3, GL_FLOAT),
		VertexAttribute(2, GL_FLOAT),
		VertexAttribute(3, GL_FLOAT),
		VertexAttribute(1, GL_BYTE) };