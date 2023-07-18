#pragma once

#include <world/Chunk.hpp>
#include <string>
#include <fstream>
#include <zlib.h>

class ChunkFileLoader
{
public:
	ChunkFileLoader(const std::string& savePath);

	bool LoadChunk(Chunk& chunk);
	bool IsPresent(const ChunkPos& position);
	void SaveChunk(const Chunk& chunk);
	void Flush();
private:
	typedef glm::vec<3, uint8_t> InRegionPosition;
	typedef glm::ivec3 RegionPosition;
	class RegionFile
	{
	public:
		static constexpr glm::ivec3 c_RegionSize = { 32, 3, 32 };

		RegionFile(const std::string filename);
		~RegionFile();
		RegionFile(const RegionFile&) = delete;
		RegionFile& operator=(const RegionFile&) = delete;

		bool LoadChunk(const InRegionPosition position, std::vector<char>& data);
		bool IsPresent(const InRegionPosition& position);
		void SaveChunk(const InRegionPosition& position, std::vector<char>&& data);

		inline static InRegionPosition GetInRegionPosition(const ChunkPos& chunkPosition);
		inline static RegionPosition GetRegionPosition(const ChunkPos& chunkPosition);
		inline static ChunkPos GetChunkPos(const RegionPosition& regionPosition, const InRegionPosition& inRegionPosition);
	private:
		struct ChunkHeaderData
		{
			uint32_t Address,
				Size;
		};
		ChunkHeaderData m_ChunkHeaderData[c_RegionSize.x][c_RegionSize.y][c_RegionSize.z];
		std::unordered_map<InRegionPosition, std::vector<char>> m_ChunkCache;

		std::fstream m_File;
		std::string m_Filename;

		std::mutex m_AccessMutex;

		bool m_Changed;

		void flush();
		void readHeaders();
		const ChunkHeaderData* getHeaderData(const InRegionPosition& position);
		bool loadIntoCache(const InRegionPosition& position);
	};

	std::mutex m_OpenedRegionsMutex;
	std::unordered_map<RegionPosition, std::shared_ptr<RegionFile>> m_OpenedRegions;

	std::string m_SavePath;

	std::shared_ptr<RegionFile> getRegion(const RegionPosition& position);
};

ChunkFileLoader::InRegionPosition ChunkFileLoader::RegionFile::GetInRegionPosition(const ChunkPos& chunkPosition)
{
	return Math::Mod(chunkPosition, c_RegionSize);
}

ChunkFileLoader::RegionPosition ChunkFileLoader::RegionFile::GetRegionPosition(const ChunkPos& chunkPosition)
{
	return glm::floor((glm::vec3)chunkPosition / (glm::vec3)c_RegionSize);
}

ChunkPos ChunkFileLoader::RegionFile::GetChunkPos(const RegionPosition& regionPosition, const InRegionPosition& inRegionPosition)
{
	return regionPosition * c_RegionSize + (glm::ivec3)inRegionPosition;
}