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

DroppedItem::DroppedItem(ItemStack&& stack, const Rigidbody& rigidbody) :
	m_ItemStack(std::move(stack)),
	m_Rigidbody(rigidbody) {

}

const std::string DroppedItemSerializer::c_RigidbodyTag = "rigidbody",
DroppedItemSerializer::c_ItemStackTag = "itemstack",
DroppedItemSerializer::c_CountAttribute = "count";

void DroppedItemSerializer::Serialize(const DroppedItem& droppedItem, std::vector<char>& data) {
	DEBUG_LOG("Serializing item");
	QXML::QXMLWriter writer;

	QXML::Element itemstack(c_ItemStackTag);
	itemstack.AddAttribute(QXML::Attribute(c_CountAttribute, droppedItem.m_ItemStack.GetCount()));
	itemstack.AddData(droppedItem.m_ItemStack.GetItemHeld().GetType().GetRawID());
	writer.AddElement(itemstack);

	QXML::Element rigidbody(c_RigidbodyTag);
	rigidbody.SetAsRaw();
	std::vector<char> rigidbodyData;
	RigidbodySerializer rigidbodySerializer;
	rigidbodySerializer.Serialize(droppedItem.m_Rigidbody, rigidbodyData);
	rigidbody.AddData(rigidbodyData);
	writer.AddElement(rigidbody);

	data = writer.GetResult();
}

std::unique_ptr<DroppedItem> DroppedItemSerializer::Deserialize(const std::vector<char>& data, BlockManager& world) {
	DEBUG_LOG("Deserializing item");

	QXML::QXMLReader reader(data);
	auto* itemstackElement = reader.GetBase().GetElementsByTag(c_ItemStackTag),
		* rigidbodyElement = reader.GetBase().GetElementsByTag(c_RigidbodyTag);

	if (itemstackElement == nullptr || itemstackElement->size() != 1 ||
		rigidbodyElement == nullptr || rigidbodyElement->size() != 1)
		return nullptr;

	std::unique_ptr<Item> itemHeld = Item::GetByType(ItemType((*itemstackElement)[0].GetDataAsString()));
	if (itemHeld == nullptr)
		return nullptr;

	ItemStack stack(*itemHeld, (*itemstackElement)[0].GetAttributeValue(c_CountAttribute).m_Value);
	RigidbodySerializer rigidbodySerializer;
	std::unique_ptr<Rigidbody> rigidbody = rigidbodySerializer.Deserialize((*rigidbodyElement)[0].GetData(), world);
	if (rigidbody == nullptr)
		return nullptr;

	return std::make_unique<DroppedItem>(std::move(stack), *rigidbody);
}