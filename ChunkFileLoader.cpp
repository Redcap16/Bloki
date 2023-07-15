#include <world/ChunkFileLoader.hpp>

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
	m_File(filename, std::ios_base::in | std::ios_base::out | std::ios_base::binary),
	m_Filename(filename)
{
	if(m_File.peek() != std::fstream::traits_type::eof()) //File is not empty
		readHeaders();
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


	if (!loadIntoCache(position))
		return false;

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
	if (getHeaderData(position) != nullptr)
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

	size_t currentAddress = c_RegionSize.x * c_RegionSize.y * c_RegionSize.z * sizeof(ChunkHeaderData);
	m_File.seekp(0, std::ios_base::beg);
	{
		std::vector<char> headerSection(currentAddress);
		m_File.write(headerSection.data(), currentAddress);
	}

	for (int x = 0; x < c_RegionSize.x; ++x)
		for (int y = 0; y < c_RegionSize.y; ++y)
			for (int z = 0; z < c_RegionSize.z; ++z)
			{
				const InRegionPosition position{ x, y, z };
				if (!loadIntoCache(position)) //Nothing to save
					continue;

				if (!m_File.good())
				{
					DEBUG_LOG("Error: Cant access file in " + m_Filename);
					return;
				}

				//Write the data
				m_File.seekp(currentAddress, std::ios_base::beg);
				if (!m_File.good())
				{
					DEBUG_LOG("Error: Cant access file in " + m_Filename);
					return;
				}

				m_File.write(m_ChunkCache[position].data(), m_ChunkCache.size());

				//Write the header
				const size_t currentHeaderAddress = x * c_RegionSize.y * c_RegionSize.z + y * c_RegionSize.z + z;
				m_File.seekp(currentHeaderAddress, std::ios_base::beg);
				if (!m_File.good())
				{
					DEBUG_LOG("Error: Cant access file in " + m_Filename);
					return;
				}

				ChunkHeaderData buffer;
				buffer.Address = currentAddress;
				buffer.Size = m_ChunkCache[position].size();

				m_File.write((char*)&buffer, sizeof(buffer));
				currentAddress += m_ChunkCache.size();
			}
}

void ChunkFileLoader::RegionFile::readHeaders()
{
	m_File.seekg(0, std::ios_base::beg);

	ChunkHeaderData buffer;
	for(int x = 0; x < c_RegionSize.x; ++x)
		for(int y = 0; y < c_RegionSize.y; ++y)
			for (int z = 0; z < c_RegionSize.z; ++z)
			{
				if (m_File.good())
				{
					DEBUG_LOG("Error: cant read chunk headers");
					return;
				}

				m_File.read((char*)&buffer, sizeof(buffer));
				m_ChunkHeaderData[x][y][z] = buffer;
			}
}

const ChunkFileLoader::RegionFile::ChunkHeaderData* ChunkFileLoader::RegionFile::getHeaderData(const InRegionPosition& position)
{
	if (position.x >= c_RegionSize.x ||
		position.y >= c_RegionSize.y ||
		position.z >= c_RegionSize.z)
		return nullptr;
	return &m_ChunkHeaderData[position.x][position.y][position.z];
}

bool ChunkFileLoader::RegionFile::loadIntoCache(const InRegionPosition& position)
{
	if (m_ChunkCache.find(position) != m_ChunkCache.end())
		return true; //It is there, so it dont have to be loaded

	const ChunkHeaderData* header = getHeaderData(position);
	if (header == nullptr)
		return false;

	if (!m_File.good())
	{
		DEBUG_LOG("Error: Cant access file in " + m_Filename);
		return false;
	}
	m_File.seekg(header->Address, std::ios_base::beg);

	if (!m_File.good())
	{
		DEBUG_LOG("Error: Cant access file in " + m_Filename);
		return false;
	}

	std::vector<char>& data = m_ChunkCache[position];
	data.reserve(header->Size);
	m_File.read(data.data(), header->Size);

	if (data.size() != header->Size)
	{
		DEBUG_LOG("Error: Cant read all of bytes of a chunk in " + m_Filename);
		return false;
	}
	m_ChunkCache.erase(position);
}

std::shared_ptr<ChunkFileLoader::RegionFile> ChunkFileLoader::getRegion(const RegionPosition& position)
{
	std::lock_guard<std::mutex> lock(m_OpenedRegionsMutex);
	if (m_OpenedRegions.find(position) == m_OpenedRegions.end())
	{
		std::string filename = m_SavePath + "/" + std::to_string(position.x) + "-" + std::to_string(position.y) + "-" + std::to_string(position.z) + ".reg";
		m_OpenedRegions[position] = std::make_shared<RegionFile>(m_SavePath);
	}

	return m_OpenedRegions.at(position);
}