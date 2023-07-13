#pragma once

#include <world/Chunk.hpp>

class ChunkFileLoader
{
public:
	struct RawChunk
	{
		std::unique_ptr<Chunk> Data;
		bool Finished;
	};

	bool LoadChunk(const ChunkPos& position, RawChunk& chunk);
	bool IsPresent(const ChunkPos& position);
	void SaveChunk(const ChunkPos& position, RawChunk chunk);
};