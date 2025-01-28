#pragma once

#include <game/save_loading/WholeSaveLoader.hpp>

class BlockDataLoader {
public:
	BlockDataLoader(WholeSaveLoader& loader);

	bool LoadChunk(Chunk& chunk);
	bool IsPresent(const ChunkPos& position);
	void SaveChunk(const Chunk& chunk);

private:
	WholeSaveLoader& m_Loader;
};