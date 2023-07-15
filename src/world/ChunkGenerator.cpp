#include <world/ChunkGenerator.hpp>

ChunkGenerator::ChunkGenerator(BlockManager& world) :
	m_World(world)
{

}

void ChunkGenerator::GenerateChunk(Chunk& chunk)
{
	//TODO: Use BlockArray swapping
	ChunkPos position = chunk.GetPosition();
	if (position.y > 0) //not only air
		return;

	for (int x = 0; x < BlockArray::ChunkSize.x; x++)
		for (int z = 0; z < BlockArray::ChunkSize.z; z++)
		{
			float perlinx = Math::Mod((position.x * (float)BlockArray::ChunkSize.x + (float)x) / 60.0f, 256.0f),
				perlinz = Math::Mod((position.z * (float)BlockArray::ChunkSize.z + (float)z) / 60.0f, 256.0f);

			int height = (db::perlin<float>(perlinx, perlinz) + 1.0f) * 30 + 1;

			for (int y = 0; y < BlockArray::ChunkSize.y; y++)
			{
				Block block = Block(Block::BlockType::Air);

				if (position.y < 0)
					block.Type = Block::BlockType::Stone;
				else if (position.y == 0)
				{
					if (y < 25 && y > height)
						block.Type = Block::BlockType::Water;
					else if (y == height)
						block.Type = Block::BlockType::Grass;
					else if (y >= height - 3 && y < height)
						block.Type = Block::BlockType::Mud;
					else if (y < height - 3)
						block.Type = Block::BlockType::Stone;
				}

				if (block.Type != Block::BlockType::Air)
					chunk.SetBlock(glm::uvec3(x, y, z), block);
			}

			if (!(rand() % 100) && height > 25) //TREE
				generateTree(chunk, position, position * (glm::ivec3)BlockArray::ChunkSize + glm::ivec3(x, height + 1, z));
		}
}

void ChunkGenerator::generateTree(Chunk& chunk, glm::ivec3 givenChunkPosition, glm::ivec3 rootPosition)
{
	int height = rand() % 3 + 4;

	for (int x = -2; x < 3; ++x)
		for (int z = -2; z < 3; ++z)
			if ((x != -2 && x != 2) || (z != -2 && z != 2))
				setStructureBlock(chunk, rootPosition + glm::ivec3(x, height - 2, z), Block::BlockType::Leaves);
	for (int y = 0; y < 2; ++y)
		for (int x = -1; x < 2; ++x)
			for (int z = -1; z < 2; ++z)
				if ((x != -1 && x != 1) || (z != -1 && z != 1))
					setStructureBlock(chunk, rootPosition + glm::ivec3(x, height - y, z), Block::BlockType::Leaves);
	for (int i = 0; i < height; ++i)
		setStructureBlock(chunk, rootPosition + glm::ivec3(0, i, 0), Block::BlockType::Wood);
}

void ChunkGenerator::setStructureBlock(Chunk& chunk, const glm::ivec3& blockPosition, Block block)
{
	if (Chunk::GetChunkPosition(blockPosition) == chunk.GetPosition())
		chunk.SetBlock(Chunk::GetInChunkPosition(blockPosition), block);
	else
		m_World.PlaceBlock(blockPosition, block, false);
}