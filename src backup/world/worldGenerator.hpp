#pragma once

#include <deque>

#include <world/Chunk.hpp>
#include <window/window.h>

#include <util/db_perlin.h>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

class WorldGenerator
{
private:
	const unsigned int stepSize = 40960;

	struct GeneratedChunk
	{
		Chunk* chunk;
		glm::ivec3 position;
	};

	bool running = false;
	HANDLE threadHandle;
	DWORD threadId;
	HGLRC renderingContext;

	std::deque<GeneratedChunk> generationQueue;

	WorldGenerator();
	void generatingLoop();
	static DWORD WINAPI threadStart(LPVOID lpParam);
public:
	~WorldGenerator();

	static WorldGenerator& GetInstance();
	void Setup();
	static void Generate(Chunk* chunk, glm::ivec3 position);
	bool RemoveFromGeneration(Chunk* chunk);
	void Step();
	inline bool IsDone()
	{
		return generationQueue.empty();
	}
};