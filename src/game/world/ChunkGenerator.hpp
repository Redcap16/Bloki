#pragma once

#include <game/world/Chunk.hpp>
#include <game/world/BlockManager.hpp>

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

	void generateTree(BlockArray& chunk, const ChunkPos& givenChunkPosition, const WorldPos& rootPosition);
	void setStructureBlock(BlockArray& chunk, const ChunkPos& givenChunkPosition, const WorldPos& blockPosition, Block block);
};