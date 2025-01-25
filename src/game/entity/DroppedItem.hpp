#pragma once

#include <game/physics/Rigidbody.hpp>
#include <game/entity/Entity.hpp>
#include <engine/util/Direction.hpp>
#include <engine/graphics/VertexArray.hpp>
#include <engine/graphics/Vertex.hpp>
#include <game/items/Item.hpp>
#include <game/items/Inventory.hpp>
#include <game/world/Chunk.hpp>

#include <engine/qxml/QXMLReader.hpp>
#include <engine/qxml/QXMLWriter.hpp>

class DroppedItem
{
public:
	DroppedItem(BlockManager& world, ItemStack&& item, glm::vec3 position, glm::vec3 velocity = glm::vec3(0));
	DroppedItem(ItemStack&& stack, const Rigidbody& rigidbody);

	void Update(float deltaTime);
	inline glm::vec3 GetPosition() const;
	void ChangeVelocity(glm::vec3 velocity) { m_Rigidbody.ChangeVelocity(velocity); }
	
	ItemStack& GetItemStack() { return m_ItemStack; }
	const ItemStack& GetItemStack() const { return m_ItemStack; }

	void Serialize(std::vector<char>& data);
	static std::unique_ptr<DroppedItem> Deserialize(const std::vector<char>& data, BlockManager& world);

private:
	static const std::string c_RigidbodyTag,
		c_ItemStackTag,
		c_CountAttribute;

	Rigidbody m_Rigidbody;
	ItemStack m_ItemStack;
};

glm::vec3 DroppedItem::GetPosition() const
{
	return m_Rigidbody.GetPosition();
}