#include <game/save_loading/BlockDataLoader.hpp>

BlockDataLoader::BlockDataLoader(WholeSaveLoader& loader) :
	m_Loader(loader) {

}

bool BlockDataLoader::LoadChunk(Chunk& chunk) {
	std::shared_ptr<WholeSaveLoader::RegionFile> region = m_Loader.GetRegion(WholeSaveLoader::GetRegionPosition(chunk.GetPosition()));

	auto inRegPos = WholeSaveLoader::GetInRegionPosition(chunk.GetPosition());
	if (!region->IsChunkPresent(inRegPos))
		return false;

	WholeSaveLoader::ChunkRecord& chunkRecord = region->GetChunkRecord(inRegPos);

	std::vector<char> blockdata;
	if (!chunkRecord.GetAccess()->GetRawBlockData(blockdata))
		return false;

	chunk.Deserialize(blockdata);
	return true;
}

bool BlockDataLoader::IsPresent(const ChunkPos& position) {
	std::shared_ptr<WholeSaveLoader::RegionFile> region = m_Loader.GetRegion(WholeSaveLoader::GetRegionPosition(position));
	return region->IsChunkPresent(WholeSaveLoader::GetInRegionPosition(position));
}

void BlockDataLoader::SaveChunk(const Chunk& chunk) {
	std::shared_ptr<WholeSaveLoader::RegionFile> region = m_Loader.GetRegion(WholeSaveLoader::GetRegionPosition(chunk.GetPosition()));

	WholeSaveLoader::ChunkRecord& chunkRecord = region->GetChunkRecord(WholeSaveLoader::GetInRegionPosition(chunk.GetPosition()));
	std::vector<char> blockdata;
	chunk.Serialize(blockdata);

	chunkRecord.GetAccess()->SaveRawBlockData(std::move(blockdata));
}
