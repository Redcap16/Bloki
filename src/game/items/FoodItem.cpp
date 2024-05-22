#include <game/items/FoodItem.hpp>

ItemTypeArray<FoodItem::FoodType> FoodItem::m_ItemTypes;

FoodItem::FoodItem(FoodType type) :
	m_FoodType(type) {

}

std::unique_ptr<Item> FoodItem::Clone() const {
	return std::make_unique<FoodItem>(*this);
}

std::string FoodItem::GetName() const {
	switch (m_FoodType) {
	case FoodType::Apple:
		return "Apple";
	case FoodType::Bread:
		return "Bread";
	case FoodType::Meat:
		return "Meat";
	default:
		return "";
	}
}

ItemTextureData FoodItem::GetTextureData() const {
	switch (m_FoodType) {
	case FoodType::Apple:
		return ItemTextureData({ 2, 0 });
	case FoodType::Bread:
		return ItemTextureData({ 1, 0 });
	case FoodType::Meat:
		return ItemTextureData({ 3, 0 });
	}
	return ItemTextureData({0, 0});
}

bool FoodItem::Use(ItemUser& user, BlockManager& blockManager) {
	user.ChangeHealth(0.4f);
	return true;
}