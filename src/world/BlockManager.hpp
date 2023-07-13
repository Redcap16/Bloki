#pragma once

#include <world/Block.hpp>

class BlockManager
{
public:
	virtual Block GetBlock(glm::ivec3 position) const = 0;
	virtual void DestroyBlock(glm::ivec3 position) = 0;
	virtual bool PlaceBlock(glm::ivec3 position, Block block, bool force = false) = 0;
};