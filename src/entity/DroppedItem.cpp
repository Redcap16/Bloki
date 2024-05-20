#include <entity/DroppedItem.hpp>
#include <world/Chunk.hpp>

DroppedItem::DroppedItem(BlockManager& world, ItemStack&& item, glm::vec3 position) :
	m_Rigidbody(world, {position, glm::vec3(0), glm::vec3(0.5, 0.5, 0.5)}),
	m_ItemStack(std::move(item))
{
}

void DroppedItem::Update(float deltaTime)
{
	m_Rigidbody.Update(deltaTime);
}


DroppedItemRepository::DroppedItemRepository(BlockManager& blockManager) :
	m_BlockManager(blockManager),
	m_CenterChunkPosition(0) {

}

std::weak_ptr<DroppedItem> DroppedItemRepository::AddDroppedItem(ItemStack&& item, glm::vec3 position) {
	std::shared_ptr<DroppedItem> newDroppedItem = std::make_shared<DroppedItem>(m_BlockManager, std::move(item), position);
	std::weak_ptr<DroppedItem> result = newDroppedItem;
	m_Items.emplace_back(std::move(newDroppedItem));

	for (auto listener : m_Listeners)
		listener->CreatedDroppedItem(*m_Items.back());

	return result;
}

bool DroppedItemRepository::RemoveDroppedItem(const DroppedItem* item) {
	for (auto it = m_Items.begin(); it != m_Items.end(); ++it)
		if (it->get() == item) {
			for (auto listener : m_Listeners)
				listener->DestroyedDroppedItem(*it->get());

			m_Items.erase(it);
			return true;
		}

	return false;
}

std::vector<DroppedItem*> DroppedItemRepository::FindsItemNearby(glm::vec3 position, float range) {
	std::vector<DroppedItem*> result;
	for (auto& item : m_Items)
		if (glm::length(item->GetPosition() - position) < range)
			result.push_back(item.get());

	return result;
}

void DroppedItemRepository::Update(float deltaTime) {
	std::set<std::shared_ptr<DroppedItem>> itemsToUnload;
	for (auto it = m_Items.begin(); it != m_Items.end();) {
		(*it)->Update(deltaTime);
		if (!inArea((*it)->GetPosition())) {
			itemsToUnload.insert(std::move(*it));
			it = m_Items.erase(it);
		}
		else
			++it;
	}

	for (auto listener : m_Listeners)
		for(auto& item : itemsToUnload)
			listener->UnloadedDroppedItem(*item);

	//TODO: Save unloaded items
}

void DroppedItemRepository::SetCenterChunk(glm::ivec3 chunkPos) {
	if (chunkPos == m_CenterChunkPosition)
		return;

	for(int x = chunkPos.x - c_WorkingArea.x; x <= chunkPos.x + c_WorkingArea.x; x++)
		for(int y = chunkPos.y - c_WorkingArea.y; y <= chunkPos.y + c_WorkingArea.y; y++)
			for (int z = chunkPos.z - c_WorkingArea.z; z <= chunkPos.z + c_WorkingArea.z; z++) {
				if (!inArea({ x, y, z }))
					loadChunk({ x, y, z });
			}
	m_CenterChunkPosition = chunkPos;
}

bool DroppedItemRepository::inArea(glm::ivec3 itemPosition) {
	glm::ivec3 chunkPosition = Chunk::GetChunkPosition(itemPosition) - m_CenterChunkPosition;
	return chunkPosition.x >= -c_WorkingArea.x &&
		chunkPosition.x <= c_WorkingArea.x &&
		chunkPosition.y >= -c_WorkingArea.y &&
		chunkPosition.y <= c_WorkingArea.y &&
		chunkPosition.z >= -c_WorkingArea.z &&
		chunkPosition.z <= c_WorkingArea.z;
}

void DroppedItemRepository::loadChunk(glm::ivec3 position) {
	//TODO: some loading stuff
	//TODO: Inform listeners about change
}