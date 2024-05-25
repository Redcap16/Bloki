#pragma once

#include <game/items/Item.hpp>

class FoodItem : public Item {
public:
	enum class FoodType {
		Bread,
		Apple,
		Meat
	};

	FoodItem(FoodType type);
	std::unique_ptr<Item> Clone() const override;

	ItemType GetType() const override { return m_ItemTypes[m_FoodType]; }
	std::string GetName() const override;
	const Texture& GetTexture() const override;
	bool Use(ItemUser& user, BlockManager& blockManager) override;

private:
	static ItemTypeArray<FoodType> m_ItemTypes;
	FoodType m_FoodType;
};