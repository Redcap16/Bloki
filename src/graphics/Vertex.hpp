#pragma once

#include <inttypes.h>
#include <glm/glm.hpp>

#include <graphics/VertexBuffer.hpp>

struct Vertex2D
{
	glm::vec2 Position;
	glm::vec2 TextureCoords;

	static const std::vector<VertexAttribute> Attributes;
};

struct Vertex3D
{
	glm::vec3 Position;
	glm::vec2 TextureCoords;
	glm::vec3 Normal;
	char state;

	static const std::vector<VertexAttribute> Attributes;
}; 

struct Vertex3DS
{
	glm::vec<3, uint8_t> Position;
	glm::vec2 TextureCoords;
	glm::vec<3, uint8_t> Normal;
	uint8_t state;

	static const std::vector<VertexAttribute> Attributes;
};