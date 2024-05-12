#pragma once

#include <world/BlockManager.hpp>
#include <memory>
#include <array>
#include <map>

struct ItemTextureData {
	glm::ivec2 Position;

	ItemTextureData(glm::ivec2 position) :
		Position(position) {

	}
};

class ItemUser {
public:
	virtual void Heal(float health) = 0;
	virtual WorldPos GetLookingAt() = 0;
	virtual WorldPos GetPlacingAt() = 0;
};

class ItemType {
private:
	unsigned int m_Id;

	ItemType(unsigned int id) :
		m_Id(id) {
	}
public:
	static ItemType Create() {
		static unsigned int lastId = 1;
		return { lastId++ };
	}

	friend bool operator==(const ItemType& lhs, const ItemType& rhs);
	friend bool operator<(const ItemType& lhs, const ItemType& rhs);
};

template <class TSubType>
class ItemTypeArray {
public:
	ItemType GetType(TSubType subType);
	ItemType operator[](TSubType subType);

private:
	std::map<TSubType, ItemType> m_Types;
};

template <class TSubType>
ItemType ItemTypeArray<TSubType>::GetType(TSubType subType) {
	if (m_Types.find(subType) == m_Types.end()) {
		return m_Types.insert({ subType, ItemType::Create() }).first->second;
	}

	return m_Types.at(subType);
}

template <class TSubType>
ItemType ItemTypeArray<TSubType>::operator[](TSubType subType) {
	return GetType(subType);
}

class Item
{
public:
	static const int c_MaxStackCount = 64;

	virtual std::unique_ptr<Item> Clone() const = 0;

	virtual ItemType GetType() const = 0;
	virtual std::string GetName() const = 0;
	virtual ItemTextureData GetTextureData() const = 0;
	virtual bool Use(ItemUser& user, BlockManager& blockManager) = 0;
};

inline bool operator==(const ItemType& lhs, const ItemType& rhs) {
	return lhs.m_Id == rhs.m_Id;
}

inline bool operator!=(const ItemType& lhs, const ItemType& rhs) {
	return !(lhs == rhs);
}

inline bool operator<(const ItemType& lhs, const ItemType& rhs) {
	return lhs.m_Id < rhs.m_Id;
}

bool operator==(const Item& lhs, const Item& rhs);

inline bool operator!=(const Item& lhs, const Item& rhs)
{
	return !(lhs == rhs);
}