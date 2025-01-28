#include <game/items/BlockItem.hpp>
#include <game/graphics/ItemTextureProvider.hpp>
#include <game/graphics/BlockTextureProvider.hpp>
#include <game/graphics/ConcreteItemMesh.hpp>

const std::string BlockItem::c_ItemClass = "blockitem";

BlockItem::BlockItem(Block::BlockType blockType) :
	m_BlockType(blockType) {

}

std::unique_ptr<Item> BlockItem::Clone() const {
	return std::make_unique<BlockItem>(*this);
}

ItemType BlockItem::GetType() const {
	return ItemType(c_ItemClass, Block::GetBlockName(m_BlockType));
}

std::string BlockItem::GetName() const {
	return Block::GetBlockDisplayName(m_BlockType);
}

const Texture& BlockItem::GetTexture() const {
	return game::graphics::ItemTextureProvider::GetLoader().GetTextureForBlock(m_BlockType);
}

std::unique_ptr<game::graphics::DroppedItemMesh> BlockItem::GetMesh() const {
	using namespace game::graphics;
	return std::make_unique<BlockItemMesh>(BlockTextureProvider::GetLoader().GetBlockTexture(m_BlockType));
}

bool BlockItem::Use(ItemUser& user, BlockManager& blockManager) {
	WorldPos placingPosition;
	if (user.GetPlacingAt(placingPosition))
		return blockManager.PlaceBlock(placingPosition, Block(m_BlockType), false);
	return false;
}

void BlockItem::RegisterTypes() {
	for (int blockType = 0; blockType < Block::c_BlockCount; blockType++)
		Item::RegisterItem(BlockItem((Block::BlockType)blockType));
}