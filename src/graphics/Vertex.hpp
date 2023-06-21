#pragma once

#include <inttypes.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <graphics/VertexBuffer.hpp>

struct Vertex2D
{
	glm::vec2 Position;
	glm::vec2 TextureCoords;

	static constexpr int s_AttributeCount = 2;
	static const VertexAttribute s_Attributes[s_AttributeCount];
};

struct Vertex3D
{
	glm::vec3 Position;
	glm::vec2 TextureCoords;
	glm::vec3 Normal;
	char state;
	
	static constexpr int s_AttributeCount = 4;
	static const VertexAttribute s_Attributes[s_AttributeCount];
}; 

struct Vertex3DShort
{
	float x, y, z;
	float u, v;
};

struct Vertex3DT : public Vertex3D
{
	float transparency;
};