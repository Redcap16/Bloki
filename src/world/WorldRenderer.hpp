#pragma once

#include <world/Chunk.hpp>
#include <world/ChunkRenderer.hpp>
#include <world/LoadedChunks.hpp>
#include <core/Player.hpp>

#include <set>
#include <memory>
#include <thread>
#include <queue>

class WorldRenderer : public ChunkEventListener {
public:
	WorldRenderer(Renderer3D& renderer, LoadedChunks& loadedChunks, const Player& player);
	~WorldRenderer();

	void SetChunksToRender(std::set<Chunk*> chunksToRender);

	void ChunkLoaded(const glm::ivec3& chunkPosition) override;
	void ChunkUnloaded(const glm::ivec3& chunkPosition) override;
	void ChunkUpdated(const glm::ivec3& chunkPosition) override;

	void Update();
private:
	class RendererUpdater {
	public:
		RendererUpdater();
		~RendererUpdater();
		RendererUpdater(const RendererUpdater&) = delete;
		RendererUpdater& operator=(const RendererUpdater&) = delete;
		RendererUpdater (RendererUpdater&&) = delete;
		RendererUpdater& operator=(RendererUpdater&&) = delete;

		void AddUpdateTask(const glm::ivec3& position, std::weak_ptr<ChunkRenderer>&& chunkRenderer);
	private:
		std::unordered_map<glm::ivec3, std::weak_ptr<ChunkRenderer>> m_ChunkRenderers; //To avoid adding same renderer multiple times
		std::queue<glm::ivec3> m_UpdateQueue;
		std::mutex m_UpdateQueueMutex;
		std::condition_variable m_UpdateQueueCV;
		std::thread m_UpdateThread;
		bool m_Done = false;

		void threadLoop();
		void updateRenderer(ChunkRenderer& renderer);
	};

	std::unordered_map<glm::ivec3, std::shared_ptr<ChunkRenderer>> m_ChunkRenderers;

	Renderer3D& m_Renderer;
	LoadedChunks& m_LoadedChunks;
	const Player& m_Player;

	RendererUpdater m_Updater;

	glm::ivec3 m_LastHighlightedPosition;
	bool m_LastHighlighted;

	void createRenderers(std::set<const Chunk*>& chunks);
	void removeRenderers(std::set<const Chunk*>& chunks);
	void updateHighlight();
};