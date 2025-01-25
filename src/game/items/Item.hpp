#pragma once

#include <game/world/BlockManager.hpp>
#include <memory>
#include <array>
#include <map>
#include <engine/graphics/Texture.hpp>
#include <game/graphics/DroppedItemMesh.hpp>

class ItemUser {
public:
	virtual void ChangeHealth(float healthChange) = 0;
	virtual WorldPos GetLookingAt() const = 0;
	virtual bool GetPlacingAt(WorldPos &position) const = 0;
};

class ItemType {
private:
	std::string m_ID;
public:
	ItemType(const std::string& itemClass, const std::string& itemType) :
		m_ID(itemClass + "." + itemType) {
	}

	friend bool operator==(const ItemType& lhs, const ItemType& rhs);
	friend bool operator<(const ItemType& lhs, const ItemType& rhs);
};

class Item
{
public:
	static const int c_MaxStackCount = 64;

	virtual std::unique_ptr<Item> Clone() const = 0;

	virtual ItemType GetType() const = 0;
	virtual std::string GetName() const = 0;
	virtual const Texture& GetTexture() const = 0;
	virtual std::unique_ptr<game::graphics::DroppedItemMesh> GetMesh() const = 0;
	virtual bool Use(ItemUser& user, BlockManager& blockManager) = 0;
};

inline bool operator==(const ItemType& lhs, const ItemType& rhs) {
	return lhs.m_ID == rhs.m_ID;
}

inline bool operator!=(const ItemType& lhs, const ItemType& rhs) {
	return !(lhs == rhs);
}

inline bool operator<(const ItemType& lhs, const ItemType& rhs) {
	return lhs.m_ID < rhs.m_ID;
}

bool operator==(const Item& lhs, const Item& rhs);

inline bool operator!=(const Item& lhs, const Item& rhs)
{
	return !(lhs == rhs);
}