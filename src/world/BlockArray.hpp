#pragma once

#include <world/Block.hpp>
#include <stdexcept>

typedef glm::uvec3 InChunkPos;

struct BlockArray
{
	static constexpr glm::uvec3 ChunkSize = { 16, 64, 16 };

	BlockArray(const BlockArray&) = delete;
	BlockArray& operator=(const BlockArray&) = delete;
	BlockArray(BlockArray&& other) : data(std::move(other.data)) {}
	BlockArray& operator=(BlockArray&& other) { if (this != &other) data = std::move(other.data); return *this; }

	const Block& Get(InChunkPos position) const { return (*data)[position.x][position.y][position.z]; };
	void Set(InChunkPos position, Block block) { (*data)[position.x][position.y][position.z] = block; };

	static inline constexpr bool PositionOnBorder(InChunkPos position);
	static inline constexpr bool PositionInBounds(InChunkPos position);

private:
	std::unique_ptr<Block[ChunkSize.x][ChunkSize.y][ChunkSize.z]> data;
};

constexpr bool BlockArray::PositionOnBorder(InChunkPos position)
{
	return position.x == 0 || position.x == ChunkSize.x - 1 ||
		position.y == 0 || position.y == ChunkSize.y - 1 ||
		position.z == 0 || position.z == ChunkSize.z - 1;
}

constexpr bool BlockArray::PositionInBounds(InChunkPos position)
{
	return position.x < ChunkSize.x &&
		position.y < ChunkSize.y &&
		position.z < ChunkSize.z;
}