#include <game/world/ChunkFileLoader.hpp>

ChunkFileLoader::ChunkFileLoader(const std::string& savePath) :
	m_SavePath(savePath)
{

}

bool ChunkFileLoader::LoadChunk(Chunk& chunk)
{
	//Get region
	//Load data from it
	//Deserailize data in chunk
	RegionPosition regionPos = RegionFile::GetRegionPosition(chunk.GetPosition());
	InRegionPosition inRegionPos = RegionFile::GetInRegionPosition(chunk.GetPosition());

	std::shared_ptr<RegionFile> region = getRegion(regionPos);
	std::vector<char> data;
	if (!region->LoadChunk(inRegionPos, data))
		return false;

	chunk.Deserialize(data);

	return true;
}

bool ChunkFileLoader::IsPresent(const ChunkPos& position)
{
	//Get region
	//Check if it's present
	RegionPosition regionPos = RegionFile::GetRegionPosition(position);
	InRegionPosition inRegionPos = RegionFile::GetInRegionPosition(position);

	std::shared_ptr<RegionFile> region = getRegion(regionPos);

	return region->IsPresent(inRegionPos);
}

void ChunkFileLoader::SaveChunk(const Chunk& chunk)
{
	//Get region
	//Serialize data
	//Save data in there
	std::vector<char> data;
	chunk.Serialize(data);

	RegionPosition regionPos = RegionFile::GetRegionPosition(chunk.GetPosition());
	InRegionPosition inRegionPos = RegionFile::GetInRegionPosition(chunk.GetPosition());

	std::shared_ptr<RegionFile> region = getRegion(regionPos);
	region->SaveChunk(inRegionPos, std::move(data));
}

void ChunkFileLoader::Flush()
{
	m_OpenedRegions.clear();
	//Clear opened regions, they will flush automatically
}

ChunkFileLoader::RegionFile::RegionFile(const std::string filename) :
	m_Filename(filename),
	m_Changed(false)
{	
	loadIntoCache();
}

ChunkFileLoader::RegionFile::~RegionFile()
{
	flush();
}

bool ChunkFileLoader::RegionFile::LoadChunk(const InRegionPosition position, std::vector<char>& data)
{	
	//Check if theres something in cache
	//If there is load it
	//If there's not
	//Check if there's something in headers
	//If there is, load it from headers and save it in cache
	//If it's not, then
	//Return false
	std::lock_guard<std::mutex> lock(m_AccessMutex);

	if (m_ChunkCache.find(position) != m_ChunkCache.end())
	{
		data = m_ChunkCache[position];
		return true;
	}

	data = m_ChunkCache[position];
	return true;
}

bool ChunkFileLoader::RegionFile::IsPresent(const InRegionPosition& position)
{
	//Firstly check in cache
	//If there's nothing
	//Check in headers
	std::lock_guard<std::mutex> lock(m_AccessMutex);

	if (m_ChunkCache.find(position) != m_ChunkCache.end())
		return true;

	return false;
}

void ChunkFileLoader::RegionFile::SaveChunk(const InRegionPosition& position, std::vector<char>&& data)
{
	//Save it to cache
	//Dont create any headers
	//Mark file as changed
	std::lock_guard<std::mutex> lock(m_AccessMutex);

	m_ChunkCache[position] = std::move(data);
	m_Changed = true;
}

void ChunkFileLoader::RegionFile::flush()
{
	//If changed
	//For every header that dont have cache ready, load its contents into a cache
	//For every cache load it into new, clear file and save its new header
	//Save all new headers on the beggining of the file
	std::lock_guard<std::mutex> lock(m_AccessMutex);

	if (!m_Changed)
		return;

	//Prepare data
	QXML::QXMLWriter qxmlWriter(m_Filename);

	for (int x = 0; x < c_RegionSize.x; ++x)
		for (int y = 0; y < c_RegionSize.y; ++y)
			for (int z = 0; z < c_RegionSize.z; ++z)
			{
				const InRegionPosition position{ x, y, z };

				if (m_ChunkCache.find(position) == m_ChunkCache.end()) //Nothing to save
					continue;

				QXML::Element chunkEl("chunk");
				chunkEl.AddAttribute(QXML::Attribute("x", x));
				chunkEl.AddAttribute(QXML::Attribute("y", y));
				chunkEl.AddAttribute(QXML::Attribute("z", z));

				QXML::Element blockdataEl("blockdata");
				blockdataEl.SetAsRaw();
				blockdataEl.AddData(m_ChunkCache[position]);

				chunkEl.AddInnerElement(blockdataEl);
				qxmlWriter.AddElement(chunkEl);
			}
}

void ChunkFileLoader::RegionFile::loadIntoCache()
{
	QXML::QXMLReader reader = QXML::QXMLReader::OpenFile(m_Filename);
	std::vector<QXML::Element> chunks = reader.GetBase().GetElementsByTag("chunk");

	for (auto& chunk : chunks) {
		int x = chunk.GetAttributeValue("x").m_Value,
			y = chunk.GetAttributeValue("y").m_Value,
			z = chunk.GetAttributeValue("z").m_Value;

		auto insides = chunk.GetElementsByTag("blockdata");
		if (insides.size() != 1)
			continue;

		m_ChunkCache[{x, y, z}] = insides[0].GetData();
	}
}

std::shared_ptr<ChunkFileLoader::RegionFile> ChunkFileLoader::getRegion(const RegionPosition& position)
{
	std::lock_guard<std::mutex> lock(m_OpenedRegionsMutex);
	if (m_OpenedRegions.find(position) == m_OpenedRegions.end())
	{
		std::string filename = m_SavePath + "/" + std::to_string(position.x) + "'" + std::to_string(position.y) + "'" + std::to_string(position.z) + ".reg";
		m_OpenedRegions[position] = std::make_shared<RegionFile>(filename);
	}

	return m_OpenedRegions.at(position);
}