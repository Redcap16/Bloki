#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <graphics/RenderPass.hpp>
#include <graphics/Vertex.hpp>
#include <graphics/VertexBuffer.hpp>
#include <world/Block.hpp>
#include <world/ChunkMesh.hpp>

class Chunk;

class ChunkHandler
{
public:
	virtual Chunk* GetChunkByCoord(glm::ivec3 position) = 0;
};

class Chunk
{
public:
	static const int chunkSizeXZ = 16,
		chunkSizeY = 64;
protected:
	glm::ivec3 position;
	ChunkHandler& world;

	ChunkMesh chunkMesh;
	/*Block block[chunkSizeXZ][chunkSizeY][chunkSizeXZ];

	VertexBuffer<Vertex3D> vboSolid;
	VertexBuffer<Vertex3DT> vboTransparent;

	glm::ivec3 highlightedBlock = glm::ivec3(0);
	bool highlighted = false;
	const float highlightedLight = 0.3f;

	bool toUpdate = false,
		loaded = false;

	bool blockVisible(Block::BlockType type, Block::BlockType neighbor);
	void pushFace(const unsigned int vertexIndices[4],
		glm::ivec3 position,
		Block::BlockType type,
		bool highlighted);
	void pushBlock(glm::ivec3 position, Block& current, Chunk* neighborhood[6]);*/

public:
	Chunk(glm::ivec3 position, ChunkHandler& world);

	//void CreateMesh(Chunk* neighborhood[6] = { nullptr });

	void Update(bool force = false);
	void Render(RenderPass pass);
	//void Clear(glm::ivec3 position);

	/*std::pair<Block, bool> GetBlock(glm::uvec3 pos); //LOCAL POSITION
	bool SetBlock(glm::uvec3 pos, Block& newBlock, bool update = true); //LOCAL POSITION
	void SetHighlightedBlock(glm::ivec3 position);
	void DisableHighlightedBlock();*/

	inline glm::ivec3 GetPosition()
	{
		return position;
	}

	//inline bool IsLoaded() { return loaded; }
};