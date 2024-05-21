#pragma once

#include <items/Item.hpp>
#include <world/Block.hpp>

class BlockItem : public Item {
public:
	BlockItem(Block::BlockType blockType);

	std::unique_ptr<Item> Clone() const override;

	ItemType GetType() const override { return m_ItemTypes[m_BlockType]; }
	std::string GetName() const override;
	ItemTextureData GetTextureData() const override;
	bool Use(ItemUser& user, BlockManager& blockManager) override;

private:
	static ItemTypeArray<Block::BlockType> m_ItemTypes;
	Block::BlockType m_BlockType;
};