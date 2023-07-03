#pragma once

#include <glm/glm.hpp>
#include <world/Chunk.hpp>
#include <world/ChunkArray.hpp>
#include <core/GameState.hpp>
#include <core/Renderer.hpp>

class ChunkRenderer
{
public:
	ChunkRenderer(ChunkArray& chunkArray, GameState& gameState);

	void RenderChunks(const RenderingContext& context, glm::vec3 position);
	inline const BlockSubtextures& GetBlockSubtextures() const;
private:
	GameState& m_GameState;

	ChunkArray& m_ChunkArray;
	ShaderProgram* m_ShaderProgram;

	static BlockSubtextures m_BlockSubtextures;
	static bool m_TextureSetup;

	AtlasTexture* m_BlockTexture;

	void setupBlockTextures();
	inline glm::vec3 calculateChunkPosition(glm::ivec3 chunkPosition, glm::vec3 centerPosition);
};

glm::vec3 ChunkRenderer::calculateChunkPosition(glm::ivec3 chunkPosition, glm::vec3 centerPosition)
{
	return (glm::vec3)((chunkPosition - (glm::ivec3)ChunkArray::Size / 2) * (glm::ivec3)ChunkBlockData::ChunkSize) + centerPosition;
}

const BlockSubtextures& ChunkRenderer::GetBlockSubtextures() const
{
	return m_BlockSubtextures;
}