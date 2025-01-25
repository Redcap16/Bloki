#include <game/items/Item.hpp>

bool operator==(const Item& lhs, const Item& rhs)
{
	return (lhs.GetType() == rhs.GetType());
}

std::unique_ptr<Item> Item::GetByType(ItemType type) {
	return s_RegisteredItems.at(type)->Clone();
}

void Item::RegisterItem(const Item& item) {
	s_RegisteredItems[item.GetType()] = item.Clone();
}