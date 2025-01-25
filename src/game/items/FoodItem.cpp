#include <game/items/FoodItem.hpp>
#include <game/graphics/ItemTextureProvider.hpp>
#include <game/graphics/ConcreteItemMesh.hpp>

const std::string FoodItem::c_ItemClass = "fooditem";

FoodItem::FoodItem(FoodType type) :
	m_FoodType(type) {

}

std::unique_ptr<Item> FoodItem::Clone() const {
	return std::make_unique<FoodItem>(*this);
}

ItemType FoodItem::GetType() const {
	switch (m_FoodType) {
	case FoodType::Apple:
		return ItemType(c_ItemClass, "apple");
	case FoodType::Bread:
		return ItemType(c_ItemClass, "bread");
	case FoodType::Meat:
		return ItemType(c_ItemClass, "meat");
	default:
		throw std::invalid_argument("Invalid item type");
	}
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

std::unique_ptr<game::graphics::DroppedItemMesh> FoodItem::GetMesh() const {
	using namespace game::graphics;
	return std::make_unique<FlatItemMesh>(GetTexture());
}

bool FoodItem::Use(ItemUser& user, BlockManager& blockManager) {
	user.ChangeHealth(0.4f);
	return true;
}

void FoodItem::RegisterTypes() {
	Item::RegisterItem(FoodItem(FoodType::Bread));
	Item::RegisterItem(FoodItem(FoodType::Apple));
	Item::RegisterItem(FoodItem(FoodType::Meat));
}