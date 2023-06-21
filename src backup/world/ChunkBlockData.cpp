#include <world/ChunkBlockData.hpp>

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


void ChunkBlockData::SetBlock(glm::uvec3 position, Block& block)
{
	if (!positionInBounds(position))
		return;

	blockArray[position.x][position.y][position.z] = block;
}

Block& ChunkBlockData::GetBlock(glm::uvec3 position)
{
	if (!positionInBounds(position))
		return;

	return blockArray[position.x][position.y][position.z];
}