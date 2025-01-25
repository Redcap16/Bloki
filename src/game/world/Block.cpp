#include <game/world/Block.hpp>

Block::BlockData::BlockData() :
	BlockData("", "")
{

}

Block::BlockData::BlockData(const std::string& name, const std::string& displayName, TransparencyType transparencyType) :
	Name(name),
	DisplayName(displayName),
	TransType(transparencyType)
{

}



Block::BlockData Block::c_BlockData[c_BlockCount];

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
	Type(BlockType::Air)
{
}

Block::Block(BlockType type) :
	Type(type)
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
	c_BlockData[(int)Block::BlockType::Air] = BlockData("air", "Air", TransparencyType::FullTransparency);
	c_BlockData[(int)Block::BlockType::Grass] = BlockData("grass", "Grass");
	c_BlockData[(int)Block::BlockType::Mud] = BlockData("mud", "Mud");
	c_BlockData[(int)Block::BlockType::Stone] = BlockData("stone", "Stone");
	c_BlockData[(int)Block::BlockType::Water] = BlockData("water", "Water", TransparencyType::PartialTransparency);
	c_BlockData[(int)Block::BlockType::Wood] = BlockData("wood", "Wood");
	c_BlockData[(int)Block::BlockType::Leaves] = BlockData("leaves", "Leaves", TransparencyType::FullTransparency);
}