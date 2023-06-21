#pragma once

#include <graphics/Vertex.hpp>
#include <graphics/VertexBuffer.hpp>
#include <graphics/ShaderProgram.hpp>
#include <world/ChunkBlockData.hpp>
#include <world/Block.hpp>
#include <graphics/Camera3D.hpp>
#include <graphics/ResourceManager.hpp>
#include <string>

struct BlockSubtextures
{
	AtlasTexture::SubTexture TextureCoords[Block::blockCount];
};

class ChunkMesh
{
public:
	ChunkMesh(ChunkBlockData &blockData, const BlockSubtextures& blockSubtextures);
	~ChunkMesh();

	void SetHighlighted(glm::uvec3 position);
	void UnsetHighlight();
	void CreateMesh();

	void UpdateNeighbors(const ChunkNeighbors& neighbors);

	void UpdateBuffers();
	void Render(bool transparent);

private:
	VertexBuffer<Vertex3D> m_OpaqueMesh;
	VertexBuffer<Vertex3D> m_TransparentMesh;

	ChunkBlockData& blockData;
	ChunkNeighbors neighborArray;

	AtlasTexture* m_BlockTexture;
	const BlockSubtextures& m_BlockSubtextures;

	bool isHighlighted = false;
	glm::uvec3 highlightedPos = glm::uvec3(0);

	bool isBlockVisible(const Block& block, const Block& itsNeighbor);
	bool isBlockOnBorderVisible(ChunkBlockData* neighboringChunk, glm::uvec3 neighborBlockPosition, const Block& block);
	void processFace(FaceDirection direction, glm::uvec3 position, const Block& block, bool highlighted);
	void processBlock(glm::uvec3 position);
};