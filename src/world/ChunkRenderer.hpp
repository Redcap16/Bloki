#pragma once

#include <glm/glm.hpp>
#include <world/ChunkMesh.hpp>
#include <core/Renderer.hpp>

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
	ChunkRenderer(Renderer3D& renderer, const BlockArray& blockArray, const glm::vec3& position);

	void SetHighlight(InChunkPos position);
	void ResetHighlight();
	void UpdateNeighbors(const std::array<const BlockArray*, 6>& neighbors);
	void UpdateGeometry();
private:
	glm::vec3 m_Position;
	ChunkMesh m_OpaqueMesh,
		m_TransparentMesh;
	const BlockArray& m_BlockArray;
	std::array<const BlockArray*, 6> m_Neighbors;

	InChunkPos m_HighlightedPosition;
	bool m_AnythingHighlighted;

	void processBlock(InChunkPos position);
	bool isBlockVisible(InChunkPos position, Direction direction);
	const BlockArray* getNeighbor(glm::ivec3 position);
};