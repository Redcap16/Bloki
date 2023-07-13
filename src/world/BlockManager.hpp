#pragma once

#include <world/Block.hpp>

class BlockManager
{
public:
	virtual const Block* GetBlock(glm::ivec3 position) = 0;
	virtual void DestroyBlock(glm::ivec3 position) = 0;
	virtual void PlaceBlock(glm::ivec3 position, const Block& block, bool force = false) = 0;
};