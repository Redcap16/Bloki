#include <ui/InventoryUI.hpp>

std::array<AtlasTexture::SubTexture, Item::c_ItemCount> ItemQuad::m_TextureCoords;

ItemQuad::ItemQuad() :
	m_Rectangle(glm::ivec2(0), c_Size),
	m_Texture(c_TextureFilepath)
{
	loadTextureCoords();

	m_TextureCoords[Item::Bread].UV = glm::vec2(0, 0);
	m_TextureCoords[Item::Bread].Size = glm::vec2(0.125, 0.125);
}

ItemQuad::ItemQuad(ItemQuad&& other) noexcept :
	m_Stack(other.m_Stack),
	m_Rectangle(std::move(other.m_Rectangle)),
	m_Texture(std::move(other.m_Texture))
{

}

ItemQuad& ItemQuad::operator=(ItemQuad&& other) noexcept
{
	if (this == &other)
		return *this;

	m_Stack = other.m_Stack;
	m_Rectangle = std::move(other.m_Rectangle);
	m_Texture = std::move(other.m_Texture);

	return *this;
}

void ItemQuad::SetItem(const ItemStack& stack)
{
	if (stack == m_Stack)
		return;

	m_Stack = stack;
	update();
}

void ItemQuad::Render(ShaderProgram& shader) const
{
	UniformLocation hasTexture = shader.GetUniformLocation("HasTexture"),
		texture = shader.GetUniformLocation("Texture");
	shader.SetUniform(hasTexture, 1);
	shader.SetUniform(texture, 0);

	m_Texture->Bind(0);
	m_Rectangle.Render();
}

void ItemQuad::update()
{
	if (m_Stack.ItemHeld.IsBlock)
	{
		//TODO: Add rendered block image
	}
	else
		m_Rectangle.SetTextureCoords(m_TextureCoords[m_Stack.ItemHeld.ItemID].UV, m_TextureCoords[m_Stack.ItemHeld.ItemID].Size);
}

void ItemQuad::loadTextureCoords()
{
	//TODO: Load textureData from atlas texture
}

MouseHolder::MouseHolder(WidgetParent& parent, glm::ivec2 position) :
	Widget(parent, position, ItemQuad::c_Size)
{
	SetAnchor(AnchorPoint::Center);
}

void MouseHolder::SetStack(const ItemStack& stack)
{
	if (m_Stack == stack)
		return;
	
	m_Stack = stack;
	if (m_Stack.Count != 0)
		m_Mesh.SetItem(m_Stack);
}

void MouseHolder::handleMouseEvent(const MouseEvent& event)
{

}

void MouseHolder::render(WidgetRenderParams& params)
{
	if (!m_Stack.Count)
		return;

	m_Mesh.Render(params.m_Shader);
}

ItemSlot::ItemSlot(WidgetParent& parent, glm::ivec2 position, MouseHolder& mouseHolder, Inventory& inventory, int index) :
	Widget(parent, position, ItemQuad::c_Size),
	m_MouseHolder(&mouseHolder),
	m_Inventory(&inventory),
	m_Index(index),
	m_Stack(m_Inventory->GetItemStack(index))
{
	if (m_Stack.Count != 0)
		m_Mesh.SetItem(m_Stack);
}

void ItemSlot::Update(const ItemStack& stack)
{
	if (m_Stack == stack)
		return;

	m_Stack = stack;
	if (m_Stack.Count != 0)
		m_Mesh.SetItem(stack);
}

void ItemSlot::handleMouseEvent(const MouseEvent& event)
{
	assert(m_MouseHolder != nullptr);

	if (event.Type == MouseEvent::MouseRelease &&
		m_OnHover)
	{
		ItemStack item = m_MouseHolder->GetStack();
		if (item.Count && m_Stack.Count &&
			item.ItemHeld.StackableWith(m_Stack.ItemHeld))
		{
			int transferCount = std::min(m_Stack.ItemHeld.MaxStackCount() - m_Stack.Count, item.Count);
			m_Stack.Count += transferCount;
			item.Count -= transferCount;

			m_MouseHolder->SetStack(item);
		}
		else
		{
			m_MouseHolder->SetStack(m_Stack);
			Update(item);
		}

		m_Inventory->SetItemStack(m_Index, m_Stack); //FIXME: It invokes ItemSlot::Update at the end
	}
}

void ItemSlot::render(WidgetRenderParams& params)
{
	if (!m_Stack.Count)
		return;

	m_Mesh.Render(params.m_Shader);
}

InventoryUI::InventoryUI(WidgetParent& parent, Inventory& inventory) :
	m_Inventory(inventory),
	WidgetGroup(parent, glm::ivec2(0), glm::ivec2(0)),
	m_Background(*this)
{
	m_Inventory.AddListener(this);

	SetAnchor(AnchorPoint::Center);
	SetRelativeTo(AnchorPoint::Center);
	SetVisible(false);
	SetSize(m_Background.GetSize());

	m_MouseHolder = std::make_unique <MouseHolder>(*this, glm::ivec2(0));

	prepareSlots();

	Update();
}

void InventoryUI::Update()
{
}

void InventoryUI::Updated(int index)
{
	m_Slots[index].Update(m_Inventory.GetItemStack(index));
}

void InventoryUI::handleMouseEvent(const MouseEvent& event)
{
	m_MouseHolder->SetPosition(event.Position - m_FinalPosition);
	WidgetGroup::handleMouseEvent(event);
}

InventoryUI::InventoryBackground::InventoryBackground(InventoryUI& inventory) :
	Widget(inventory, glm::ivec2(0), glm::ivec2(0)),
	m_Texture(c_TextureFilename, true),
	m_Rectangle(glm::ivec2(0), glm::ivec2(0), glm::vec2(0), glm::vec2(1, 1))
{
	SetSize(m_Texture->GetSize());
	m_Rectangle.SetSize(m_Texture->GetSize());
}

void InventoryUI::InventoryBackground::render(WidgetRenderParams& params)
{
	UniformLocation hasTexture = params.m_Shader.GetUniformLocation("HasTexture"),
		texture = params.m_Shader.GetUniformLocation("Texture");
	params.m_Shader.SetUniform(hasTexture, 1);
	params.m_Shader.SetUniform(texture, 0);

	m_Texture->Bind(0);
	m_Rectangle.Render();
}

void InventoryUI::prepareSlots()
{
	static constexpr glm::ivec2 slotMargin = { 8, 8 };
	static constexpr glm::ivec2 firstSlot = { 18, 18 };
	static const int rowWidth = 9;
	static const int rowsTillHotbar = 3;
	static const int hotbarMargin = 29;

	glm::ivec2 currentPosition = firstSlot;
	for (int i = 0; i < Inventory::c_Size; ++i)
	{
		m_Slots.emplace_back(*this, currentPosition, *m_MouseHolder, m_Inventory, i);

		currentPosition.x += slotMargin.x + ItemQuad::c_Size.x;
		if ((i + 1) % rowWidth == 0)
		{
			currentPosition.x = firstSlot.x;
			currentPosition.y += ItemQuad::c_Size.y;
			if ((i + 1) / rowWidth == rowsTillHotbar)
				currentPosition.y += hotbarMargin;
			else
				currentPosition.y += slotMargin.y;
		}
	}
}