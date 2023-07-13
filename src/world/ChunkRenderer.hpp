#pragma once

#include <glm/glm.hpp>
#include <world/Chunk.hpp>
#include <world/ChunkMesh.hpp>
#include <core/Renderer.hpp>

struct ChunkResources
{
public:
	AtlasTexture* BlockTexture;
	AtlasTexture::SubTexture TextureCoords[Block::c_BlockCount];
	ShaderProgram* OpaqueShader;
	ShaderProgram* TransparentShader;

	ChunkResources();
	bool IsOpen() const { return m_IsOpen; }
private:
	const char* c_TextureFilename = "Block.td";
	const char* c_OpaqueShaderFilename = "ChunkOpaque.glsl";
	const char* c_TransparentShaderFilename = "ChunkTransparent.glsl";

	bool m_IsOpen;

	bool setupTexture();
	bool setupShaders();
};

class ChunkRenderer
{
public:
	ChunkRenderer(Renderer3D& renderer, const BlockArray& blockArray, glm::vec3& position);

	void SetHighlight(InChunkPos position);
	void ResetHighlight();
	void UpdateNeighbors(const BlockArray* neighbors[6]);
	void UpdateGeometry();
private:
	static std::unique_ptr<ChunkResources> s_Resources;

	ChunkMesh m_OpaqueMesh,
		m_TransparentMesh;
	const BlockArray& m_BlockArray;
	glm::vec3& m_Position;
	const BlockArray* m_Neighbors[6];

	InChunkPos m_HighlightedPosition;
	bool m_AnythingHighlighted;

	void processBlock(InChunkPos position);
	bool isBlockVisible(InChunkPos position, Direction direction);
	const BlockArray* getNeighbor(glm::ivec3 position);
};