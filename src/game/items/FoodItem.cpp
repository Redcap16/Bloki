#include <game/items/FoodItem.hpp>
#include <game/graphics/ItemTextureProvider.hpp>

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

const Texture& FoodItem::GetTexture() const {
	auto& loader = game::graphics::ItemTextureProvider::GetLoader();
	switch (m_FoodType) {
	case FoodType::Apple:
		return loader.GetTextureForFlat({ 2, 0 });
	case FoodType::Bread:
		return loader.GetTextureForFlat({ 1, 0 });
	case FoodType::Meat:
		return loader.GetTextureForFlat({ 3, 0 });
	}
	return loader.GetTextureForFlat({0, 0});
}

bool FoodItem::Use(ItemUser& user, BlockManager& blockManager) {
	user.ChangeHealth(0.4f);
	return true;
}