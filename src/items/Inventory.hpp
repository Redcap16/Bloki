#pragma once

#include <items/Item.hpp>
#include <array>
#include <cassert>

struct ItemStack
{
	Item ItemHeld;
	int Count;

	ItemStack() : ItemHeld(Item::Bread), Count(0) { }
	ItemStack(Item itemHeld, int count) : ItemHeld(itemHeld), Count(count) {}
};

inline bool operator==(const ItemStack& lhs, const ItemStack& rhs)
{
	return lhs.Count == rhs.Count &&
		lhs.ItemHeld == rhs.ItemHeld;
}

inline bool operator!=(const ItemStack& lhs, const ItemStack& rhs)
{
	return !(lhs == rhs);
}

class InventoryUpdateListener
{
public:
	virtual void Updated(int index) = 0;
};

class Inventory
{
public:
	static const int c_Size = 36;

	const ItemStack& GetItemStack(int index) const { return m_Items.at(index); };
	void SetItemStack(int index, const ItemStack& stack);
	void AddListener(InventoryUpdateListener* listener) { m_Listeners.push_back(listener); }

private:
	std::array<ItemStack, c_Size> m_Items;
	std::vector<InventoryUpdateListener*> m_Listeners;
};