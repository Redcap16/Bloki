#include <world/ChunkBlockData.hpp>

ChunkNeighbors::ChunkNeighbors() :
	ChunkNeighbors(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr)
{

}

ChunkNeighbors::ChunkNeighbors(ChunkBlockData* xPlus, ChunkBlockData* xMinus, ChunkBlockData* yPlus, ChunkBlockData* yMinus, ChunkBlockData* zPlus, ChunkBlockData* zMinus) :
	XPlus(xPlus),
	XMinus(xMinus),
	YPlus(yPlus),
	YMinus(yMinus),
	ZPlus(zPlus),
	ZMinus(zMinus)
{

}

bool ChunkBlockData::positionInBounds(glm::uvec3 position) const
{
	const bool inChunk = (position.x < ChunkSize.x &&
		position.y < ChunkSize.y &&
		position.z < ChunkSize.z);
	return inChunk;
}

ChunkBlockData::ChunkBlockData()
{

}


void ChunkBlockData::SetBlock(glm::uvec3 position, const Block& block)
{
	if (!positionInBounds(position))
		return;

	blockArray[position.x][position.y][position.z] = block;
}

Block& ChunkBlockData::GetBlock(glm::uvec3 position)
{
	if (!positionInBounds(position))
		throw std::invalid_argument("Position is out of range");

	return blockArray[position.x][position.y][position.z];
}