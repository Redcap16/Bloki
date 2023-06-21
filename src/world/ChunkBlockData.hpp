#pragma once

#include <world/Block.hpp>
#include <stdexcept>

///<summary>
///Class that keeps data about blocks present in the chunk and makes possible to manage them
///</summary>
class ChunkBlockData
{
public:
	static constexpr unsigned int NeighborCount = 6;
	static constexpr glm::uvec3 ChunkSize = { 16, 64, 16 };
private:
	Block blockArray[ChunkSize.x][ChunkSize.y][ChunkSize.z];

	bool positionInBounds(glm::uvec3 position) const;
public:
	ChunkBlockData();

	void SetBlock(glm::uvec3 position, const Block& block);
	Block& GetBlock(glm::uvec3 position);
	static constexpr bool PositionOnBorder(glm::uvec3 position);
};

constexpr bool ChunkBlockData::PositionOnBorder(glm::uvec3 position)
{
	return position.x == 0 || position.x == ChunkSize.x - 1 ||
		position.y == 0 || position.y == ChunkSize.y - 1 ||
		position.z == 0 || position.z == ChunkSize.z - 1;
}

struct ChunkNeighbors
{
	ChunkBlockData* XPlus, *XMinus,
		*YPlus, *YMinus,
		*ZPlus, *ZMinus;

	ChunkNeighbors();
	ChunkNeighbors(ChunkBlockData* xPlus, ChunkBlockData* xMinus, ChunkBlockData* yPlus, ChunkBlockData* yMinus, ChunkBlockData* zPlus, ChunkBlockData* zMinus);
};