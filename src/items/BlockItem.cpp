#include <items/BlockItem.hpp>

ItemTypeArray<Block::BlockType> BlockItem::m_ItemTypes;

BlockItem::BlockItem(Block::BlockType blockType) :
	m_BlockType(blockType) {

}

std::unique_ptr<Item> BlockItem::Clone() const {
	return std::make_unique<BlockItem>(*this);
}

std::string BlockItem::GetName() const {
	return "Block name goes here";
}

ItemTextureData BlockItem::GetTextureData() const {
	return ItemTextureData({ 4, 0 });
}

bool BlockItem::Use(ItemUser& user, BlockManager& blockManager) {
	WorldPos placingPosition;
	if (user.GetPlacingAt(placingPosition))
		return blockManager.PlaceBlock(placingPosition, Block(m_BlockType), false);
	return false;
}