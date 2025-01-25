#pragma once

#include <game/world/Chunk.hpp>
#include <game/entity/DroppedItemRepository.hpp>
#include <game/graphics/WorldRenderer.hpp>

class AreaLoader {
public:
	AreaLoader(WorldRenderer& renderer, 
		DroppedItemRepository& itemRepository, 
		LoadedChunks& world);

	void SetCenterChunk(ChunkPos chunkPosition);
private:
	const glm::ivec3 c_RenderDistance = { 3, 1, 3 };
	const glm::ivec3 c_WorldLoadingDistance = {5, 1, 5};

	WorldRenderer& m_WorldRenderer;
	DroppedItemRepository& m_ItemRepository;
	LoadedChunks& m_World;

	ChunkPos m_CenterChunk;

	bool m_Loaded = false;

	void setChunksToRender();
	void setChunksToLoad();
};