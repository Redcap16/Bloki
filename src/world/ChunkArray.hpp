#pragma once

#include <world/Chunk.hpp>

struct ChunkArray
{
	static constexpr glm::uvec3 Size = { 15, 1, 15 };
	Chunk* Chunks[Size.x][Size.y][Size.z] = { nullptr };

	inline bool InBounds(glm::ivec3 position);
};

bool ChunkArray::InBounds(glm::ivec3 position)
{
	return position.x >= 0 && position.x < Size.x &&
		position.y >= 0 && position.y < Size.y &&
		position.z >= 0 && position.z < Size.z;
}