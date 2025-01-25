#include <game/entity/DroppedItem.hpp>
#include <game/world/Chunk.hpp>

DroppedItem::DroppedItem(BlockManager& world, ItemStack&& item, glm::vec3 position, glm::vec3 velocity) :
	m_Rigidbody(world, {position, glm::vec3(0), glm::vec3(0.5, 0.5, 0.5)}),
	m_ItemStack(std::move(item)) {
	m_Rigidbody.SetVelocity(velocity);
}

void DroppedItem::Update(float deltaTime)
{
	m_Rigidbody.Update(deltaTime);
}

void DroppedItem::Serialize(std::vector<char>& data) {
	QXML::QXMLWriter writer;
	DebugProvider::Get().Log("Serializing item");
	//TODO: To implement serialization: change variable number item ids to static string ids
}

std::unique_ptr<DroppedItem> DroppedItem::Deserialize(const std::vector<char>& data, BlockManager& world) {
	DebugProvider::Get().Log("Deserializing item");
	//TODO: To implement serialization: change variable number item ids to static string ids
	return {};
}