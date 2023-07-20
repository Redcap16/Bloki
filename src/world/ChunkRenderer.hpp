#pragma once

#include <glm/glm.hpp>
#include <world/ChunkMesh.hpp>
#include <core/Renderer.hpp>
#include <world/Chunk.hpp>

#include <array>

struct ChunkResources
{
public:
	AtlasTexture* BlockTexture;
	AtlasTexture::SubTexture TextureCoords[Block::c_BlockCount];
	ShaderProgram* OpaqueShader;
	ShaderProgram* TransparentShader;

	static ChunkResources& GetInstance();
	bool IsOpen() const { return m_IsOpen; }
private:
	const char* c_TextureFilename = "block.td";
	const char* c_OpaqueShaderFilename = "chunk-solid.shader";
	const char* c_TransparentShaderFilename = "chunk-transparent.shader";

	bool m_IsOpen;
	ChunkResources();

	bool setupTexture();
	bool setupShaders();
};

class ChunkRenderer
{
public:
	ChunkRenderer(Renderer3D& renderer, std::shared_ptr<const Chunk> chunk);

	void SetHighlight(InChunkPos position);
	void ResetHighlight();

	const Chunk* GetChunk() const { return m_Chunk.get(); }

	void UpdateGeometry();
private:
	glm::vec3 m_Position;
	ChunkMesh m_OpaqueMesh,
		m_TransparentMesh;

	const std::array<const Chunk*, 6>& m_Neighbors;
	std::shared_ptr<const Chunk> m_Chunk;
	Chunk::BlockAccess<const BlockArray>* m_CurrentBlockAccess;

	InChunkPos m_HighlightedPosition;
	bool m_AnythingHighlighted;

	void processBlock(InChunkPos position);
	bool isBlockVisible(InChunkPos position, Direction direction);
	const Chunk* getNeighbor(glm::ivec3 position) const;
};