#include <game/graphics/WorldRenderer.hpp>
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
		if (renderer.second->DoesNeedGeometryUpdate())
			m_Updater.AddUpdateTask(renderer.first, renderer.second);
}

WorldRenderer::RendererUpdater::RendererUpdater() :
	m_UpdateThread(&WorldRenderer::RendererUpdater::threadLoop, this) {
	
}

WorldRenderer::RendererUpdater::~RendererUpdater() {
	m_Done = true;
	m_UpdateQueueCV.notify_one();
	m_UpdateThread.join();
}

void WorldRenderer::RendererUpdater::AddUpdateTask(const glm::ivec3& position, std::weak_ptr<ChunkRenderer>&& chunkRenderer) {
	std::lock_guard<std::mutex> lock(m_UpdateQueueMutex);
	if (m_ChunkRenderers.find(position) != m_ChunkRenderers.end()) //It already is in the update queue
		return;

	m_ChunkRenderers[position] = chunkRenderer;
	m_UpdateQueue.push(position);
	m_UpdateQueueCV.notify_one();
}

void WorldRenderer::RendererUpdater::threadLoop() {
	while (!m_Done) {
		std::unique_lock<std::mutex> lock(m_UpdateQueueMutex);
		if (m_UpdateQueue.empty()) {
			m_UpdateQueueCV.wait(lock);
			continue;
		}

		const glm::ivec3& processedPosition = m_UpdateQueue.front();
		std::shared_ptr<ChunkRenderer> rendererToUpdate = m_ChunkRenderers[processedPosition].lock();
		//Clear
		m_ChunkRenderers.erase(processedPosition);
		m_UpdateQueue.pop();
		lock.unlock();

		if (!rendererToUpdate) //Check if expired
			continue;

		updateRenderer(*rendererToUpdate);
	}
}

void WorldRenderer::RendererUpdater::updateRenderer(ChunkRenderer& renderer) {
	renderer.UpdateGeometry();
}

void WorldRenderer::createRenderers(std::set<const Chunk*>& chunks) {
	for (auto chunk : chunks) {
		m_ChunkRenderers[chunk->GetPosition()] = std::make_shared<ChunkRenderer>(m_Renderer, *chunk);
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