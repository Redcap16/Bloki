#pragma once

#include <world/Chunk.hpp>
#include <world/ChunkRenderer.hpp>
#include <world/LoadedChunks.hpp>
#include <core/Player.hpp>

#include <set>
#include <memory>

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
	std::set<std::unique_ptr<ChunkRenderer>> m_ChunkRenderers;

	Renderer3D& m_Renderer;
	LoadedChunks& m_LoadedChunks;
	const Player& m_Player;

	void createRenderers(std::set<const Chunk*>& chunks);
	void removeRenderers(std::set<const Chunk*>& chunks);
};