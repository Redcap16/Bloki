#pragma once

#include <world/Chunk.hpp>

#include <util/db_perlin.h>
#include <thread>

class World;

class WorldGenerator
{
public:
	WorldGenerator(World &world);
	~WorldGenerator();

	void Generate(Chunk& chunk, glm::ivec3 position);
private:
	World& m_World;

	void generateTree(Chunk& chunk, glm::ivec3 givenChunkPosition, glm::ivec3 rootPosition);
	void setStructureBlock(Chunk& chunk, glm::ivec3 givenChunkPosition, glm::ivec3 position, Block::BlockType type);
};