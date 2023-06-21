#include <world/ChunkManager.hpp>

//#include <world/World.hpp>

ChunkManager::ChunkForGeneration::ChunkForGeneration(Chunk* chunk, glm::ivec3 position) :
	chunk(chunk),
	position(position)
{

}

ChunkManager::ChunkForGeneration::ChunkForGeneration() :
	chunk(nullptr),
	position(glm::ivec3(0))
{

}

void ChunkManager::setupThread()
{
	/*InitializeCriticalSection(&cSection);

	ThreadData *td = (ThreadData*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ThreadData));
	tdp = td;
	td->instance = this;
	td->renderingContext = Window::GetInstance().CreateRenderingContext();
	threadHandle = CreateThread(NULL, 0, threadStartPoint, td, 0, &threadId);

	if (threadHandle == NULL)
	{
		Debug::GetInstance().Log("Error: cant create new thread");
		return;
	}*/


	renderingContext = Window::GetInstance().CreateRenderingContext();
	thread = std::thread(&ChunkManager::threadLoop, this); 
}

void ChunkManager::destroyThread()
{
	running = false;
	/*WaitForSingleObject(threadHandle, INFINITE);
	CloseHandle(threadHandle);
	HeapFree(GetProcessHeap(), 0, td);
	td = nullptr;

	DeleteCriticalSection(&cSection);*/
	thread.join();
}

/*DWORD WINAPI ChunkManager::threadStartPoint(LPVOID param)
{
	if (param == nullptr)
		return 1;
	ThreadData* td = (ThreadData*)param;
	wglMakeCurrent(Window::GetInstance().GetDeviceContext(), td->renderingContext);

	td->instance->threadLoop();
}*/

void ChunkManager::reloadChunks(glm::ivec3 newCenter)
{
	DebugBox::GetInstance().ChangeDebugValue("center", std::to_string(centerChunk.x) + " " + std::to_string(centerChunk.y) + " " + std::to_string(centerChunk.z));
	
	Chunk* newChunks[chunkArrayX][chunkArrayY][chunkArrayZ] = { nullptr };
	std::vector<Chunk*> buffer;
	for (int x = 0; x < chunkArrayX; x++)
		for (int y = 0; y < chunkArrayY; y++)
			for (int z = 0; z < chunkArrayZ; z++)
			{
				const glm::ivec3 halfArea = {
					(((int)chunkArrayX - 1) / 2),
					(((int)chunkArrayY - 1) / 2),
					(((int)chunkArrayZ - 1) / 2)
				};

				const glm::ivec3 chunkPosition = glm::ivec3(
					(int)centerChunk.x + x,
					(int)centerChunk.y + y,
					(int)centerChunk.z + z) - halfArea;

				const glm::ivec3 chunkPositionInNewArray = chunkPosition - newCenter + halfArea;

				if (chunkPositionInNewArray.x <= (int)chunkArrayX - 1 &&
					chunkPositionInNewArray.x >= 0 &&
					chunkPositionInNewArray.y <= (int)chunkArrayY - 1 &&
					chunkPositionInNewArray.y >= 0 &&
					chunkPositionInNewArray.z <= (int)chunkArrayZ - 1 &&
					chunkPositionInNewArray.z >= 0) //It is in new array
				{
					Chunk*& loadedChunk = loadedChunks[x][y][z];
					newChunks[chunkPositionInNewArray.x][chunkPositionInNewArray.y][chunkPositionInNewArray.z] = loadedChunk;
				}
				else //If not, load it
				{
					buffer.push_back(loadedChunks[x][y][z]);
				}
			}
	for (unsigned int x = 0; x < chunkArrayX; x++)
		for (unsigned int y = 0; y < chunkArrayY; y++)
			for (unsigned int z = 0; z < chunkArrayZ; z++)
			{
				Chunk*& chunk = newChunks[x][y][z];
				if (chunk == nullptr)
				{
					if (buffer.empty())
					{
						std::cout << "Something went wrong on chunk reloading..." << std::endl;
						return;
					}

					chunk = buffer.back();
					buffer.pop_back();

					const glm::ivec3 halfArea = {
					(((int)chunkArrayX - 1) / 2),
					(((int)chunkArrayY - 1) / 2),
					(((int)chunkArrayZ - 1) / 2)
					};

					const glm::ivec3 chunkPosition = glm::ivec3(
						(int)centerChunk.x + x,
						(int)centerChunk.y + y,
						(int)centerChunk.z + z) - halfArea;

					reloadChunk(chunkPosition, chunk);
				}

				loadedChunks[x][y][z] = newChunks[x][y][z];
			}

	if(!buffer.empty())
	{
		std::cout << "Something went wrong on chunk reloading..." << std::endl;
		return;
	}
	/*
	bool useOld[chunkArrayX][chunkArrayY][chunkArrayZ] = { false };
	Chunk* newChunks[chunkArrayX][chunkArrayY][chunkArrayZ];
	for (unsigned int x = 0; x < chunkArrayX; x++)
		for (unsigned int y = 0; y < chunkArrayY; y++)
			for (unsigned int z = 0; z < chunkArrayZ; z++)
			{
				const glm::ivec3 chunkPosition = {
					newCenter.x + x - (((int)chunkArrayX - 1) / 2),
					newCenter.y + y - (((int)chunkArrayY - 1) / 2),
					newCenter.z + z - (((int)chunkArrayZ - 1) / 2),
				};
				const glm::ivec3 halfArea = {
					(((int)chunkArrayX - 1) / 2),
					(((int)chunkArrayY - 1) / 2),
					(((int)chunkArrayZ - 1) / 2)
				};

				const glm::ivec3 chunkPositionInOldArray = chunkPosition - centerChunk + halfArea;

				if (chunkPositionInOldArray.x <= (int)chunkArrayX - 1 &&
					chunkPositionInOldArray.x >= 0 &&
					chunkPositionInOldArray.y <= (int)chunkArrayY - 1 &&
					chunkPositionInOldArray.y >= 0 &&
					chunkPositionInOldArray.z <= (int)chunkArrayZ - 1 &&
					chunkPositionInOldArray.z >= 0) //It is in old array
				{
					Chunk*& loadedChunk = loadedChunks[chunkPositionInOldArray.x][chunkPositionInOldArray.y][chunkPositionInOldArray.z];
					useOld[chunkPositionInOldArray.x][chunkPositionInOldArray.y][chunkPositionInOldArray.z] = true;
					newChunks[x][y][z] = loadedChunk;
				}
				else //If not, load it
				{
					//newChunks[x][y][z] = loadChunk(chunkPosition);
					//newChunks[x][y][z] = loadedChunk;
					reloadChunk({ x, y, z }, loadedChunk);
				}
			}

	for (unsigned int x = 0; x < chunkArrayX; x++)
		for (unsigned int y = 0; y < chunkArrayY; y++)
			for (unsigned int z = 0; z < chunkArrayZ; z++)
			{
				//Chunk* old = loadedChunks[x][y][z];
				loadedChunks[x][y][z] = newChunks[x][y][z]; //Replace old by new

				//if (!useOld[x][y][z])
				//	unloadChunk(old);
			}*/

	centerChunk = newCenter;
}

void ChunkManager::loadChunks()
{
	for (unsigned int x = 0; x < chunkArrayX; x++)
		for (unsigned int y = 0; y < chunkArrayY; y++)
			for (unsigned int z = 0; z < chunkArrayZ; z++)
			{
				const glm::ivec3 chunkPosition = glm::ivec3(x, y, z) + centerChunk - glm::ivec3(((chunkArrayX - 1) / 2), ((chunkArrayY - 1) / 2), ((chunkArrayZ - 1) / 2));
	
				//Chunk *old = loadedChunks[x][y][z];
				loadedChunks[x][y][z] = loadChunk(chunkPosition);	
				
				//if (old != nullptr)
				//	unloadChunk(old);
			}
}

void ChunkManager::reloadChunk(glm::ivec3 position, Chunk* chunk)
{
	chunk->Clear(position);
	WorldGenerator::Generate(chunk, position);

	Chunk* neighborhood[6] = { GetChunkByCoord(position + glm::ivec3(1, 0, 0)),
		GetChunkByCoord(position + glm::ivec3(-1, 0, 0)),
		GetChunkByCoord(position + glm::ivec3(0, 1, 0)),
		GetChunkByCoord(position + glm::ivec3(0, -1, 0)),
		GetChunkByCoord(position + glm::ivec3(0, 0, 1)),
		GetChunkByCoord(position + glm::ivec3(0, 0, -1)) };
	chunk->CreateMesh(neighborhood);
}

Chunk* ChunkManager::loadChunk(glm::ivec3 position)
{
	Chunk* chunk = nullptr;
	if (false)
	{
		//load chunk from file
	}
	else
		chunk = generateChunk(position);

	return chunk;
}

void ChunkManager::unloadAll()
{
	for (unsigned int x = 0; x < chunkArrayX; x++)
		for (unsigned int y = 0; y < chunkArrayY; y++)
			for (unsigned int z = 0; z < chunkArrayZ; z++)
				unloadChunk(loadedChunks[x][y][z]);
}

void ChunkManager::unloadChunk(Chunk* chunk)
{
	if (!chunk)
		return;

	if (!chunk->IsLoaded()) //Need to remove from loading queue
		removeFromGeneration(chunk);
	delete chunk;
}

Chunk* ChunkManager::generateChunk(glm::ivec3 position)
{
	Chunk* newChunk = new Chunk(position, *this);

	generationQueue.push_back(ChunkForGeneration(newChunk, position));
	std::cout << "Adding chunk for generation at " << generationQueue.size() << std::endl;

	return newChunk;
}

void ChunkManager::removeFromGeneration(Chunk* chunk)
{
	for (auto it = generationQueue.begin(); it != generationQueue.end(); it++)
	{
		if (it->chunk == chunk)
		{
			generationQueue.erase(it);
			return;
		}
	}
}

void ChunkManager::threadLoop()
{
	//loadChunks();
	wglMakeCurrent(Window::GetInstance().GetDeviceContext(), renderingContext);

	while (running)
	{
		if (centerChunkChanged)
		{
			reloadChunks(newCenterChunk);
			centerChunkChanged = false;
		}
		updateChunks();
		generateChunks();

		GLenum error = glGetError();
		if (error)
			std::cout << "OPENGL ERROR: " << error << std::endl;
		DWORD err2 = GetLastError();
		if (err2)
		{
			std::cout << "WINAPI ERROR T2: " << err2 << std::endl;

			LPWSTR buffer = 0;
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err2, 0, (LPWSTR)&buffer, 0, 0);
			std::wstring ws = buffer;
			std::cout << std::string(ws.begin(), ws.end()) << std::endl;
			LocalFree(buffer);
		}
	}
}

void ChunkManager::updateChunks()
{
	bool force = false;
	if (Window::GetInstance().GetKey('T'))
	{
		Window::GetInstance().SetKey('T', false);
		force = true;
	}

	for (unsigned int x = 0; x < chunkArrayX; x++)
		for (unsigned int y = 0; y < chunkArrayY; y++)
			for (unsigned int z = 0; z < chunkArrayZ; z++)
				if(loadedChunks[x][y][z] != nullptr)
					loadedChunks[x][y][z]->Update(force);
}

void ChunkManager::generateChunks()
{
	if (generationQueue.empty())
		return;

	ChunkForGeneration& chunkfg = generationQueue.front();
	if (chunkfg.chunk == nullptr)
	{
		generationQueue.pop_front();
		return;
	}

	WorldGenerator::Generate(chunkfg.chunk, chunkfg.position); 
	
	Chunk* neighborhood[6] = {GetChunkByCoord(chunkfg.position + glm::ivec3(1, 0, 0)),
		GetChunkByCoord(chunkfg.position + glm::ivec3(-1, 0, 0)),
		GetChunkByCoord(chunkfg.position + glm::ivec3(0, 1, 0)),
		GetChunkByCoord(chunkfg.position + glm::ivec3(0, -1, 0)),
		GetChunkByCoord(chunkfg.position + glm::ivec3(0, 0, 1)),
		GetChunkByCoord(chunkfg.position + glm::ivec3(0, 0, -1)) };
	chunkfg.chunk->CreateMesh(neighborhood);

	generationQueue.pop_front();
}

ChunkManager::ChunkManager() :
	shaderSolid("chunk-solid.shader"),
	shaderTransparent("chunk-transparent.shader")
{
	setupThread();
	loadChunks();
}

ChunkManager::~ChunkManager()
{
	destroyThread();
	unloadAll();
}

Chunk* ChunkManager::GetChunk(glm::ivec3 position)  //GLOBAL POSITION
{
	glm::vec3 chunkSize = { Chunk::chunkSizeXZ, Chunk::chunkSizeY, Chunk::chunkSizeXZ };
	position = glm::floor((glm::vec3)position / chunkSize);

	const glm::ivec3 chunkPositionInArray = position - centerChunk;

	if (chunkPositionInArray.x <= (((int)chunkArrayX - 1) / 2) &&
		chunkPositionInArray.x >= -(((int)chunkArrayX - 1) / 2) &&
		chunkPositionInArray.y <= (((int)chunkArrayY - 1) / 2) &&
		chunkPositionInArray.y >= -(((int)chunkArrayY - 1) / 2) &&
		chunkPositionInArray.z <= (((int)chunkArrayZ - 1) / 2) &&
		chunkPositionInArray.z >= -(((int)chunkArrayZ - 1) / 2)) //It it in array
	{
		return loadedChunks[chunkPositionInArray.x + (((int)chunkArrayX - 1) / 2)]
			[chunkPositionInArray.y + (((int)chunkArrayY - 1) / 2)]
		[chunkPositionInArray.z + (((int)chunkArrayZ - 1) / 2)];
	}

	return nullptr;
}

Chunk* ChunkManager::GetChunkByCoord(glm::ivec3 position) //By chunk position
{
	const glm::ivec3 chunkPosition = position - centerChunk + glm::ivec3(((int)chunkArrayX - 1) / 2,
		((int)chunkArrayY - 1) / 2,
		((int)chunkArrayZ - 1) / 2);

	if (Math::IsBetween(chunkPosition.x, 0, chunkArrayX - 1) &&
		Math::IsBetween(chunkPosition.y, 0, chunkArrayY - 1) &&
		Math::IsBetween(chunkPosition.z, 0, chunkArrayZ - 1))
		return loadedChunks[chunkPosition.x][chunkPosition.y][chunkPosition.z];

	return nullptr;
}

std::pair<Block, bool> ChunkManager::GetBlock(glm::ivec3 position) //GLOBAL POSITION
{
	std::lock_guard<std::mutex> lock(mutex);
	Chunk* chunk = GetChunk(position);
	if (!chunk)
		return { Block(), false };

	return chunk->GetBlock(ToLocalPosition(position));
}

bool ChunkManager::SetBlock(glm::ivec3 position, Block& block, bool update) //GLOBAL POSITION
{
	std::lock_guard<std::mutex> lock(mutex);
	Chunk* chunk = GetChunk(position);
	if (!chunk)
		return false;

	return chunk->SetBlock(ToLocalPosition(position), block, update);
}

void ChunkManager::SetCenterChunk(glm::ivec3 position) //GLOBAL POSITION
{
	newCenterChunk = {
		floor(position.x / (float)Chunk::chunkSizeXZ),
		floor(position.y / (float)Chunk::chunkSizeY),
		floor(position.z / (float)Chunk::chunkSizeXZ),
	};

	if (newCenterChunk != centerChunk)
		centerChunkChanged = true;
}


void ChunkManager::Render(Camera3D& camera, RenderPass pass)
{
	ShaderProgram* currentProgram;
	if (pass == RenderPass::Solid) currentProgram = &shaderSolid;
	else if (pass == RenderPass::Transparent) currentProgram = &shaderTransparent;
	else return;

	currentProgram->UseProgram();

	Block::TextureManager::GetInstance().texture.BindTexture(*currentProgram, "textureAtlas", 0);

	std::lock_guard<std::mutex> lock(mutex);

	for (int x = 0; x < chunkArrayX; x++)
		for (int y = 0; y < chunkArrayY; y++)
			for (int z = 0; z < chunkArrayZ; z++)
			{
				Chunk* const currentChunk = loadedChunks[x][y][z];
				if (currentChunk == nullptr)
					continue;
				const glm::vec3 chunkPos = currentChunk->GetPosition() * glm::ivec3(Chunk::chunkSizeXZ, Chunk::chunkSizeY, Chunk::chunkSizeXZ);

				const glm::mat4 chunkTranslation = glm::translate(glm::mat4(1.0f), chunkPos),
					mvp = camera.GetCameraMatrix() * chunkTranslation;

				currentProgram->SetUniform("mvpMatrix", mvp);

				currentChunk->Render(pass);
			}
}

void ChunkManager::Update()
{
}

glm::ivec3 ChunkManager::ToLocalPosition(glm::ivec3 world)
{
	return {
		world.x % Chunk::chunkSizeXZ >= 0 ? world.x % Chunk::chunkSizeXZ : (world.x % Chunk::chunkSizeXZ) + Chunk::chunkSizeXZ,
		world.y % Chunk::chunkSizeY >= 0 ? world.y % Chunk::chunkSizeY : (world.y % Chunk::chunkSizeY) + Chunk::chunkSizeY,
		world.z % Chunk::chunkSizeXZ >= 0 ? world.z % Chunk::chunkSizeXZ : (world.z % Chunk::chunkSizeXZ) + Chunk::chunkSizeXZ }; //To get modulo not remainder
}

glm::ivec3 ChunkManager::ToWorldPosition(glm::ivec3 chunk, glm::ivec3 local)
{
	return { chunk.x * Chunk::chunkSizeXZ + local.x,
		chunk.y * Chunk::chunkSizeY + local.y,
		chunk.z * Chunk::chunkSizeXZ + local.z };
}

void ChunkManager::SetHighlightedBlock(glm::ivec3 position)
{
	if (highlighted && highlightedBlock == position)
		return; //No need to change anything

	std::lock_guard<std::mutex> lock(mutex);
	if (highlighted)
	{
		Chunk* const chunk = GetChunk(highlightedBlock);
		chunk->DisableHighlightedBlock();
	}
	else
		highlighted = true;

	highlightedBlock = position;

	Chunk* const chunk = GetChunk(highlightedBlock);
	chunk->SetHighlightedBlock(ToLocalPosition(highlightedBlock));
}

void ChunkManager::DisableHighlightedBlock()
{
	if (!highlighted)
		return; //Nothing to do
	std::lock_guard<std::mutex> lock(mutex); 

	Chunk* const chunk = GetChunk(highlightedBlock);
	chunk->DisableHighlightedBlock();
	highlighted = false;
}

std::pair<glm::ivec3, bool> ChunkManager::GetHighlightedBlock()
{
	return { highlightedBlock, highlighted };
}