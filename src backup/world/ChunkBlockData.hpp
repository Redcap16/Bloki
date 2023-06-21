#pragma once

#include <world/Block.hpp>

///<summary>
///Class that keeps data about blocks present in the chunk and makes possible to manage them
///</summary>
class ChunkBlockData
{
public:
	static constexpr glm::uvec3 ChunkSize = { 16, 64, 16 };
private:
	Block blockArray[ChunkSize.x][ChunkSize.y][ChunkSize.z];

	bool positionInBounds(glm::uvec3 position) const;
public:
	ChunkBlockData();

	void SetBlock(glm::uvec3 position, Block& block);
	Block& GetBlock(glm::uvec3 position);
};

