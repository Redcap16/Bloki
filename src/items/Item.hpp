#pragma once

#include <world/Block.hpp>
#include <array>

struct ItemData
{
	int MaxStackCount;
};

class Item
{
public:
	static const int c_ItemCount = 1;

	enum ItemType
	{
		Bread
	};
	bool IsBlock;
	union
	{
		ItemType ItemID;
		Block::BlockType BlockID;
	};

	Item(ItemType type) :IsBlock(false), ItemID(type) { }
	Item(Block::BlockType type) : IsBlock(true), BlockID(type) { }

	bool StackableWith(const Item& item) const;
	int MaxStackCount() const;

private:
	static std::array<ItemData, c_ItemCount> s_ItemData;
	static const int c_BlockMaxStackCount = 64;
};

bool operator==(const Item& lhs, const Item& rhs);

inline bool operator!=(const Item& lhs, const Item& rhs)
{
	return !(lhs == rhs);
}