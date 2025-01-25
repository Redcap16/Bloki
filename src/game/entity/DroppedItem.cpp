#include <game/entity/DroppedItem.hpp>
#include <game/world/Chunk.hpp>

const std::string DroppedItem::c_RigidbodyTag = "rigidbody",
	DroppedItem::c_ItemStackTag = "itemstack",
	DroppedItem::c_CountAttribute = "count";

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
	DebugProvider::Get().Log("Serializing item");
	QXML::QXMLWriter writer;

	QXML::Element itemstack(c_ItemStackTag);
	itemstack.AddAttribute(QXML::Attribute(c_CountAttribute, m_ItemStack.GetCount()));
	itemstack.AddData(m_ItemStack.GetItemHeld().GetType().GetRawID());
	writer.AddElement(itemstack);

	QXML::Element rigidbody(c_RigidbodyTag);
	rigidbody.SetAsRaw();
	std::vector<char> rigidbodyData;
	m_Rigidbody.Serialize(rigidbodyData);
	rigidbody.AddData(rigidbodyData);
	writer.AddElement(rigidbody);

	data = writer.GetResult();
}

std::unique_ptr<DroppedItem> DroppedItem::Deserialize(const std::vector<char>& data, BlockManager& world) {
	DebugProvider::Get().Log("Deserializing item");
	
	QXML::QXMLReader reader(data);
	auto* itemstackElement = reader.GetBase().GetElementsByTag(c_ItemStackTag),
		*rigidbodyElement = reader.GetBase().GetElementsByTag(c_RigidbodyTag);
	
	if (itemstackElement == nullptr || itemstackElement->size() != 1 ||
		rigidbodyElement == nullptr || rigidbodyElement->size() != 1)
		return nullptr;

	std::unique_ptr<Item> itemHeld = Item::GetByType(ItemType((*itemstackElement)[0].GetDataAsString()));
	if (itemHeld == nullptr)
		return nullptr;

	ItemStack stack(*itemHeld, (*itemstackElement)[0].GetAttributeValue(c_CountAttribute).m_Value);
	std::unique_ptr<Rigidbody> rigidbody = Rigidbody::Deserialize((*rigidbodyElement)[0].GetData(), world);
	if (rigidbody == nullptr)
		return nullptr;

	return std::make_unique<DroppedItem>(std::move(stack), *rigidbody);
}


DroppedItem::DroppedItem(ItemStack&& stack, const Rigidbody& rigidbody) :
	m_ItemStack(std::move(stack)),
	m_Rigidbody(rigidbody) {

}