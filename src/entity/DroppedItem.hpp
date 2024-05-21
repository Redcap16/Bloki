#pragma once

#include <physics/Rigidbody.hpp>
#include <entity/Entity.hpp>
#include <util/Direction.hpp>
#include <graphics/VertexArray.hpp>
#include <graphics/Vertex.hpp>
#include <items/Item.hpp>
#include <items/Inventory.hpp>

class DroppedItem
{
public:
	DroppedItem(BlockManager& world, ItemStack&& item, glm::vec3 position, glm::vec3 velocity = glm::vec3(0));

	void Update(float deltaTime);
	inline glm::vec3 GetPosition() const;
	
	ItemStack& GetItemStack() { return m_ItemStack; }
	const ItemStack& GetItemStack() const { return m_ItemStack; }
private:
	Rigidbody m_Rigidbody;
	ItemStack m_ItemStack;
};

glm::vec3 DroppedItem::GetPosition() const
{
	return m_Rigidbody.GetPosition();
}

class DroppedItemRepositoryListener {
public:
	virtual void LoadedDroppedItem(DroppedItem& item) = 0;
	virtual void UnloadedDroppedItem(DroppedItem& item) = 0;
	virtual void CreatedDroppedItem(DroppedItem& item) = 0;
	virtual void DestroyedDroppedItem(DroppedItem& item) = 0;
};

class DroppedItemRepository {
public:
	DroppedItemRepository(BlockManager& blockManager);

	void AddListener(DroppedItemRepositoryListener& listener) { m_Listeners.insert(&listener); };
	void RemoveListener(DroppedItemRepositoryListener& listener) { m_Listeners.erase(&listener); };

	std::weak_ptr<DroppedItem> AddDroppedItem(ItemStack&& item, glm::vec3 position, glm::vec3 velocity = glm::vec3(0));
	bool RemoveDroppedItem(const DroppedItem* item);
	const std::vector<std::shared_ptr<DroppedItem>> GetDroppedItems() const { return m_Items; }

	std::vector<DroppedItem*> FindsItemNearby(glm::vec3 position, float range);

	void Update(float deltaTime);
	void SetCenterChunk(glm::ivec3 chunkPos);

private:
	const glm::ivec3 c_WorkingArea = { 5, 3, 5 };
	glm::ivec3 m_CenterChunkPosition;
	BlockManager& m_BlockManager;

	std::vector<std::shared_ptr<DroppedItem>> m_Items;
	std::set<DroppedItemRepositoryListener*> m_Listeners;

	bool inArea(glm::ivec3 itemPosition);
	void loadChunk(glm::ivec3 position);

	void itemDestroyed(DroppedItem& item);
};