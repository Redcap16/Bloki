#include <game/save_loading/PlayerDataLoader.hpp>

PlayerDataLoader::PlayerDataLoader(WholeSaveLoader& loader) :
	m_Loader(loader) {

}

std::unique_ptr<Player> PlayerDataLoader::LoadPlayer(BlockManager& world, window::Keyboard& keyboard, window::Mouse& mouse, glm::ivec2 windowSize, DroppedItemRepository& droppedItemRepository) {
	auto generalFile = m_Loader.GetGeneral();
	if (generalFile == nullptr)
		return nullptr;

	std::vector<char> playerdata;
	generalFile->GetRawPlayerData(playerdata);
	PlayerSerializer playerSerializer;
	return std::move(playerSerializer.Deserialize(playerdata, world, keyboard, mouse, windowSize, droppedItemRepository));
}

void PlayerDataLoader::SavePlayer(const Player& player) {
	auto generalFile = m_Loader.GetGeneral();
	if (generalFile == nullptr)
		return;

	std::vector<char> playerdata;
	PlayerSerializer playerSerializer;
	playerSerializer.Serialize(player, playerdata);
	generalFile->SaveRawPlayerData(playerdata);
}