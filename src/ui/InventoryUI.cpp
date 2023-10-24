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

void ItemQuad::Render(RenderingParams& params) const
{
	if (!m_Stack.Count)
		return;

	params.SetColoringType(ColoringType::ColorTexture);

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
	m_Mesh.SetItem(m_Stack);
}

void MouseHolder::handleMouseEvent(const MouseEvent& event)
{

}

void MouseHolder::render(RenderingParams& params)
{
	if (!m_Stack.Count)
		return;

	m_Mesh.Render(params);
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

void ItemSlot::render(RenderingParams& params)
{
	m_Mesh.Render(params);
}

ItemPicture::ItemPicture(WidgetParent& parent, glm::ivec2 position) :
	Widget(parent, position, ItemQuad::c_Size)
{
	
}

void ItemPicture::SetItem(const ItemStack& stack)
{
	m_Mesh.SetItem(stack);
}

void ItemPicture::render(RenderingParams& params)
{
	m_Mesh.Render(params);
}

PictureBox::PictureBox(WidgetParent& parent, std::string filename, glm::ivec2 position, glm::ivec2 size) :
	Widget(parent, position, size),
	m_Texture(filename, true),
	m_Rectangle(glm::ivec2(0), glm::ivec2(0), glm::vec2(0), glm::vec2(1, 1)),
	m_Font("DejaVuSerif.ttf", 50),
	m_Test(m_Font, { -30, -30 }, "Hello, my name is FILIP mm m  m,,,,", glm::ivec3(255, 0, 0))
{
	if (size == glm::ivec2(0, 0))
		SetSize(m_Texture->GetSize());
	m_Rectangle.SetSize(m_Texture->GetSize());
}

void PictureBox::render(RenderingParams& params)
{
	params.SetColoringType(ColoringType::ColorTexture);

	m_Texture->Bind(0);
	m_Rectangle.Render();

	params.SetColoringType(ColoringType::AlphaTexture);
	m_Test.Render(params);
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

void InventoryUI::InventoryBackground::render(RenderingParams& params)
{
	params.SetColoringType(ColoringType::ColorTexture);

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

Hotbar::Hotbar(WidgetParent& parent, Inventory& inventory) :
	WidgetGroup(parent, -c_Position, glm::ivec2(0)),
	m_Inventory(inventory),
	m_Background(*this, c_TextureFilename, glm::ivec2(0))
{
	m_Inventory.AddListener(this);

	SetAnchor(AnchorPoint::CenterBottom);
	SetRelativeTo(AnchorPoint::CenterBottom);

	SetSize(m_Background.GetSize());

	prepareSlots();
}

void Hotbar::Updated(int index)
{
	if (index >= c_ItemCount)
		return;

	m_Slots[index].SetItem(m_Inventory.GetItemStack(index));
}

void Hotbar::prepareSlots()
{
	static constexpr glm::ivec2 firstSlot = { 17, 18 };
	static const int slotMargin = 8;

	glm::ivec2 currentPosition = firstSlot;
	for (int i = 0; i < c_ItemCount; ++i)
	{
		m_Slots.emplace_back(*this, currentPosition);
		currentPosition.x += slotMargin + ItemQuad::c_Size.x;
	}
}