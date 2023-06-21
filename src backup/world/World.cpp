#include <world/World.hpp>
/*
void World::reloadChunks(glm::uvec3 newCenter)
{
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

				const glm::ivec3 chunkPositionInOldArray = chunkPosition - centerChunk;

				if (chunkPositionInOldArray.x <= (((int)chunkArrayX - 1) / 2) &&
					chunkPositionInOldArray.x >= -(((int)chunkArrayX - 1) / 2) &&
					chunkPositionInOldArray.y <= (((int)chunkArrayY - 1) / 2) &&
					chunkPositionInOldArray.y >= -(((int)chunkArrayY - 1) / 2) &&
					chunkPositionInOldArray.z <= (((int)chunkArrayZ - 1) / 2) &&
					chunkPositionInOldArray.z >= -(((int)chunkArrayZ - 1) / 2)) //It it in old array
				{
					Chunk*& loadedChunk = loadedChunks[chunkPositionInOldArray.x + (((int)chunkArrayX - 1) / 2)]
						[chunkPositionInOldArray.y + (((int)chunkArrayY - 1) / 2)]
					[chunkPositionInOldArray.z + (((int)chunkArrayZ - 1) / 2)];
					newChunks[x][y][z] = loadedChunk;
					loadedChunk = 0;
				}
				else //If not, load it
					newChunks[x][y][z] = loadChunk(chunkPosition);
			}

	for (unsigned int x = 0; x < chunkArrayX; x++)
		for (unsigned int y = 0; y < chunkArrayY; y++)
			for (unsigned int z = 0; z < chunkArrayZ; z++)
			{
				Chunk*& loadedChunk = loadedChunks[x][y][z];
				if (loadedChunk)
					unloadChunk(loadedChunk);

				loadedChunk = newChunks[x][y][z]; //Replace old by new
			}

	centerChunk = newCenter;
}

void World::loadChunks()
{
	for (unsigned int x = 0; x < chunkArrayX; x++)
		for (unsigned int y = 0; y < chunkArrayY; y++)
			for (unsigned int z = 0; z < chunkArrayZ; z++)
			{
				const glm::ivec3 chunkPosition = glm::ivec3(x, y, z) + centerChunk - glm::ivec3(((chunkArrayX - 1) / 2), ((chunkArrayY - 1) / 2), ((chunkArrayZ - 1) / 2));
				if (loadedChunks[x][y][z])
					unloadChunk(loadedChunks[x][y][z]);
				loadedChunks[x][y][z] = loadChunk(chunkPosition);
			}
}

Chunk* World::loadChunk(glm::ivec3 position)
{
	Chunk* chunk = nullptr;
	if (false)
	{
		//load chunk from file
	}
	else
		chunk = generateChunk(position);

	//chunk->CreateMesh();
	return chunk;
}

void World::unloadAll()
{
	for (unsigned int x = 0; x < chunkArrayX; x++)
		for (unsigned int y = 0; y < chunkArrayY; y++)
			for (unsigned int z = 0; z < chunkArrayZ; z++)
				unloadChunk(loadedChunks[x][y][z]);
}

void World::unloadChunk(Chunk* chunk)
{
	if (!chunk)
		return;

	if (!chunk->IsLoaded()) //Need to remove from loading queue
		WorldGenerator::GetInstance().RemoveFromGeneration(chunk);
	delete chunk;
}

Chunk* World::generateChunk(glm::ivec3 position)
{
	Chunk* newChunk = new Chunk(position, *this);

	/*if (position.y > 0) //only air
		return newChunk;

	for (unsigned int x = 0; x < Chunk::chunkSizeXZ; x++)
		for (unsigned int z = 0; z < Chunk::chunkSizeXZ; z++)
		{
			float perlinx = fmod((position.x * (float)Chunk::chunkSizeXZ + (float)x) / 60.0f, 256),
				perlinz = fmod((position.z * (float)Chunk::chunkSizeXZ + (float)z) / 60.0f, 256);
			if (perlinx < 0)
				perlinx += 256;
			if (perlinz < 0)
				perlinz += 256;
			int height = (db::perlin<float>(perlinx, perlinz) + 1.0f) * 30 + 1;
			for (unsigned int y = 0; y < Chunk::chunkSizeY; y++)
			{
				Block block = Block(Block::BlockType::AIR);

				if (position.y < 0)
					block.type = Block::BlockType::STONE;
				else if (position.y == 0)
				{
					if (y == height)
						block.type = Block::BlockType::GRASS;
					else if (y >= height - 3 && y < height)
						block.type = Block::BlockType::MUD;
					else if (y < height - 3)
						block.type = Block::BlockType::STONE;
				}

				if (block.type != Block::BlockType::AIR)
					newChunk->SetBlock(glm::uvec3(x, y, z), block);
			}
		}
	if (position.y > 0)
		return new Chunk();*/

		/*ChunkGenerating* newChunk = new ChunkGenerating();
		newChunk->position = position;
		chunkGenerated.push_back(newChunk);*//*
	WorldGenerator::GetInstance().Generate(newChunk, position);

	return newChunk;
}*/

World::World() :
	cm()
{
	//loadChunks();
}

World::~World()
{
	//unloadAll();
}

World& World::GetInstance()
{
	static World instance;
	return instance;
}
/*
void World::Setup()
{
	WorldGenerator::GetInstance().Setup();
}

Chunk* World::GetChunk(glm::ivec3 position)  //GLOBAL POSITION
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

Chunk* World::GetChunkByCoord(glm::ivec3 position) //By chunk position
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

std::pair<Block, bool> World::GetBlock(glm::ivec3 position) //GLOBAL POSITION
{
	Chunk* chunk = GetChunk(position);
	if (!chunk)
		return { Block(), false };

	return chunk->GetBlock(ToLocalPosition(position));
}

bool World::SetBlock(glm::ivec3 position, Block& block, bool update) //GLOBAL POSITION
{
	Chunk* chunk = GetChunk(position);
	if (!chunk)
		return false;

	return chunk->SetBlock(ToLocalPosition(position), block, update);
}

void World::SetCenterChunk(glm::ivec3 position) //GLOBAL POSITION
{
	const glm::ivec3 newCenterChunk = {
		position.x / (int)Chunk::chunkSizeXZ,
		position.y / (int)Chunk::chunkSizeY,
		position.z / (int)Chunk::chunkSizeXZ,
	};

	if (newCenterChunk != centerChunk)
		reloadChunks(newCenterChunk);
}

void World::Update()
{
	//WorldGenerator::GetInstance().Step();

	bool force = false;
	if (Window::GetInstance().GetKey('T'))
	{
		Window::GetInstance().SetKey('T', false);
		force = true;
	}

	for (unsigned int x = 0; x < chunkArrayX; x++)
		for (unsigned int y = 0; y < chunkArrayY; y++)
			for (unsigned int z = 0; z < chunkArrayZ; z++)
				loadedChunks[x][y][z]->Update(force);
}

void World::Render(Camera3D& camera, RenderPass pass)
{
	ShaderProgram* current;
	if (pass == RenderPass::Solid) current = &shaderSolid;
	else if (pass == RenderPass::Transparent) current = &shaderTransparent;
	else return;

	current->UseProgram();

	Block::TextureManager::GetInstance().texture.BindTexture(*current, "textureAtlas", 0);

	for (int x = 0; x < chunkArrayX; x++)
	{
		for (int y = 0; y < chunkArrayY; y++)
		{
			for (int z = 0; z < chunkArrayZ; z++)
			{
				const glm::vec3 chunkPos = {
					(int)Chunk::chunkSizeXZ * (x - ((int)chunkArrayX / 2) + centerChunk.x),
					(int)Chunk::chunkSizeY * (y - ((int)chunkArrayY / 2) + centerChunk.y),
					(int)Chunk::chunkSizeXZ * (z - ((int)chunkArrayZ / 2) + centerChunk.z)
				};

				const glm::mat4 chunkTranslation = glm::translate(glm::mat4(1.0f), chunkPos),
					mvp = camera.GetCameraMatrix() * chunkTranslation;

				current->SetUniform("mvpMatrix", mvp);

				Chunk* const current = loadedChunks[x][y][z];
				current->Render(pass);
			}
		}
	}
}

glm::ivec3 World::ToLocalPosition(glm::ivec3 world)
{
	return {
		world.x % Chunk::chunkSizeXZ >= 0 ? world.x % Chunk::chunkSizeXZ : (world.x % Chunk::chunkSizeXZ) + Chunk::chunkSizeXZ,
		world.y % Chunk::chunkSizeY >= 0 ? world.y % Chunk::chunkSizeY : (world.y % Chunk::chunkSizeY) + Chunk::chunkSizeY,
		world.z % Chunk::chunkSizeXZ >= 0 ? world.z % Chunk::chunkSizeXZ : (world.z % Chunk::chunkSizeXZ) + Chunk::chunkSizeXZ }; //To get modulo not remainder
}

glm::ivec3 World::ToWorldPosition(glm::ivec3 chunk, glm::ivec3 local)
{
	return { chunk.x * Chunk::chunkSizeXZ + local.x,
		chunk.y * Chunk::chunkSizeY + local.y,
		chunk.z * Chunk::chunkSizeXZ + local.z };
}

void World::SetHighlightedBlock(glm::ivec3 position)
{
	if (highlighted && highlightedBlock == position)
		return; //No need to change anything
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

void World::DisableHighlightedBlock()
{
	if (!highlighted)
		return; //Nothing to do
	Chunk* const chunk = GetChunk(highlightedBlock);
	chunk->DisableHighlightedBlock();
	highlighted = false;
}

std::pair<glm::ivec3, bool> World::GetHighlightedBlock()
{
	return { highlightedBlock, highlighted };
}*/