#include <ui/InventoryUI.hpp>

ItemPicture::ItemPicture(WidgetParent& parent, glm::ivec2 position, ItemStack& itemStack) :
	Widget(parent, position, ItemQuad::c_Size),
	m_Stack(itemStack),
	m_Background({ 0, 0 }, ItemQuad::c_Size, c_DefaultBackgroundColor)
{
	m_Picture.SetItem(&itemStack);
}

void ItemPicture::SetHighlighted(bool highlighted) {
	m_Background.SetColor(highlighted ? c_HighlightedBackgroundColor : c_DefaultBackgroundColor);
	m_Background.SetSize(highlighted ? ItemQuad::c_Size + glm::ivec2(2 * c_HighlightedBackgroundSizeIncrease, 2 * c_HighlightedBackgroundSizeIncrease) : ItemQuad::c_Size);
	m_Background.SetPosition(highlighted ? glm::ivec2{-c_HighlightedBackgroundSizeIncrease, -c_HighlightedBackgroundSizeIncrease} : glm::ivec2{0, 0});
}

void ItemPicture::render(RenderingParams& params)
{
	params.SetColoringType(ColoringType::Color);
	m_Background.Render();

	m_Picture.Render(params);
}

PictureBox::PictureBox(WidgetParent& parent, std::string filename, glm::ivec2 position, glm::ivec2 size) :
	Widget(parent, position, size),
	m_Texture(filename, true),
	m_Rectangle(glm::ivec2(0), glm::ivec2(0), glm::vec2(0), glm::vec2(1, 1))
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
}

InventoryUI::InventoryUI(WidgetParent& parent, Inventory& inventory) :
	m_Inventory(inventory),
	WidgetGroup(parent, glm::ivec2(0), glm::ivec2(0)),
	m_Background(*this)
{
	SetAnchor(AnchorPoint::Center);
	SetRelativeTo(AnchorPoint::Center);
	SetVisible(false);
	SetSize(m_Background.GetSize());

	m_MouseHolder = std::make_unique <MouseHolder>(parent, glm::ivec2(0));

	prepareSlots();

	Update();
}

void InventoryUI::Update()
{
}

void InventoryUI::handleMouseEvent(const MouseEvent& event)
{
	m_MouseHolder->SetPosition(event.Position);
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

	glm::ivec2 currentPosition = firstSlot + glm::ivec2(0, (ItemQuad::c_Size.y + slotMargin.y) * rowsTillHotbar + hotbarMargin - slotMargin.y);
	for (int i = 0; i < Inventory::c_Size; ++i)
	{
		m_Slots[i] = std::make_unique<ItemSlot>(*this, currentPosition, *m_MouseHolder, m_Inventory.GetItemStack(i));

		currentPosition.x += slotMargin.x + ItemQuad::c_Size.x;
		if ((i + 1) % rowWidth == 0)
		{
			currentPosition.x = firstSlot.x;
			currentPosition.y -= ItemQuad::c_Size.y;
			if ((i + 1) / rowWidth == 1)
				currentPosition.y -= hotbarMargin;
			else
				currentPosition.y -= slotMargin.y;
		}
	}
}

Hotbar::Hotbar(WidgetParent& parent, Inventory& inventory, Window& window) :
	WidgetGroup(parent, -c_Position, glm::ivec2(0)),
	m_Inventory(inventory),
	m_Background(*this, c_TextureFilename, glm::ivec2(0)),
	m_Window(window)
{
	m_Window.AddKeyboardListener(*this);

	SetAnchor(AnchorPoint::CenterBottom);
	SetRelativeTo(AnchorPoint::CenterBottom);

	SetSize(m_Background.GetSize());

	prepareSlots();
	m_Slots[m_Inventory.GetSelectedItemIndex()]->SetHighlighted(true);
}

Hotbar::~Hotbar() {
	m_Window.RemoveKeyboardListener(*this);
}

void Hotbar::OnKeyboardEvent(const KeyboardEvent& event) {
	if (event.m_Type == KeyboardEvent::KeyPressed) {
		switch (event.m_Key.Code()) {
		case Keyboard::Key::Left:
			m_Slots[m_Inventory.GetSelectedItemIndex()]->SetHighlighted(false);
			m_Inventory.ChangeSelectedItem(-1);
			m_Slots[m_Inventory.GetSelectedItemIndex()]->SetHighlighted(true);
			break;
		case Keyboard::Key::Right:
			m_Slots[m_Inventory.GetSelectedItemIndex()]->SetHighlighted(false);
			m_Inventory.ChangeSelectedItem(1);
			m_Slots[m_Inventory.GetSelectedItemIndex()]->SetHighlighted(true);
			break;
		}
	}
}

void Hotbar::prepareSlots()
{
	static constexpr glm::ivec2 firstSlot = { 17, 18 };
	static const int slotMargin = 8;

	glm::ivec2 currentPosition = firstSlot;
	for (int i = 0; i < c_ItemCount; ++i)
	{
		m_Slots[i] = std::make_unique<ItemPicture>(*this, currentPosition, m_Inventory.GetItemStack(i));
		currentPosition.x += slotMargin + ItemQuad::c_Size.x;
	}
}