#pragma once

#include <game/items/Item.hpp>
#include <game/world/Block.hpp>

class BlockItem : public Item {
public:
	BlockItem(Block::BlockType blockType);

	std::unique_ptr<Item> Clone() const override;

	ItemType GetType() const override;
	std::string GetName() const override;
	const Texture& GetTexture() const override;
	std::unique_ptr<game::graphics::DroppedItemMesh> GetMesh() const override;
	bool Use(ItemUser& user, BlockManager& blockManager) override;

	static void RegisterTypes();
private:
	static const std::string c_ItemClass;

	Block::BlockType m_BlockType;
};