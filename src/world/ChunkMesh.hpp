#pragma once

#include <graphics/Vertex.hpp>
#include <graphics/VertexArray.hpp>
#include <graphics/ShaderProgram.hpp>
#include <core/Renderer.hpp>
#include <world/BlockArray.hpp>
#include <world/Block.hpp>
#include <graphics/ResourceManager.hpp>
#include <util/Direction.hpp>
#include <string>
#include <thread>

class ChunkMesh : public Renderable
{
public:
	ChunkMesh(glm::vec3& position, const AtlasTexture::SubTexture (&textureCoords)[Block::c_BlockCount]);

	void AddFace(Direction dir, InChunkPos position, Block block, BlockState state);
	void FinishGeometry() { m_GeometryFinished = true; }
	
	glm::mat4 GetModelMatrix() const override;
	void Render(const RenderingContext& context) override;
private:
	VertexArray m_MeshVAO;
	VertexBuffer<Vertex3DS> m_MeshVBO;
	ElementBuffer m_MeshEBO;

	std::atomic<bool> m_GeometryFinished;

	glm::vec3& m_Position;
	const AtlasTexture::SubTexture(&m_TextureCoords)[Block::c_BlockCount];

	void updateBuffers();
};