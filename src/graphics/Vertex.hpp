#pragma once

#include <inttypes.h>
#include <glm/glm.hpp>

#include <graphics/VertexBuffer.hpp>

//TODO: Struct packing

struct Vertex2D
{
	glm::ivec2 Position;
	glm::vec2 TextureCoords;
	glm::vec<4, uint8_t> Color;

	Vertex2D() : 
		Position(0),
		TextureCoords(0),
		Color(0) {}
	Vertex2D(glm::ivec2 pos, glm::vec2 texCoords, glm::vec<4, uint8_t> color) :
		Position(pos),
		TextureCoords(texCoords),
		Color(color) { }
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
	glm::vec2 TextureCoords;
	glm::vec<3, int8_t> Position;
	glm::vec<3, int8_t> Normal;
	uint8_t state;

	static const std::vector<VertexAttribute> Attributes;
};