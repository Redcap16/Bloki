#include <world/Block.hpp>

Block::BlockData::BlockData() :
	BlockData("")
{

}

Block::BlockData::BlockData(const std::string& textureName, TransparencyType transparencyType) :
	TextureName(textureName),
	TransType(transparencyType)
{

}

glm::vec3 GetDirectionVector(FaceDirection dir)
{
	switch (dir)
	{
	case FaceDirection::Top:
		return glm::vec3(0, 1, 0);
	case FaceDirection::Bottom:
		return glm::vec3(0, -1, 0);
	case FaceDirection::Right:
		return glm::vec3(1, 0, 0);
	case FaceDirection::Left:
		return glm::vec3(-1, 0, 0);
	case FaceDirection::Front:
		return glm::vec3(0, 0, 1);
	case FaceDirection::Back:
		return glm::vec3(0, 0, -1);
	}
}

Block::BlockData Block::blockData[blockCount];

//const Block::BlockData Block::blockData[blockCount] = {
//		[0] = {.textureName = "", .transparency = 1} };
		/*{"Mud", "mud.png", 1},
		{"Grass", "grass.png", 1},
		{"Stone", "stone.png", 1},
		{"Water", "water.png", 0.7f} };*/

//const glm::ivec2 Block::TextureManager::textureSize = glm::ivec2(64, 48);
//
//void Block::TextureManager::setupTextures()
//{
//	for (int i = 0; i < blockCount; i++)
//	{
//		if (blockData[i].TextureName == "")
//		{
//			subTextures.push_back(AtlasTexture::SubTexture());
//			continue;
//		}
//
//		const auto subTexture = texture.Load(blockData[i].TextureName);
//		if (!subTexture.first)
//		{
//			std::cout << "Cant load " << blockData[i].TextureName << " texture." << std::endl;
//			subTextures.push_back(AtlasTexture::SubTexture());
//			continue;
//		}
//
//		subTextures.push_back(subTexture.second);
//	}
//}
//
//Block::TextureManager::TextureManager() :
//	texture(textureSize, blockCount)
//{
//	setupTextures();
//}
//
//Block::TextureManager::~TextureManager()
//{
//
//}
//
//Block::TextureManager& Block::TextureManager::GetInstance()
//{
//	static TextureManager instance;
//	return instance;
//}
//
//AtlasTexture::SubTexture& Block::TextureManager::GetSubTexture(BlockType id)
//{
//	return subTextures[(int)id];
//}



Block::Block() :
	type(BlockType::Air)
{
}

Block::Block(BlockType type) :
	type(type)
{
}

//AtlasTexture::SubTexture& Block::GetSubTexture(BlockType type)
//{
//	return TextureManager::GetInstance().GetSubTexture(type);
//}

/*Block::BlockType Block::GetIdByName(const std::string name) //Warning slow function
{
	for (BlockType i = 0; i < blockCount; i++)
		if (name == blockData[i].name)
			return i;

	return 0;
}*/

void Block::SetupBlockData()
{
	blockData[(int)Block::BlockType::Air] = BlockData("", TransparencyType::FullTransparency);
	blockData[(int)Block::BlockType::Grass] = BlockData("grass.png");
	blockData[(int)Block::BlockType::Mud] = BlockData("mud.png");
	blockData[(int)Block::BlockType::Stone] = BlockData("stone.png");
	blockData[(int)Block::BlockType::Water] = BlockData("water.png", TransparencyType::PartialTransparency);
	blockData[(int)Block::BlockType::Wood] = BlockData("wood.png");
	blockData[(int)Block::BlockType::Leaves] = BlockData("leaves.png", TransparencyType::FullTransparency);
}