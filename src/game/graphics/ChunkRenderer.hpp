#pragma once

#include <glm/glm.hpp>
#include <game/graphics/ChunkMesh.hpp>
#include <engine/core/Renderer.hpp>
#include <game/world/Chunk.hpp>

#include <array>

class ChunkRenderer : public ChunkUpdateListener
{
public:
	ChunkRenderer(Renderer3D& renderer, const Chunk& chunk);
	~ChunkRenderer();
	ChunkRenderer(const ChunkRenderer&) = delete;
	ChunkRenderer& operator=(const ChunkRenderer&) = delete;

	void SetHighlight(InChunkPos position);
	void ResetHighlight();

	const Chunk* GetChunk() const { return m_Chunk; }

	bool DoesNeedGeometryUpdate() const { return m_MeshNeedUpdate; }
	void UpdateGeometry();

	void ChunkUpdated(const ChunkPos& chunkPosition) override;
private:
	const char* c_TextureFilename = "block.td";
	const char* c_OpaqueShaderFilename = "chunk-solid.shader";
	const char* c_TransparentShaderFilename = "chunk-transparent.shader";

	Resource<AtlasTexture> m_BlockTexture;
	Resource<ShaderProgram> m_OpaqueShader,
		m_TransparentShader;

	static AtlasTexture::SubTexture s_TextureCoords[Block::c_BlockCount];
	static bool s_TextureCoordsCreated;

	Renderer3D& m_Renderer;

	glm::vec3 m_Position;
	ChunkMesh m_OpaqueMesh,
		m_TransparentMesh;

	const std::array<const Chunk*, 6>& m_Neighbors;
	const Chunk* m_Chunk;
	Chunk::BlockAccess<const BlockArray>* m_CurrentBlockAccess;

	InChunkPos m_HighlightedPosition;
	bool m_AnythingHighlighted;

	bool m_MeshNeedUpdate;

	void processBlock(const InChunkPos& position);
	bool isBlockVisible(Block block, const InChunkPos& position, Direction direction);
	const Chunk* getNeighbor(const glm::ivec3& position) const;

	void loadTextureCoords();
};