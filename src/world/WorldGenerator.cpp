#include <world/worldGenerator.hpp>
#include <world/World.hpp>

WorldGenerator::WorldGenerator(World& world) :
	m_World(world)
{
	
}

WorldGenerator::~WorldGenerator()
{
}

void WorldGenerator::Generate(Chunk& chunk, glm::ivec3 position)
{
	if (position.y <= 0) //not only air
	{
		for (int x = 0; x < ChunkBlockData::ChunkSize.x; x++)
		{
			for (int z = 0; z < ChunkBlockData::ChunkSize.z; z++)
			{
				float perlinx = fmod((position.x * (float)ChunkBlockData::ChunkSize.x + (float)x) / 60.0f, 256),
					perlinz = fmod((position.z * (float)ChunkBlockData::ChunkSize.z + (float)z) / 60.0f, 256);
				if (perlinx < 0)
					perlinx += 256;
				if (perlinz < 0)
					perlinz += 256;
				int height = (db::perlin<float>(perlinx, perlinz) + 1.0f) * 30 + 1;

				for (int y = 0; y < ChunkBlockData::ChunkSize.y; y++)
				{
					Block block = Block(Block::BlockType::Air);

					if (position.y < 0)
						block.type = Block::BlockType::Stone;
					else if (position.y == 0)
					{
						if (y < 25 && y > height)
							block.type = Block::BlockType::Water;
						else if (y == height)
							block.type = Block::BlockType::Grass;
						else if (y >= height - 3 && y < height)
							block.type = Block::BlockType::Mud;
						else if (y < height - 3)
							block.type = Block::BlockType::Stone;
					}

					if (block.type != Block::BlockType::Air)
						chunk.SetBlock(glm::uvec3(x, y, z), block);
				}

				if (!(rand() % 100) && height > 25) //TREE
					generateTree(chunk, position, position * (glm::ivec3)ChunkBlockData::ChunkSize + glm::ivec3(x, height + 1, z));
			}
		}
	}
}

void WorldGenerator::generateTree(Chunk& chunk, glm::ivec3 givenChunkPosition, glm::ivec3 rootPosition)
{
	int th = rand() % 3 + 4;

	for (int x = -2; x < 3; ++x)
		for (int z = -2; z < 3; ++z)
			if((x != -2 && x != 2) || (z != -2 && z != 2))
				setStructureBlock(chunk, givenChunkPosition, rootPosition + glm::ivec3(x, th - 2, z), Block::BlockType::Leaves);
	for (int y = 0; y < 2; ++y)
		for (int x = -1; x < 2; ++x)
			for (int z = -1; z < 2; ++z)
				if ((x != -1 && x != 1) || (z != -1 && z != 1))
				setStructureBlock(chunk, givenChunkPosition, rootPosition + glm::ivec3(x, th - y, z), Block::BlockType::Leaves);
	for (int i = 0; i < th; ++i)
		setStructureBlock(chunk, givenChunkPosition, rootPosition + glm::ivec3(0, i, 0), Block::BlockType::Wood);
}

void WorldGenerator::setStructureBlock(Chunk& chunk, glm::ivec3 givenChunkPosition, glm::ivec3 position, Block::BlockType type)
{
	glm::ivec3 chunkPosition = glm::floor((glm::vec3)position / (glm::vec3)ChunkBlockData::ChunkSize),
		blockPosition = position - chunkPosition * (glm::ivec3)ChunkBlockData::ChunkSize;

	if (givenChunkPosition == chunkPosition)
		chunk.SetBlock(blockPosition, Block(type));
	else
	{
		if (!m_World.isChunkGenerated(chunkPosition))
			return; //TODO: Then add structure to queue for future generation;

		Chunk* neighborChunk = m_World.getChunk(chunkPosition, true);
		if (neighborChunk == nullptr)
			return;

		neighborChunk->SetBlock(blockPosition, Block(type));
	}
}