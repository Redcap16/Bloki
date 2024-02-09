#include <world/WorldRenderer.hpp>
#include <algorithm>

WorldRenderer::WorldRenderer(Renderer3D& renderer, LoadedChunks& loadedChunks, const Player& player) :
	m_Renderer(renderer),
	m_LoadedChunks(loadedChunks),
	m_Player(player) {
	m_LoadedChunks.AddChunkListener(this);
}

WorldRenderer::~WorldRenderer() {
	m_LoadedChunks.RemoveChunkListener(this);
}

void WorldRenderer::SetChunksToRender(std::set<Chunk*> chunksToRender) {
	std::set<const Chunk*> currentlyDrawnChunks;
	for (auto& renderer : m_ChunkRenderers)
		currentlyDrawnChunks.insert(renderer.second->GetChunk());

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
	m_ChunkRenderers.erase(chunkPosition);
}

void WorldRenderer::ChunkUpdated(const glm::ivec3& chunkPosition) {
	//Nothing
}

void WorldRenderer::Update() {
	updateHighlight();

	for (auto& renderer : m_ChunkRenderers)
		renderer.second->UpdateGeometry();
}

void WorldRenderer::createRenderers(std::set<const Chunk*>& chunks) {
	for (auto chunk : chunks) {
		m_ChunkRenderers[chunk->GetPosition()] = std::make_unique<ChunkRenderer>(m_Renderer, *chunk);
	}
}

void WorldRenderer::removeRenderers(std::set<const Chunk*>& chunks) {
	for (auto it = m_ChunkRenderers.begin(); it != m_ChunkRenderers.end();) {
		if (chunks.find(const_cast<Chunk*>(it->second->GetChunk())) == chunks.end())
			++it;
		else {
			it = m_ChunkRenderers.erase(it);
		}
	}
}

void WorldRenderer::updateHighlight() {
	if (m_LastHighlighted != m_Player.IsPointingAtAnything() ||
		m_LastHighlightedPosition != m_Player.GetPointingAt()) {

		if (m_LastHighlighted)
			m_ChunkRenderers[Chunk::GetChunkPosition(m_LastHighlightedPosition)]->ResetHighlight();

		m_LastHighlighted = m_Player.IsPointingAtAnything();
		m_LastHighlightedPosition = m_Player.GetPointingAt();
		if (m_LastHighlighted)
			m_ChunkRenderers[Chunk::GetChunkPosition(m_LastHighlightedPosition)]->SetHighlight(Chunk::GetInChunkPosition(m_LastHighlightedPosition));
	}
}