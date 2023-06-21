#pragma once

#include <world/ChunkManager.hpp>
#include <world/worldGenerator.hpp>
#include <graphics/ShaderProgram.hpp>
#include <graphics/Camera3D.hpp>
#include <window/window.h>

/*class World : ChunkHandler
{
private:
	static const int chunkRadius = 5,
		chunkArrayX = chunkRadius * 2 + 1,
		chunkArrayY = 3,
		chunkArrayZ = chunkArrayX;
	Chunk* loadedChunks[chunkArrayX][chunkArrayY][chunkArrayZ] = { nullptr };
	glm::ivec3 centerChunk;

	glm::ivec3 highlightedBlock = glm::ivec3(0);
	bool highlighted = false;

	ShaderProgram shaderTransparent;
	ShaderProgram shaderSolid;

	void reloadChunks(glm::uvec3 newCenter);
	void loadChunks();
	Chunk* loadChunk(glm::ivec3 position);
	void unloadAll();
	void unloadChunk(Chunk* chunk);
	Chunk* generateChunk(glm::ivec3 position);

	World();
public:
	~World();
	static World& GetInstance();
	static void Setup();

	Chunk* GetChunk(glm::ivec3 position);  //GLOBAL POSITION=
	Chunk* GetChunkByCoord(glm::ivec3 position) override; //By chunk position=
	std::pair<Block, bool> GetBlock(glm::ivec3 position); //GLOBAL POSITION=
	bool SetBlock(glm::ivec3 position, Block& block, bool update); //GLOBAL POSITION=
	void SetCenterChunk(glm::ivec3 position); //GLOBAL POSITION=

	void Update();
	void Render(Camera3D& camera, RenderPass pass);

	glm::ivec3 ToLocalPosition(glm::ivec3 world);
	glm::ivec3 ToWorldPosition(glm::ivec3 chunk, glm::ivec3 local);

	void SetHighlightedBlock(glm::ivec3 position);
	void DisableHighlightedBlock();
	std::pair<glm::ivec3, bool> GetHighlightedBlock();
};*/

class World
{
private:

	World();
public:
	ChunkManager cm;
	~World();
	static World& GetInstance();

	Chunk* GetChunk(glm::ivec3 position) { return cm.GetChunk(position); };  //GLOBAL POSITION
	Chunk* GetChunkByCoord(glm::ivec3 position) { return cm.GetChunkByCoord(position); }; //By chunk position
	std::pair<Block, bool> GetBlock(glm::ivec3 position) { return cm.GetBlock(position); };//GLOBAL POSITION
	bool SetBlock(glm::ivec3 position, Block& block, bool update) { return cm.SetBlock(position, block, update); }; //GLOBAL POSITION
	void SetCenterChunk(glm::ivec3 position) { cm.SetCenterChunk(position); }; //GLOBAL POSITION

	void Render(Camera3D& camera, RenderPass pass) { cm.Render(camera, pass); };

	glm::ivec3 ToLocalPosition(glm::ivec3 world) { return cm.ToLocalPosition(world); }
	glm::ivec3 ToWorldPosition(glm::ivec3 chunk, glm::ivec3 local) { return cm.ToWorldPosition(chunk, local); };

	void SetHighlightedBlock(glm::ivec3 position) { cm.SetHighlightedBlock(position); };
	void DisableHighlightedBlock() { cm.DisableHighlightedBlock(); };
	std::pair<glm::ivec3, bool> GetHighlightedBlock() { return cm.GetHighlightedBlock(); };
};