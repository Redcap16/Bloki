#pragma once

#include <game/items/Item.hpp>
#include <array>
#include <set>
#include <cassert>
#include <memory>
#include <engine/qxml/QXMLReader.hpp>
#include <engine/qxml/QXMLWriter.hpp>

class StackUpdateListener
{
public:
	virtual void StackUpdated() = 0;
};

class ItemStack
{
public:
	ItemStack() : m_Count(0) {}
	ItemStack(const Item& itemHeld, int count);

	void AddListener(StackUpdateListener* listener) { m_Listeners.insert(listener); }
	void RemoveListener(StackUpdateListener* listener) { m_Listeners.erase(listener); }

	Item& GetItemHeld() { return *m_ItemHeld; }
	const Item& GetItemHeld() const { return *m_ItemHeld; }
	int GetCount() const { return m_Count; }

	bool Empty() const;
	
	void Set(const Item& item, unsigned int count);
	void SetItemHeld(const Item& item);
	void SetCount(unsigned int count);
	void Clear();
	void ChangeCount(int change);

	void SwapContents(ItemStack& second);
	void MoveTo(ItemStack& second, int count = 0);

	bool Use(ItemUser& user, BlockManager& blockManager);

	friend bool operator==(const ItemStack& lhs, const ItemStack& rhs);

private:
	std::set<StackUpdateListener*> m_Listeners;

	std::unique_ptr<Item> m_ItemHeld;
	unsigned int m_Count;

	void notifyListeners();
};

inline bool operator==(const ItemStack& lhs, const ItemStack& rhs)
{
	if (!lhs.m_ItemHeld && !rhs.m_ItemHeld)
		return true;
	if (!lhs.m_ItemHeld || !rhs.m_ItemHeld)
		return false;
	return lhs.m_Count == rhs.m_Count &&
		*lhs.m_ItemHeld == *rhs.m_ItemHeld;
}

inline bool operator!=(const ItemStack& lhs, const ItemStack& rhs)
{
	return !(lhs == rhs);
}

class Inventory : public StackUpdateListener
{
public:
	static const int c_Size = 36;

	Inventory();
	~Inventory();

	ItemStack& GetItemStack(int index) { return m_Items.at(index); };
	const ItemStack& GetItemStack(int index) const { return m_Items.at(index); };

	bool AddItem(ItemStack& item);

	void SetSelectedItem(int index);
	void ChangeSelectedItem(int indexChange);
	int GetSelectedItemIndex() const { return m_SelectedItemIndex; }
	const ItemStack& GetSelectedItem() const { return m_Items[m_SelectedItemIndex]; }
	ItemStack& GetSelectedItem() { return m_Items[m_SelectedItemIndex]; }

	void StackUpdated() override;

	void Serialize(std::vector<char>& data) const;
	static std::unique_ptr<Inventory> Deserialize(const std::vector<char>& data);
private:
	Inventory(std::array<ItemStack, c_Size>& items);

	std::array<ItemStack, c_Size> m_Items;
	int m_SelectedItemIndex = 0;
	const int c_SelectableItemCount = 9;
};