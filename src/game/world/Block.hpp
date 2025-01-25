#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

typedef glm::ivec3 WorldPos;

enum class TransparencyType
{
	Opaque,
	FullTransparency,
	PartialTransparency
};

struct BlockState
{
	bool HighLighted;
};

struct Block
{
public:
	enum BlockType
	{
		Air,
		Mud,
		Grass,
		Stone,
		Water,
		Wood,
		Leaves
	};
	static constexpr unsigned int c_BlockCount = 7;

	BlockType Type;


	Block();
	Block(BlockType type);
	
	const std::string& GetBlockName() const { return c_BlockData[(unsigned int)Type].Name; };
	const std::string& GetBlockDisplayName() const { return c_BlockData[(unsigned int)Type].DisplayName; };
	TransparencyType GetTransparencyType() const { return c_BlockData[(unsigned int)Type].TransType; };

	static void SetupBlockData();
	static const std::string& GetBlockName(BlockType type) { return c_BlockData[(unsigned int)type].Name; };
	static const std::string& GetBlockDisplayName(BlockType type) { return c_BlockData[(unsigned int)type].DisplayName; };
	static TransparencyType GetTransparencyType(BlockType type) { return c_BlockData[(unsigned int)type].TransType; };

private:
	struct BlockData
	{
		std::string	Name, DisplayName;
		TransparencyType TransType;

		BlockData();
		BlockData(const std::string& name, const std::string& displayName, TransparencyType transparencyType = TransparencyType::Opaque);
	};

	static BlockData c_BlockData[c_BlockCount];
};