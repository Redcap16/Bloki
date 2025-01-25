#pragma once

#include <game/entity/DroppedItem.hpp>
#include <game/world/ChunkFileLoader.hpp>
#include <unordered_set>
#include <unordered_map>

class DroppedItemRepositoryListener {
public:
	virtual void LoadedDroppedItem(DroppedItem& item) = 0;
	virtual void UnloadedDroppedItem(DroppedItem& item) = 0;
	virtual void CreatedDroppedItem(DroppedItem& item) = 0;
	virtual void DestroyedDroppedItem(DroppedItem& item) = 0;
};

class DroppedItemRepository {
public:
	DroppedItemRepository(BlockManager& blockManager, ItemDataLoader& loader);
	~DroppedItemRepository();

	void AddListener(DroppedItemRepositoryListener& listener) { m_Listeners.insert(&listener); };
	void RemoveListener(DroppedItemRepositoryListener& listener) { m_Listeners.erase(&listener); };

	std::weak_ptr<DroppedItem> AddDroppedItem(ItemStack&& item, glm::vec3 position, glm::vec3 velocity = glm::vec3(0));
	bool RemoveDroppedItem(const DroppedItem* item);
	const std::set<std::shared_ptr<DroppedItem>> GetDroppedItems() const { return m_Items; }

	std::set<DroppedItem*> FindsItemNearby(glm::vec3 position, float range);

	void Update(float deltaTime);

	void SetLoadedChunks(std::unordered_set<ChunkPos> loadedChunks);

private:
	ItemDataLoader m_ItemDataLoader;

	glm::ivec3 m_CenterChunkPosition;
	BlockManager& m_BlockManager;

	std::set<std::shared_ptr<DroppedItem>> m_Items;
	std::set<DroppedItemRepositoryListener*> m_Listeners;

	std::unordered_set<ChunkPos> m_LoadedChunks;

	void getRidOfItemsOutsideArea(std::unordered_set<ChunkPos> loadedChunks);
	void loadChunk(ChunkPos position);
	void unloadChunk(ChunkPos chunkPosition, std::set<std::shared_ptr<DroppedItem>>& items);

	void itemCreated(DroppedItem& item);
	void itemUnloaded(DroppedItem& item);
	void itemDestroyed(DroppedItem& item);
};