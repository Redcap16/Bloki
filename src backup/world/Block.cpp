#include <world/Block.hpp>

const Block::BlockData Block::blockData[blockCount] = {
		{"Air", "", 1},
		{"Mud", "mud.png", 1},
		{"Grass", "grass.png", 1},
		{"Stone", "stone.png", 1},
		{"Water", "water.png", 0.7f} };

const glm::ivec2 Block::TextureManager::textureSize = glm::ivec2(64, 48);

void Block::TextureManager::setupTextures()
{
	for (BlockType i = 0; i < blockCount; i++)
	{
		if (blockData[i].textureName == "")
		{
			subTextures.push_back(AtlasTexture::SubTexture());
			continue;
		}

		const auto subTexture = texture.Load(blockData[i].textureName);
		if (!subTexture.first)
		{
			std::cout << "Cant load " << blockData[i].name << " texture." << std::endl;
			subTextures.push_back(AtlasTexture::SubTexture());
			continue;
		}

		subTextures.push_back(subTexture.second);
	}
}

Block::TextureManager::TextureManager() :
	texture(textureSize, blockCount)
{
	setupTextures();
}

Block::TextureManager::~TextureManager()
{

}

Block::TextureManager& Block::TextureManager::GetInstance()
{
	static TextureManager instance;
	return instance;
}

AtlasTexture::SubTexture& Block::TextureManager::GetSubTexture(BlockType id)
{
	return subTextures[id];
}



Block::Block() :
	type(0)
{
}

Block::Block(BlockType type) :
	type(type)
{
}

AtlasTexture::SubTexture& Block::GetSubTexture(BlockType type)
{
	return TextureManager::GetInstance().GetSubTexture(type);
}

Block::BlockType Block::GetIdByName(const std::string name) //Warning slow function
{
	for (BlockType i = 0; i < blockCount; i++)
		if (name == blockData[i].name)
			return i;

	return 0;
}