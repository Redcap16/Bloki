#include <items/Item.hpp>

std::array<ItemData, Item::c_ItemCount> Item::s_ItemData;

bool Item::StackableWith(const Item& item) const
{
	if (item.IsBlock && IsBlock)
		return item.BlockID == BlockID;
	else if (!item.IsBlock && !IsBlock)
		return item.ItemID == ItemID && 
		s_ItemData[(int)item.ItemID].MaxStackCount > 1;
}

int Item::MaxStackCount() const
{
	if (IsBlock)
		return c_BlockMaxStackCount;
	
	return s_ItemData[ItemID].MaxStackCount;
}

bool operator==(const Item& lhs, const Item& rhs)
{
	return (lhs.IsBlock && rhs.IsBlock && lhs.BlockID == rhs.BlockID) ||
		(!lhs.IsBlock && !rhs.IsBlock && lhs.ItemID == rhs.ItemID);
}