#include <items/Inventory.hpp>

void Inventory::SetItemStack(int index, const ItemStack& stack) 
{ 
	m_Items.at(index) = stack;

	for (auto listener : m_Listeners)
		listener->Updated(index);
}