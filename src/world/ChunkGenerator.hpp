#pragma once

#include <world/Chunk.hpp>

#include <util/db_perlin.h>
#include <thread>

class ChunkGenerator
{
public:
	ChunkGenerator();
	~ChunkGenerator();

	void GenerateChunk(ChunkPos position, Chunk& chunk);
private:

	void generateTree(Chunk& chunk, glm::ivec3 givenChunkPosition, glm::ivec3 rootPosition);
	void setStructureBlock(Chunk& chunk, glm::ivec3 givenChunkPosition, glm::ivec3 position, Block::BlockType type);
};