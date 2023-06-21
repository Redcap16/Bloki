#pragma once

#include <graphics/Vertex.hpp>
#include <graphics/VertexBuffer.hpp>
#include <world/ChunkBlockData.hpp>
#include <world/Block.hpp>

class ChunkMesh
{
private:
	VertexBuffer<Vertex3D> mesh;
	ChunkBlockData& blockData;

	bool isBlockVisible(const Block& block, const Block& itsNeighbor);
	void processFace(FaceDirection direction, glm::uvec3 position, const Block& block);
	void processBlock(glm::uvec3 position);
public:
	ChunkMesh(ChunkBlockData &blockData);
	~ChunkMesh();

	void CreateMesh();
	void UpdateBuffers();

	inline VertexBuffer<Vertex3D> GetVertexBuffer() const
	{
		return mesh;
	}
};