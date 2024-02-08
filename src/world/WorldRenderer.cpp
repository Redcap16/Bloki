#include <world/WorldRenderer.hpp>
#include <algorithm>

WorldRenderer::WorldRenderer(Renderer3D& renderer, LoadedChunks& loadedChunks) :
	m_Renderer(renderer),
	m_LoadedChunks(loadedChunks) {
	m_LoadedChunks.AddChunkListener(this);
}

WorldRenderer::~WorldRenderer() {
	m_LoadedChunks.RemoveChunkListener(this);
}

void WorldRenderer::SetChunksToRender(std::set<Chunk*> chunksToRender) {
	std::set<const Chunk*> currentlyDrawnChunks;
	for (auto& renderer : m_ChunkRenderers)
		currentlyDrawnChunks.insert(renderer->GetChunk());

	std::set<const Chunk*> result;
	std::set_difference(chunksToRender.begin(), chunksToRender.end(), currentlyDrawnChunks.begin(), currentlyDrawnChunks.end(), std::inserter(result, result.begin()));
	createRenderers(result);

	result.clear();
	std::set_difference(currentlyDrawnChunks.begin(), currentlyDrawnChunks.end(), chunksToRender.begin(), chunksToRender.end(), std::inserter(result, result.begin()));
	removeRenderers(result);
}

void WorldRenderer::ChunkLoaded(const glm::ivec3& chunkPosition) {
	//Nothing
}

void WorldRenderer::ChunkUnloaded(const glm::ivec3& chunkPosition) {
	for (auto it = m_ChunkRenderers.begin(); it != m_ChunkRenderers.end(); ++it)
		if ((*it)->GetChunk()->GetPosition() == chunkPosition)
			m_ChunkRenderers.erase(it);
}

void WorldRenderer::ChunkUpdated(const glm::ivec3& chunkPosition) {
	//Nothing
}

void WorldRenderer::Update() {
	for (auto& renderer : m_ChunkRenderers)
		renderer->UpdateGeometry();
}

void WorldRenderer::createRenderers(std::set<const Chunk*>& chunks) {
	for (auto chunk : chunks) {
		auto newOne = m_ChunkRenderers.insert(std::make_unique<ChunkRenderer>(m_Renderer, *chunk));
	}
}

void WorldRenderer::removeRenderers(std::set<const Chunk*>& chunks) {
	for (auto it = m_ChunkRenderers.begin(); it != m_ChunkRenderers.end();) {
		if (chunks.find(const_cast<Chunk*>((*it)->GetChunk())) == chunks.end())
			++it;
		else {
			it = m_ChunkRenderers.erase(it);
		}
	}
}