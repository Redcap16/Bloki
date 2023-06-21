#include <world/worldGenerator.hpp>

WorldGenerator::WorldGenerator()
{
}

void WorldGenerator::generatingLoop()
{
	wglMakeCurrent(Window::GetInstance().GetDeviceContext(), renderingContext);
	while (running)
	{
		Step();
	}

	return;
}

DWORD WINAPI WorldGenerator::threadStart(LPVOID lpParam)
{
	GetInstance().generatingLoop();
	return 0;
}

WorldGenerator::~WorldGenerator()
{
	running = false;
	WaitForSingleObject(threadHandle, INFINITE);
	CloseHandle(threadHandle);
}

WorldGenerator& WorldGenerator::GetInstance()
{
	static WorldGenerator instance;
	return instance;
}

void WorldGenerator::Setup()
{
	running = true;
	renderingContext = Window::GetInstance().CreateRenderingContext();
	threadHandle = CreateThread(NULL, 0, threadStart, nullptr, 0, &threadId);
	if (threadHandle == NULL)
	{
		Debug::GetInstance().Log("Error: Cant create worldgen thread");
		return;
	}
}

void WorldGenerator::Generate(Chunk* chunk, glm::ivec3 position)
{
	/*GeneratedChunk gc;
	gc.chunk = chunk;
	gc.position = position;
	generationQueue.push_back(gc);
	std::cout << "Adding chunk for generation at " << generationQueue.size() << std::endl;*/

	static const Block::BlockType airID = Block::GetIdByName("Air"),
		grassID = Block::GetIdByName("Grass"),
		mudID = Block::GetIdByName("Mud"),
		stoneID = Block::GetIdByName("Stone"),
		waterID = Block::GetIdByName("Water");

	if (position.y <= 0) //not only air
	{
		for (int x = 0; x < Chunk::chunkSizeXZ; x++)
		{
			for (int z = 0; z < Chunk::chunkSizeXZ; z++)
			{
				float perlinx = fmod((position.x * (float)Chunk::chunkSizeXZ + (float)x) / 60.0f, 256),
					perlinz = fmod((position.z * (float)Chunk::chunkSizeXZ + (float)z) / 60.0f, 256);
				if (perlinx < 0)
					perlinx += 256;
				if (perlinz < 0)
					perlinz += 256;
				int height = (db::perlin<float>(perlinx, perlinz) + 1.0f) * 30 + 1;
				for (int y = 0; y < Chunk::chunkSizeY; y++)
				{
					Block block = Block(airID);

					if (position.y < 0)
						block.type = stoneID;
					else if (position.y == 0)
					{
						if (y < 25 && y > height)
							block.type = waterID;
						else if (y == height)
							block.type = grassID;
						else if (y >= height - 3 && y < height)
							block.type = mudID;
						else if (y < height - 3)
							block.type = stoneID;
					}

					if (block.type != airID)
						chunk->SetBlock(glm::uvec3(x, y, z), block, false);
				}
			}
		}
	}

	return;
}

bool WorldGenerator::RemoveFromGeneration(Chunk* chunk)
{
	for (auto it = generationQueue.begin(); it != generationQueue.end(); it++)
	{
		if (it->chunk == chunk)
		{
			generationQueue.erase(it);
			return true;
		}
	}

	return false;
}

void WorldGenerator::Step()
{
	if (generationQueue.empty())
		return;

	GeneratedChunk& chunk = generationQueue.front();

	static const Block::BlockType airID = Block::GetIdByName("Air"),
		grassID = Block::GetIdByName("Grass"),
		mudID = Block::GetIdByName("Mud"),
		stoneID = Block::GetIdByName("Stone"),
		waterID = Block::GetIdByName("Water");

	if (chunk.position.y <= 0) //not only air
	{
		for (int x = 0; x < Chunk::chunkSizeXZ; x++)
		{
			for (int z = 0; z < Chunk::chunkSizeXZ; z++)
			{
				float perlinx = fmod((chunk.position.x * (float)Chunk::chunkSizeXZ + (float)x) / 60.0f, 256),
					perlinz = fmod((chunk.position.z * (float)Chunk::chunkSizeXZ + (float)z) / 60.0f, 256);
				if (perlinx < 0)
					perlinx += 256;
				if (perlinz < 0)
					perlinz += 256;
				int height = (db::perlin<float>(perlinx, perlinz) + 1.0f) * 30 + 1;
				for (int y = 0; y < Chunk::chunkSizeY; y++)
				{
					Block block = Block(airID);

					if (chunk.position.y < 0)
						block.type = stoneID;
					else if (chunk.position.y == 0)
					{
						if (y < 25 && y > height)
							block.type = waterID;
						else if (y == height)
							block.type = grassID;
						else if (y >= height - 3 && y < height)
							block.type = mudID;
						else if (y < height - 3)
							block.type = stoneID;
					}

					if (block.type != airID)
						chunk.chunk->SetBlock(glm::uvec3(x, y, z), block, false);
				}
			}
		}
		chunk.chunk->CreateMesh();
	}

	generationQueue.pop_front();
	Debug::GetInstance().Log("Done generating at " + std::to_string(generationQueue.size()));
}