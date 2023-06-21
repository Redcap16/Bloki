#pragma once

#include <string>
#include <vector>

#include <graphics/AtlasTexture.hpp>

/// <summary>
/// Where Y+ is considered top, X+ right and Z+ front
/// </summary>
enum class FaceDirection
{
	Top,
	Bottom,
	Right,
	Left,
	Front,
	Back
};

enum class TransparencyType
{
	Opaque,
	FullTransparency,
	PartialTransparency
};

glm::vec3 GetDirectionVector(FaceDirection dir);

struct Block
{
	struct BlockData
	{
		std::string	TextureName;
		TransparencyType TransType;

		BlockData();
		BlockData(const std::string& textureName, TransparencyType transparencyType = TransparencyType::Opaque);
	};

	enum class BlockType
	{
		Air,
		Mud,
		Grass,
		Stone,
		Water,
		Wood,
		Leaves
	};

	BlockType type;

	static constexpr unsigned int blockCount = 7;
	static BlockData blockData[blockCount];

	/*class TextureManager
	{
	public:
		AtlasTexture texture;
	private:
		static const glm::ivec2 textureSize;

		std::vector<AtlasTexture::SubTexture> subTextures;

		void setupTextures();
		TextureManager();
	public:
		~TextureManager();

		static TextureManager& GetInstance();
		AtlasTexture::SubTexture& GetSubTexture(BlockType id);
	};*/

	Block();
	Block(BlockType type);

	//static AtlasTexture::SubTexture& GetSubTexture(BlockType type);
	//static BlockType GetIdByName(const std::string name); //Warning slow function
	static void SetupBlockData();
};