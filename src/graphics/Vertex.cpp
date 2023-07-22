#include <graphics/Vertex.hpp>

const std::vector<VertexAttribute> Vertex2D::Attributes = { VertexAttribute(offsetof(Vertex2D, Position), 2, GL_FLOAT),
		VertexAttribute(offsetof(Vertex2D, TextureCoords), 2, GL_FLOAT) };

const std::vector<VertexAttribute> Vertex3D::Attributes = { VertexAttribute(offsetof(Vertex3D, Position), 3, GL_FLOAT),
		VertexAttribute(offsetof(Vertex3D, TextureCoords), 2, GL_FLOAT),
		VertexAttribute(offsetof(Vertex3D, Normal), 3, GL_FLOAT),
		VertexAttribute(offsetof(Vertex3D, state), 1, GL_BYTE) };

const std::vector<VertexAttribute> Vertex3DS::Attributes = { VertexAttribute(offsetof(Vertex3DS, TextureCoords), 2, GL_FLOAT),
		VertexAttribute(offsetof(Vertex3DS, Position), 3, GL_BYTE),
		VertexAttribute(offsetof(Vertex3DS, Normal), 3, GL_BYTE),
		VertexAttribute(offsetof(Vertex3DS, state), 1, GL_BYTE) };