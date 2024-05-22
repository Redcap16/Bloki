#include <game/items/Inventory.hpp>

ItemStack::ItemStack(const Item& itemHeld, int count) : 
	m_Count(count) {
	m_ItemHeld = itemHeld.Clone();
}

bool ItemStack::Empty() const {
	return m_Count == 0;
}

void ItemStack::Set(const Item& item, unsigned int count) {
	SetItemHeld(item);
	SetCount(count);
}

void ItemStack::SetItemHeld(const Item& item) {
	m_ItemHeld = item.Clone();

	notifyListeners();
}

void ItemStack::SetCount(unsigned int count) {
	if(m_ItemHeld)
		m_Count = count;
	if (!m_Count)
		m_ItemHeld.reset();

	notifyListeners();
}

void ItemStack::Clear() {
	m_ItemHeld.reset();
	m_Count = 0;

	notifyListeners();
}

void ItemStack::ChangeCount(int change) {
	if (change + m_Count > Item::c_MaxStackCount ||
		(change < 0 && (unsigned int)(- change) > m_Count))
		return;

	SetCount(m_Count + change);
}

void ItemStack::SwapContents(ItemStack& second) {
	std::swap(m_ItemHeld, second.m_ItemHeld);
	std::swap(m_Count, second.m_Count);

	notifyListeners();
	second.notifyListeners();
}

void ItemStack::MoveTo(ItemStack& second, int count) {
	if (!Empty() && !second.Empty() && 
		*m_ItemHeld != *second.m_ItemHeld)
		return;

	if (second.Empty())
		second.m_ItemHeld = m_ItemHeld->Clone();

	int transferCount = std::min(Item::c_MaxStackCount - second.m_Count, m_Count);
	if (count)
		transferCount = std::min(transferCount, count);

	ChangeCount(-transferCount);
	second.ChangeCount(transferCount);

	notifyListeners();
	second.notifyListeners();
}

bool ItemStack::Use(ItemUser& user, BlockManager& blockManager) {
	if (Empty())
		return false;

	if (m_ItemHeld->Use(user, blockManager)) {
		m_Count--;
		notifyListeners();
		return true;
	}
	return false;
}

void ItemStack::notifyListeners() {
	for (auto listener : m_Listeners) {
		listener->StackUpdated();
	}
}

Inventory::Inventory() {
	for (auto& itemStack : m_Items) {
		itemStack.AddListener(this);
	}
}

Inventory::~Inventory() {
	for (auto& itemStack : m_Items) {
		itemStack.RemoveListener(this);
	}
}

bool Inventory::AddItem(ItemStack& item) {
	if (item.Empty())
		return false;

	for (auto& itemStack : m_Items) { //Try to find matching item stack first
		if(!itemStack.Empty() &&
			item.GetItemHeld() == itemStack.GetItemHeld())
			item.MoveTo(itemStack);
		if (item.Empty())
			return true;
	}
	for (auto& itemStack : m_Items) { //If not found any matching stack, place it anywhere
		item.MoveTo(itemStack);
		if (item.Empty())
			return true;
	}

	return false;
}

void Inventory::SetSelectedItem(int index) {
	if (index < 0
		&& index > c_SelectableItemCount)
		return;
	
	m_SelectedItemIndex = index;
}

void Inventory::ChangeSelectedItem(int indexChange) {
	m_SelectedItemIndex += indexChange;
	if (m_SelectedItemIndex > 0)
		m_SelectedItemIndex %= c_SelectableItemCount;
	else if (m_SelectedItemIndex < 0)
		m_SelectedItemIndex = c_SelectableItemCount + (m_SelectedItemIndex % c_SelectableItemCount);
}

void Inventory::StackUpdated() {

}