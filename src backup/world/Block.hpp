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

struct Block
{
	struct BlockData
	{
		std::string name,
			textureName;
		float transparency;
	};

	typedef unsigned short BlockType;

	BlockType type;

	static const unsigned int blockCount = 5;
	static const BlockData blockData[blockCount];

	class TextureManager
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
	};

	Block();
	Block(BlockType type);

	static AtlasTexture::SubTexture& GetSubTexture(BlockType type);
	static BlockType GetIdByName(const std::string name); //Warning slow function
};