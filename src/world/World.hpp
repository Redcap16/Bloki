#pragma once

#include <world/worldGenerator.hpp>
#include <graphics/ShaderProgram.hpp>
#include <graphics/Camera3D.hpp>
#include <window/window.h>
#include <world/Chunk.hpp>
#include <ui/DebugBox.hpp>
#include <world/ChunkArray.hpp>
#include <world/ChunkRenderer.hpp>
#include <world/BlockArray.hpp>
#include <core/GameState.hpp>

class World : public BlockArray, public Renderable
{
private:
	friend class WorldGenerator;

	ChunkArray m_ChunkArray;
	GameState& m_GameState;

	ChunkRenderer m_ChunkRenderer;

	std::thread generatingThread;
	glm::ivec3 currentCenterPosition = { 0, 0, 0 };

	volatile bool stopUpdatingFlag = false;
	std::thread updateThread;

	WorldGenerator m_Generator;

	bool inBounds(glm::ivec3 local);

	void updateChunksEntry();
	/// <summary>
	/// Gives list of local neighbors
	/// </summary>
	/// <param name="chunkPosition">Local position of chunk</param>
	/// <param name="neighborArray"></param>
	void getNeighborList(glm::uvec3 chunkPosition, Chunk* (&neighborArray)[ChunkBlockData::NeighborCount]);
	void reloadWorld(glm::ivec3 newCenterPosition);
	Chunk* createNewChunk(glm::ivec3 position);
	void deleteChunk(Chunk* chunk);

	Chunk* getChunk(glm::ivec3 position, bool force = false);
	bool isChunkGenerated(glm::ivec3 position);
public:
	World(GameState& gameState);
	~World();

	void LoadWorld();
	void Update();
	void UpdateChunks();
	void Render(const RenderingContext &context) override;

	Chunk* GetChunk(glm::ivec3 position); //TODO: Delete

	const Block* GetBlock(glm::ivec3 position) override;
	void DestroyBlock(glm::ivec3 position) override;
	void PlaceBlock(glm::ivec3 position, const Block& block, bool force = false) override;

	//bool Raytrace(glm::vec3 origin, glm::vec3 direction, glm::ivec3& finalPosition);

	void SetWorldCenter(glm::ivec3 position);
	inline glm::ivec3 GetWorldCenter() { return currentCenterPosition; }

	inline glm::ivec3 ToGlobalFromWorldPosition(glm::vec3 worldPosition)
	{
		const glm::uvec3 chunkSize = ChunkBlockData::ChunkSize;
		return { worldPosition.x / chunkSize.x, worldPosition.y / chunkSize.y, worldPosition.z / chunkSize.z };
	}
	inline glm::ivec3 ToGlobalPosition(glm::uvec3 local)
	{
		return (glm::ivec3)local + currentCenterPosition - glm::ivec3((ChunkArray::Size.x - 1) / 2, (ChunkArray::Size.y - 1) / 2, (ChunkArray::Size.z - 1) / 2);
	}
	inline glm::ivec3 ToLocalPosition(glm::ivec3 global)
	{
		return global - currentCenterPosition + glm::ivec3((ChunkArray::Size.x - 1) / 2, (ChunkArray::Size.y - 1) / 2, (ChunkArray::Size.z - 1) / 2);
	}
};