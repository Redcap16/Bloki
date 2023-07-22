#pragma once

#include <world/Block.hpp>

class BlockManager
{
public:
	virtual Block GetBlock(WorldPos position) const = 0;
	virtual void DestroyBlock(WorldPos position) = 0;
	virtual bool PlaceBlock(WorldPos position, Block block, bool force = false) = 0;
	virtual bool IsLoaded(WorldPos position) = 0;
};