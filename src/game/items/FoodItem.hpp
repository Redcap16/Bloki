#pragma once

#include <game/items/Item.hpp>
#include <stdexcept>

class FoodItem : public Item {
public:
	enum class FoodType {
		Bread,
		Apple,
		Meat
	};

	FoodItem(FoodType type);
	std::unique_ptr<Item> Clone() const override;

	ItemType GetType() const override;
	std::string GetName() const override;
	const Texture& GetTexture() const override;
	std::unique_ptr<game::graphics::DroppedItemMesh> GetMesh() const override;
	bool Use(ItemUser& user, BlockManager& blockManager) override;

private:
	static const std::string c_ItemClass;

	FoodType m_FoodType;
};