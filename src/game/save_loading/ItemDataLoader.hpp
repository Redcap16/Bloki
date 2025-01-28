#pragma once

#include <game/save_loading/WholeSaveLoader.hpp>

class ItemDataLoader {
public:
	ItemDataLoader(WholeSaveLoader& loader);

	void GetDroppedItems(const ChunkPos& position, BlockManager& world, std::set<std::shared_ptr<DroppedItem>>& items);
	void SaveDroppedItems(const ChunkPos& position, const std::set<std::shared_ptr<DroppedItem>>& items);

private:
	const std::string c_ItemTag = "item";

	WholeSaveLoader& m_Loader;

	void serializeItems(const std::set<std::shared_ptr<DroppedItem>>& items, std::vector<char>& data);
	void deserializeItems(std::set<std::shared_ptr<DroppedItem>>& items, BlockManager& world, const std::vector<char>& data);
};