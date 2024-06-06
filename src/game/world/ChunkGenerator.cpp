#include <game/world/ChunkGenerator.hpp>

ChunkGenerator::ChunkGenerator(BlockManager& world) :
	m_World(world)
{

}

void ChunkGenerator::GenerateChunk(Chunk& chunk)
{
	ChunkPos position = chunk.GetPosition();
	BlockArray tempArray;
	chunk.SwapBlockArray(tempArray);
;	if (position.y > 0) //not only air
		return;

	for (int x = 0; x < BlockArray::ChunkSize.x; x++)
		for (int z = 0; z < BlockArray::ChunkSize.z; z++)
		{
			float perlinx = Math::Mod((position.x * (float)BlockArray::ChunkSize.x + (float)x) / 60.0f, 256.0f),
				perlinz = Math::Mod((position.z * (float)BlockArray::ChunkSize.z + (float)z) / 60.0f, 256.0f);

			int height = (db::perlin<float>(perlinx, perlinz) + 1.0f) * 30 + 1;

			for (int y = 0; y < BlockArray::ChunkSize.y; y++)
			{
				Block::BlockType type = Block::Air;

				if (position.y < 0)
					type = Block::BlockType::Stone;
				else if (position.y == 0)
				{
					if (y < 25 && y > height)
						type = Block::BlockType::Water;
					else if (y == height)
						type = Block::BlockType::Grass;
					else if (y >= height - 3 && y < height)
						type = Block::BlockType::Mud;
					else if (y < height - 3)
						type = Block::BlockType::Stone;
				}

				if (type != Block::BlockType::Air)
					tempArray.Set({ x, y, z }, type);
			}

			if (!(rand() % 100) && 
				position.y >= 0 && 
				height > 25) //TREE
				generateTree(tempArray, position, position * (glm::ivec3)BlockArray::ChunkSize + glm::ivec3(x, height + 1, z));
		}

	chunk.SwapBlockArray(tempArray);
	chunk.SetGenerated();
}

void ChunkGenerator::generateTree(BlockArray& array, const ChunkPos& givenChunkPosition, const WorldPos& rootPosition)
{
	int height = rand() % 3 + 4;

	for (int x = -2; x < 3; ++x)
		for (int z = -2; z < 3; ++z)
			if ((x != -2 && x != 2) || (z != -2 && z != 2))
				setStructureBlock(array, givenChunkPosition, rootPosition + glm::ivec3(x, height - 2, z), Block::BlockType::Leaves);
	for (int y = 0; y < 2; ++y)
		for (int x = -1; x < 2; ++x)
			for (int z = -1; z < 2; ++z)
				if ((x != -1 && x != 1) || (z != -1 && z != 1))
					setStructureBlock(array, givenChunkPosition, rootPosition + glm::ivec3(x, height - y, z), Block::BlockType::Leaves);
	for (int i = 0; i < height; ++i)
		setStructureBlock(array, givenChunkPosition, rootPosition + glm::ivec3(0, i, 0), Block::BlockType::Wood);
}

void ChunkGenerator::setStructureBlock(BlockArray& array, const ChunkPos& givenChunkPosition, const WorldPos& blockPosition, Block block)
{
	if (Chunk::GetChunkPosition(blockPosition) == givenChunkPosition)
		array.Set(Chunk::GetInChunkPosition(blockPosition), block);
	else
		m_World.PlaceBlock(blockPosition, block, false);
}