#pragma once

#include <world/Chunk.hpp>
#include <world/BlockManager.hpp>

#include <thread>
#include <mutex>
#include <db_perlin/db_perlin.h>

class ChunkGenerator
{
public:
	ChunkGenerator(BlockManager& world);

	void GenerateChunk(Chunk& chunk);
private:
	BlockManager& m_World;

	void generateTree(Chunk& chunk, glm::ivec3 givenChunkPosition, glm::ivec3 rootPosition);
	void setStructureBlock(Chunk& chunk, const glm::ivec3& blockPosition, Block block);
};