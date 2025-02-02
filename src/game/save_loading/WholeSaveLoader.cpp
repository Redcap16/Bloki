#include <game/save_loading/WholeSaveLoader.hpp>

const std::string WholeSaveLoader::c_SavesPath = "saves/",
	WholeSaveLoader::c_GeneralFileName = "general.dat",
	WholeSaveLoader::c_BaseTag = "chunk",
	WholeSaveLoader::c_BlockdataTag = "blockdata",
	WholeSaveLoader::c_ItemdataTag = "itemdata";

WholeSaveLoader::WholeSaveLoader(const std::string& saveName) :
	m_SavePath(saveName) {
}

WholeSaveLoader::ChunkRecordAccess::ChunkRecordAccess(std::map<RawDataType, std::vector<char>>& data, std::mutex& regionMutex) :
	m_Data(data),
	m_Lock(regionMutex) {

}

bool WholeSaveLoader::ChunkRecordAccess::GetRawBlockData(std::vector<char>& data) {
	if (!IsDataPresent(RawDataType::BlockData))
		return false;

	data = m_Data[RawDataType::BlockData];
	return true;
}

bool WholeSaveLoader::ChunkRecordAccess::SaveRawBlockData(std::vector<char>&& data) {
	m_Data[RawDataType::BlockData] = std::move(data);
	return true;
}

bool WholeSaveLoader::ChunkRecordAccess::GetRawDroppedItemData(std::vector<char>& data) {
	if (!IsDataPresent(RawDataType::DroppedItemData))
		return false;

	data = m_Data[RawDataType::DroppedItemData];
	return true;
}

bool WholeSaveLoader::ChunkRecordAccess::SaveRawDroppedItemData(std::vector<char>&& data) {
	m_Data[RawDataType::DroppedItemData] = std::move(data); //Raw saving, DroppedItemLoader is responsible for concatenation
	return true;
}

bool WholeSaveLoader::ChunkRecordAccess::IsDataPresent(RawDataType dataType) {
	return m_Data.find(dataType) != m_Data.end();
}

WholeSaveLoader::ChunkRecord::ChunkRecord(InRegionPosition position, std::mutex& regionMutex) :
	m_RegionMutex(regionMutex),
	m_Position(position) {
}

std::unique_ptr<WholeSaveLoader::ChunkRecord> WholeSaveLoader::ChunkRecord::ReadElement(const QXML::Element& record, std::mutex& regionMutex) {
	if (!record.HasAtribute("x") ||
		!record.HasAtribute("y") ||
		!record.HasAtribute("z"))
		return nullptr;

	InRegionPosition position = { record.GetAttributeValue("x").m_Value,
		record.GetAttributeValue("y").m_Value,
		record.GetAttributeValue("z").m_Value };

	std::unique_ptr<ChunkRecord> result = std::make_unique<ChunkRecord>(position, regionMutex);

	auto* blockdata = record.GetElementsByTag(c_BlockdataTag),
		* itemdata = record.GetElementsByTag(c_ItemdataTag);

	if (blockdata != nullptr && blockdata->size() == 1)
		result->m_Data[RawDataType::BlockData] = (*blockdata)[0].GetData();
	else
		return nullptr;

	if (itemdata != nullptr) {
		if (itemdata->size() == 1)
			result->m_Data[RawDataType::DroppedItemData] = (*itemdata)[0].GetData();
		else if (itemdata->size() > 1)
			return nullptr;
	}

	return result;
}

bool WholeSaveLoader::ChunkRecord::Empty() const {
	for (auto& data : m_Data)
		if (!data.second.empty())
			return false;
	return true;
}

std::unique_ptr<WholeSaveLoader::ChunkRecordAccess> WholeSaveLoader::ChunkRecord::GetAccess() {
	return std::make_unique<ChunkRecordAccess>(m_Data, m_RegionMutex);
}

QXML::Element WholeSaveLoader::ChunkRecord::GenerateElement() {
	QXML::Element result(c_BaseTag);
	result.AddAttribute(QXML::Attribute("x", m_Position.x));
	result.AddAttribute(QXML::Attribute("y", m_Position.y));
	result.AddAttribute(QXML::Attribute("z", m_Position.z));

	QXML::Element blockdata(c_BlockdataTag);
	blockdata.SetAsRaw();
	blockdata.AddData(m_Data[RawDataType::BlockData]);
	result.AddInnerElement(blockdata);

	if (!m_Data[RawDataType::DroppedItemData].empty()) {
		QXML::Element itemdata(c_ItemdataTag);
		itemdata.SetAsRaw();
		itemdata.AddData(m_Data[RawDataType::DroppedItemData]);
		result.AddInnerElement(itemdata);
	}

	return result;
}

WholeSaveLoader::RegionFile::RegionFile(const std::string filename) :
	m_Filename(filename) {
	loadIntoCache();
}

WholeSaveLoader::RegionFile::~RegionFile() {
	flush();
}

WholeSaveLoader::ChunkRecord& WholeSaveLoader::RegionFile::GetChunkRecord(const InRegionPosition& position) {
	std::lock_guard<std::mutex> lock(m_AccessMutex);

	if (m_ChunkCache.find(position) == m_ChunkCache.end())
		m_ChunkCache[position] = std::make_unique<ChunkRecord>(position, m_AccessMutex);
	return *m_ChunkCache.at(position);
}

bool WholeSaveLoader::RegionFile::IsChunkPresent(const InRegionPosition& position) const {
	std::lock_guard<std::mutex> lock(m_AccessMutex);

	if (m_ChunkCache.find(position) != m_ChunkCache.end())
		return !m_ChunkCache.at(position)->Empty();

	return false;
}

void WholeSaveLoader::RegionFile::flush() {
	std::lock_guard<std::mutex> lock(m_AccessMutex);

	QXML::QXMLWriter qxmlWriter(m_Filename);

	for (auto& chunk : m_ChunkCache) {
		qxmlWriter.AddElement(chunk.second->GenerateElement());
	}
}

bool WholeSaveLoader::RegionFile::loadIntoCache() {
	std::lock_guard<std::mutex> lock(m_AccessMutex);

	QXML::QXMLReader reader = QXML::QXMLReader::OpenFile(m_Filename);
	const std::vector<QXML::Element>* chunks = reader.GetBase().GetElementsByTag(c_BaseTag);

	if (chunks == nullptr)
		return true; //No chunks to load

	for (auto& chunk : *chunks) {
		std::unique_ptr<ChunkRecord> record = ChunkRecord::ReadElement(chunk, m_AccessMutex);
		if (record == nullptr)
			return false;

		m_ChunkCache[record->GetPosition()] = std::move(record);
	}
	return true;
}

const std::string WholeSaveLoader::GeneralFile::c_PlayerdataTag = "playerdata";

WholeSaveLoader::GeneralFile::GeneralFile(const std::string& filename) :
	m_Filename(filename) {
	readFile();
}

WholeSaveLoader::GeneralFile::~GeneralFile() {
	saveFile();
}

bool WholeSaveLoader::GeneralFile::GetRawPlayerData(std::vector<char>& data) {
	data = m_PlayerData;
	return true;
}

void WholeSaveLoader::GeneralFile::SaveRawPlayerData(const std::vector<char>& data) {
	m_PlayerData = data;
}

bool WholeSaveLoader::GeneralFile::readFile() {
	QXML::QXMLReader reader = QXML::QXMLReader::OpenFile(m_Filename);

	auto* playerdataElement = reader.GetBase().GetElementsByTag(c_PlayerdataTag);
	if (playerdataElement == nullptr ||
		playerdataElement->size() != 1)
		return false;

	m_PlayerData = (*playerdataElement)[0].GetData();
	return true;
}

void WholeSaveLoader::GeneralFile::saveFile() {
	QXML::QXMLWriter writer(m_Filename);

	QXML::Element playerdataElement(c_PlayerdataTag);
	playerdataElement.SetAsRaw();
	playerdataElement.AddData(m_PlayerData);
	writer.AddElement(playerdataElement);
}

std::string WholeSaveLoader::getRegionFilename(const RegionPosition& position) {
	return c_SavesPath + m_SavePath + "/" +
		std::to_string(position.x) + "'" +
		std::to_string(position.y) + "'" +
		std::to_string(position.z) + ".reg";
}

std::shared_ptr<WholeSaveLoader::RegionFile> WholeSaveLoader::GetRegion(const RegionPosition& position) {
	std::lock_guard<std::mutex> lock(m_OpenedRegionsMutex);

	if (m_OpenedRegions.find(position) == m_OpenedRegions.end())
		m_OpenedRegions[position] = std::make_shared<RegionFile>(getRegionFilename(position));

	return m_OpenedRegions.at(position);
}

std::shared_ptr<WholeSaveLoader::GeneralFile> WholeSaveLoader::GetGeneral() {
	if (m_GeneralFile == nullptr)
		m_GeneralFile = std::make_shared<GeneralFile>(c_SavesPath + m_SavePath + "/" + c_GeneralFileName);

	return m_GeneralFile;
}

void WholeSaveLoader::Flush() {
	m_OpenedRegions.clear();
	m_GeneralFile = nullptr;
}