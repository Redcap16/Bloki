#include <game/save_loading/ItemDataLoader.hpp>

ItemDataLoader::ItemDataLoader(WholeSaveLoader& loader) :
	m_Loader(loader) {

}

void ItemDataLoader::GetDroppedItems(const ChunkPos& position, BlockManager& world, std::set<std::shared_ptr<DroppedItem>>& items) {
	std::shared_ptr<WholeSaveLoader::RegionFile> region = m_Loader.GetRegion(WholeSaveLoader::GetRegionPosition(position));
	WholeSaveLoader::ChunkRecord& chunkRecord = region->GetChunkRecord(WholeSaveLoader::GetInRegionPosition(position));

	std::vector<char> itemdata;
	if (!chunkRecord.GetAccess()->GetRawDroppedItemData(itemdata))
		return;

	deserializeItems(items, world, itemdata);
}

void ItemDataLoader::SaveDroppedItems(const ChunkPos& position, const std::set<std::shared_ptr<DroppedItem>>& items) {
	std::shared_ptr<WholeSaveLoader::RegionFile> region = m_Loader.GetRegion(WholeSaveLoader::GetRegionPosition(position));
	WholeSaveLoader::ChunkRecord& chunkRecord = region->GetChunkRecord(WholeSaveLoader::GetInRegionPosition(position));

	std::vector<char> itemdata;
	serializeItems(items, itemdata);

	chunkRecord.GetAccess()->SaveRawDroppedItemData(std::move(itemdata));
}

void ItemDataLoader::serializeItems(const std::set<std::shared_ptr<DroppedItem>>& items, std::vector<char>& data) {
	QXML::QXMLWriter writer;

	DroppedItemSerializer serializer;
	for (auto& item : items) {
		QXML::Element itemElement(c_ItemTag);
		std::vector<char> itemData;
		serializer.Serialize(*item, itemData);

		itemElement.SetAsRaw();
		itemElement.AddData(itemData);
		writer.AddElement(itemElement);
	}

	data = writer.GetResult();
}

void ItemDataLoader::deserializeItems(std::set<std::shared_ptr<DroppedItem>>& items, BlockManager& world, const std::vector<char>& data) {
	QXML::QXMLReader reader(data);

	auto* itemElements = reader.GetBase().GetElementsByTag(c_ItemTag);
	if (itemElements == nullptr)
		return;

	DroppedItemSerializer serializer;
	for (auto& item : *itemElements) {
		std::vector<char> itemData;
		itemData = item.GetData();

		items.insert(serializer.Deserialize(itemData, world));
	}
}