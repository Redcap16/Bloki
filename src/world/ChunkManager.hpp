#pragma once

#include <world/Chunk.hpp>
#include <world/worldGenerator.hpp>
#include <graphics/ShaderProgram.hpp>
#include <graphics/Camera3D.hpp>
#include <ui/DebugBox.hpp>
#include <window/window.h>

//#include <tchar.h>
//#include <strsafe.h>
#include <thread>
#include <mutex>
#include <chrono>

class ChunkManager : ChunkHandler
{
private:
	static const int chunkRadius = 5,
		chunkArrayX = chunkRadius * 2 + 1,
		chunkArrayY = 3,
		chunkArrayZ = chunkArrayX;

	Chunk* loadedChunks[chunkArrayX][chunkArrayY][chunkArrayZ] = { nullptr };
	glm::ivec3 centerChunk,
		newCenterChunk;
	bool centerChunkChanged = false;

	struct ChunkForGeneration
	{
		Chunk* chunk;
		glm::ivec3 position;

		ChunkForGeneration(Chunk* chunk, glm::ivec3 position);
		ChunkForGeneration();
	};

	std::deque<ChunkForGeneration> generationQueue;

	glm::ivec3 highlightedBlock = glm::ivec3(0);
	bool highlighted = false;

	ShaderProgram shaderTransparent;
	ShaderProgram shaderSolid;

	/*struct ThreadData
	{
		HGLRC renderingContext;
		ChunkManager* instance;
	};

	ThreadData* tdp;*/

	bool running = true;
	//HANDLE threadHandle;
	//DWORD threadId;
	//ThreadData* td;
	//CRITICAL_SECTION cSection;
	HGLRC renderingContext;
	std::thread thread;
	std::mutex mutex;

	void setupThread();
	void destroyThread();
	//static DWORD WINAPI threadStartPoint(LPVOID param);
	void reloadChunks(glm::ivec3 newCenter);
	void loadChunks();
	void reloadChunk(glm::ivec3 position, Chunk* chunk);
	Chunk* loadChunk(glm::ivec3 position);
	void unloadAll();
	void unloadChunk(Chunk* chunk);
	Chunk* generateChunk(glm::ivec3 position);
	void removeFromGeneration(Chunk* chunk);
	void threadLoop();
	void updateChunks();
	void generateChunks();
public:
	ChunkManager();
	~ChunkManager();

	Chunk* GetChunk(glm::ivec3 position);  //GLOBAL POSITION
	Chunk* GetChunkByCoord(glm::ivec3 position) override; //By chunk position
	std::pair<Block, bool> GetBlock(glm::ivec3 position); //GLOBAL POSITION
	bool SetBlock(glm::ivec3 position, Block& block, bool update); //GLOBAL POSITION
	void SetCenterChunk(glm::ivec3 position); //GLOBAL POSITION

	void Render(Camera3D& camera, RenderPass pass);
	void Update();

	glm::ivec3 ToLocalPosition(glm::ivec3 world);
	glm::ivec3 ToWorldPosition(glm::ivec3 chunk, glm::ivec3 local);

	void SetHighlightedBlock(glm::ivec3 position);
	void DisableHighlightedBlock();
	std::pair<glm::ivec3, bool> GetHighlightedBlock();
};


vertexArray = VertexArray();
vertexBuffer = VertexBuffer(({GL_FLOAT, 3, GL_FALSE}, {GL_FLOAT, 2, GL_FALSE}));
vertexArray.AddVertexBuffer(&vertexBuffer);
elementBuffer = ElementBuffer();
elementBuffer.AddIndex(5);
vertexArray.SetElemenentBuffer(&elementBuffer);