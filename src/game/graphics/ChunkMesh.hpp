#pragma once

#include <engine/graphics/Vertex.hpp>
#include <engine/graphics/VertexArray.hpp>
#include <engine/graphics/ShaderProgram.hpp>
#include <engine/graphics/AtlasTexture.hpp>
#include <engine/core/Renderer.hpp>
#include <game/world/BlockArray.hpp>
#include <game/world/Block.hpp>
#include <engine/util/Direction.hpp>
#include <string>
#include <thread>

class ChunkMesh : public Renderable
{
public:
	ChunkMesh(glm::vec3& position, bool transparent);

	void ClearMesh();
	void AddFace(Direction dir, InChunkPos position, Block block, BlockState state);
	void FinishGeometry();
	
	glm::mat4 GetModelMatrix() const override;
	glm::vec3 GetRenderPosition() const override { return m_Position; }
	void Render(const RenderingContext& context) override;
private:
	static constexpr glm::ivec3 s_FaceNormals[6] = {
		{0, 1, 0},
		{0, -1, 0},
		{1, 0, 0},
		{-1, 0, 0},
		{0, 0, 1},
		{0, 0, -1} };

	struct Face {
		Direction direction;
		InChunkPos position;
		Block block;
		BlockState state;

		glm::vec3 GetFaceCenterPosition() const;
	};

	class FaceCompare {
	private:
		const glm::vec3 c_CameraPosition;
	public:
		FaceCompare(glm::vec3 cameraPos) : c_CameraPosition(cameraPos) {};

		bool operator()(const Face& a, const Face& b);
	};
	std::vector<Face> m_FaceBuffer;
	std::mutex m_FaceBufferMutex;

	VertexArray m_MeshVAO;
	VertexBuffer<Vertex3DS>& m_MeshVBO;
	ElementBuffer& m_MeshEBO;

	const bool m_Transparent;

	std::atomic<bool> m_GeometryFinished;
	bool m_BuffersNeedUpdate = false;

	glm::vec3& m_Position;

	void updateBuffers();
	void sortFaces(glm::vec3 cameraPosition);
	void processFaces();
};