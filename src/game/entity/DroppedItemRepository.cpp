#include <game/entity/DroppedItemRepository.hpp>

DroppedItemRepository::DroppedItemRepository(BlockManager& blockManager, ItemDataLoader& loader) :
	m_BlockManager(blockManager),
	m_CenterChunkPosition(0),
	m_ItemDataLoader(loader) {

}

DroppedItemRepository::~DroppedItemRepository() {
	getRidOfItemsOutsideArea({}); //Unload and save everything
}

std::weak_ptr<DroppedItem> DroppedItemRepository::AddDroppedItem(ItemStack&& item, glm::vec3 position, glm::vec3 velocity) {
	if (item.Empty())
		return std::weak_ptr<DroppedItem>();

	std::shared_ptr<DroppedItem> newDroppedItem = std::make_shared<DroppedItem>(m_BlockManager, std::move(item), position, velocity);
	std::weak_ptr<DroppedItem> result = newDroppedItem;
	m_Items.insert(newDroppedItem);

	itemCreated(*newDroppedItem);

	return result;
}

bool DroppedItemRepository::RemoveDroppedItem(const DroppedItem* item) {
	for (auto it = m_Items.begin(); it != m_Items.end(); ++it)
		if (it->get() == item) {
			itemDestroyed(*it->get());

			m_Items.erase(it);
			return true;
		}

	return false;
}

std::set<DroppedItem*> DroppedItemRepository::FindsItemNearby(glm::vec3 position, float range) {
	std::set<DroppedItem*> result;
	for (auto& item : m_Items)
		if (glm::length(item->GetPosition() - position) < range)
			result.insert(item.get());
	
	return result;
}

void DroppedItemRepository::Update(float deltaTime) {
	for (auto it = m_Items.begin(); it != m_Items.end();) {
		(*it)->Update(deltaTime);
		if ((*it)->GetItemStack().Empty()) {
			itemDestroyed(*it->get());

			it = m_Items.erase(it);
			continue;
		}

		++it;
	}
}

void DroppedItemRepository::SetLoadedChunks(std::unordered_set<ChunkPos> loadedChunks) {
	getRidOfItemsOutsideArea(loadedChunks);

	for (auto& chunkPos : loadedChunks)
		if (m_LoadedChunks.find(chunkPos) == m_LoadedChunks.end()) //not loaded
			loadChunk(chunkPos);

	m_LoadedChunks = loadedChunks;
}

void DroppedItemRepository::getRidOfItemsOutsideArea(std::unordered_set<ChunkPos> loadedChunks) {
	std::unordered_map<ChunkPos, std::set<std::shared_ptr<DroppedItem>>> chunksToUnload;

	for (auto& item : m_Items) {
		chunksToUnload[Chunk::GetChunkPosition(item->GetPosition())].insert(item);
	}

	for (auto it = chunksToUnload.begin(); it != chunksToUnload.end();) {
		if (loadedChunks.find(it->first) != loadedChunks.end()) //Its in loaded chunks, dont unload it
			it = chunksToUnload.erase(it);
		else
			it++;
	}

	for (auto& chunk : chunksToUnload) {
		unloadChunk(chunk.first, chunk.second);
	}
}

void DroppedItemRepository::loadChunk(ChunkPos position) {
	std::set<std::shared_ptr<DroppedItem>> itemsLoaded;
	m_ItemDataLoader.GetDroppedItems(position, m_BlockManager, itemsLoaded);

	m_Items.insert(itemsLoaded.begin(), itemsLoaded.end());

	for (auto& item : itemsLoaded)
		itemCreated(*item);
}

void DroppedItemRepository::unloadChunk(ChunkPos chunkPosition, std::set<std::shared_ptr<DroppedItem>>& items) {
	for (auto& item : items)
		m_Items.erase(item);

	m_ItemDataLoader.SaveDroppedItems(chunkPosition, items);
}

void DroppedItemRepository::itemCreated(DroppedItem& item) {
	for (auto listener : m_Listeners)
		listener->CreatedDroppedItem(item);
}

void DroppedItemRepository::itemUnloaded(DroppedItem& item) {
	for (auto listener : m_Listeners)
		listener->UnloadedDroppedItem(item);
}

void DroppedItemRepository::itemDestroyed(DroppedItem& item) {
	for (auto listener : m_Listeners)
		listener->DestroyedDroppedItem(item);
}