#include <game/core/AreaLoader.hpp>

AreaLoader::AreaLoader(WorldRenderer& renderer,
	DroppedItemRepository& itemRepository,
	LoadedChunks& world) :
	m_WorldRenderer(renderer),
	m_ItemRepository(itemRepository),
	m_World(world) {

}

void AreaLoader::SetCenterChunk(ChunkPos chunkPosition) {
	if (m_Loaded && m_CenterChunk == chunkPosition)
		return;

	m_CenterChunk = chunkPosition;

	setChunksToLoad();
	setChunksToRender();

	m_Loaded = true;
}

void AreaLoader::setChunksToLoad() {
	std::unordered_set<ChunkPos> chunksToBeLoaded;
	for (int x = -c_WorldLoadingDistance.x; x <= c_WorldLoadingDistance.x; ++x)
		for (int y = -c_WorldLoadingDistance.y; y <= c_WorldLoadingDistance.y; ++y)
			for (int z = -c_WorldLoadingDistance.z; z <= c_WorldLoadingDistance.z; ++z) {
				chunksToBeLoaded.insert(m_CenterChunk + glm::ivec3(x, y, z));
			}
	m_ItemRepository.SetLoadedChunks(chunksToBeLoaded);
}

void AreaLoader::setChunksToRender() {
	std::set<Chunk*> chunksToRender;
	for (int x = -c_RenderDistance.x; x <= c_RenderDistance.x; ++x)
		for (int y = -c_RenderDistance.y; y <= c_RenderDistance.y; ++y)
			for (int z = -c_RenderDistance.z; z <= c_RenderDistance.z; ++z) {
				Chunk* const chunk = m_World.GetChunk(m_CenterChunk + glm::ivec3(x, y, z));
				if (chunk != nullptr)
					chunksToRender.insert(chunk);
			}

	m_WorldRenderer.SetChunksToRender(chunksToRender);
}