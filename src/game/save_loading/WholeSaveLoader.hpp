#pragma once

#include <game/world/Chunk.hpp>
#include <string>
#include <fstream>
#include <unordered_map>

#include <zlib.h>
#include <engine/qxml/Element.hpp>
#include <engine/qxml/QXMLReader.hpp>
#include <engine/qxml/QXMLWriter.hpp>


/*
*	<chunk x= y= z=>
*		<blockdata raw=SIZE>SERIALIZED DATA</blockdata>
*		<items>
*			<item raw=SIZE>SERIALIZED DATA</item>
*		</items>
*	</chunk>
*/

class WholeSaveLoader {
public:	
	static const std::string c_SavesPath,
		c_GeneralFileName;

	WholeSaveLoader(const std::string& saveName);

private:
	static const std::string c_BaseTag,
		c_BlockdataTag,
		c_ItemdataTag;

public:
	typedef glm::vec<3, uint8_t> InRegionPosition;
	typedef glm::ivec3 RegionPosition;

	enum class RawDataType {
		BlockData,
		DroppedItemData
	};
	class ChunkRecordAccess {
	public:
		ChunkRecordAccess(std::map<RawDataType, std::vector<char>>& data, std::mutex& regionMutex);
		ChunkRecordAccess(const ChunkRecordAccess&) = delete;
		ChunkRecordAccess& operator=(const ChunkRecordAccess&) = delete;
		ChunkRecordAccess(ChunkRecordAccess&&) = delete;
		ChunkRecordAccess& operator=(ChunkRecordAccess&&) = delete;

		bool GetRawBlockData(std::vector<char>& data);
		bool SaveRawBlockData(std::vector<char>&& data);

		bool GetRawDroppedItemData(std::vector<char>& data);
		bool SaveRawDroppedItemData(std::vector<char>&& data);

		bool IsDataPresent(RawDataType dataType);
	private:
		std::map<RawDataType, std::vector<char>>& m_Data;

		std::lock_guard<std::mutex> m_Lock;
	};

	class ChunkRecord {
	public:
		ChunkRecord(InRegionPosition position, std::mutex& regionMutex);
		ChunkRecord(const ChunkRecord&) = delete;
		ChunkRecord& operator=(const ChunkRecord&) = delete;
		ChunkRecord(ChunkRecord&&) = delete;
		ChunkRecord& operator=(ChunkRecord&&) = delete;

		static std::unique_ptr<ChunkRecord> ReadElement(const QXML::Element& record, std::mutex& regionMutex);
		bool Empty() const;
		std::unique_ptr<ChunkRecordAccess> GetAccess();
		QXML::Element GenerateElement();
		InRegionPosition GetPosition() { return m_Position; }
	private:
		std::map<RawDataType, std::vector<char>> m_Data;
		InRegionPosition m_Position;

		std::mutex& m_RegionMutex;

		bool readElement(const QXML::Element& element);
	};

	class RegionFile //Possibility of deadlock: dont mess with region data when having active ChunkRecordAccess
	{
	public:
		static constexpr glm::ivec3 c_RegionSize = { 32, 3, 32 };

		RegionFile(const std::string filename);
		~RegionFile();
		RegionFile(const RegionFile&) = delete;
		RegionFile& operator=(const RegionFile&) = delete;

		ChunkRecord& GetChunkRecord(const InRegionPosition& position);
		bool IsChunkPresent(const InRegionPosition& position) const;
	private:
		std::unordered_map<InRegionPosition, std::unique_ptr<ChunkRecord>> m_ChunkCache;
		std::string m_Filename;

		mutable std::mutex m_AccessMutex;

		void flush();
		bool loadIntoCache();
	};

	class GeneralFile {
	public:
		GeneralFile(const std::string& filename);
		~GeneralFile();
		GeneralFile(const GeneralFile&) = delete;
		GeneralFile& operator=(const GeneralFile&) = delete;

		bool GetRawPlayerData(std::vector<char>& data);
		void SaveRawPlayerData(const std::vector<char>& data);

	private:
		static const std::string c_PlayerdataTag;
		std::string m_Filename;

		std::vector<char> m_PlayerData;

		bool readFile();
		void saveFile();
	};


private:
	std::mutex m_OpenedRegionsMutex;
	std::unordered_map<RegionPosition, std::shared_ptr<RegionFile>> m_OpenedRegions;
	std::shared_ptr<GeneralFile> m_GeneralFile;

	const std::string m_SavePath;

	std::string getRegionFilename(const RegionPosition& position);

public:
	std::shared_ptr<RegionFile> GetRegion(const RegionPosition& position);
	std::shared_ptr<GeneralFile> GetGeneral();
	void Flush();

	inline static InRegionPosition GetInRegionPosition(const ChunkPos& chunkPosition);
	inline static RegionPosition GetRegionPosition(const ChunkPos& chunkPosition);
	inline static ChunkPos GetChunkPos(const RegionPosition& regionPosition, const InRegionPosition& inRegionPosition);
};

WholeSaveLoader::InRegionPosition WholeSaveLoader::GetInRegionPosition(const ChunkPos& chunkPosition)
{
	return Math::Mod(chunkPosition, RegionFile::c_RegionSize);
}

WholeSaveLoader::RegionPosition WholeSaveLoader::GetRegionPosition(const ChunkPos& chunkPosition)
{
	return glm::floor((glm::vec3)chunkPosition / (glm::vec3)RegionFile::c_RegionSize);
}

ChunkPos WholeSaveLoader::GetChunkPos(const RegionPosition& regionPosition, const InRegionPosition& inRegionPosition)
{
	return regionPosition * RegionFile::c_RegionSize + (glm::ivec3)inRegionPosition;
}