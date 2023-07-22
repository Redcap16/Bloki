#pragma once

#include <world/Block.hpp>
#include <stdexcept>
#include <mutex>

typedef glm::ivec3 InChunkPos;

struct BlockArray
{
	static constexpr glm::ivec3 ChunkSize = { 16, 64, 16 };

	BlockArray() : m_Data(std::make_unique<Array>()) { }
	BlockArray(const BlockArray&) = delete;
	BlockArray& operator=(const BlockArray&) = delete;
	BlockArray(BlockArray&& other) noexcept : m_Data(std::move(other.m_Data)) {}
	BlockArray& operator=(BlockArray&& other) noexcept { if (this != &other) m_Data = std::move(other.m_Data); return *this; }

	const Block& Get(InChunkPos position) const { return m_Data->Data[position.x][position.y][position.z]; };
	void Set(InChunkPos position, Block block) { m_Data->Data[position.x][position.y][position.z] = block; };
	void Swap(BlockArray& other) { m_Data.swap(other.m_Data); }

	static inline constexpr bool PositionOnBorder(InChunkPos position);
	static inline constexpr bool PositionInBounds(InChunkPos position);

private:
	struct Array
	{
		Block Data[ChunkSize.x][ChunkSize.y][ChunkSize.z];
	};

	std::unique_ptr<Array> m_Data;
};

constexpr bool BlockArray::PositionOnBorder(InChunkPos position)
{
	return position.x == 0 || position.x == ChunkSize.x - 1 ||
		position.y == 0 || position.y == ChunkSize.y - 1 ||
		position.z == 0 || position.z == ChunkSize.z - 1;
}

constexpr bool BlockArray::PositionInBounds(InChunkPos position)
{
	return position.x >= 0 && position.x < ChunkSize.x &&
		position.y >= 0 && position.y < ChunkSize.y &&
		position.z >= 0 && position.z < ChunkSize.z;
}