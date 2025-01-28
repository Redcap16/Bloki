#pragma once

#include <game/save_loading/WholeSaveLoader.hpp>
#include <game/core/Player.hpp>

class PlayerDataLoader {
public:
	PlayerDataLoader(WholeSaveLoader& loader);

	std::unique_ptr<Player> LoadPlayer(BlockManager& world, window::Keyboard& keyboard, window::Mouse& mouse, glm::ivec2 windowSize, DroppedItemRepository& droppedItemRepository);
	void SavePlayer(const Player& player);

private:
	WholeSaveLoader& m_Loader;

};