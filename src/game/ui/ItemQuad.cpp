#include <game/ui/ItemQuad.hpp>

ItemQuad::ItemQuad() :
	m_Rectangle(glm::ivec2(0), c_Size),
	m_Texture(c_TextureFilepath),
	m_Stack(nullptr),
	m_Font(c_FontFile, c_FontSize),
	m_CountText(*m_Font.Get(), c_CountTextPosition, "0", c_CountTextColor)
{
}

ItemQuad::~ItemQuad() {
	if (m_Stack != nullptr)
		m_Stack->RemoveListener(this);
}

ItemQuad::ItemQuad(ItemStack* stack) :
	m_Rectangle(glm::ivec2(0), c_Size),
	m_Texture(c_TextureFilepath),
	m_Stack(nullptr),
	m_Font(c_FontFile, c_FontSize),
	m_CountText(*m_Font.Get(), c_CountTextPosition, "0", c_CountTextColor)
{
	SetItem(stack);
}

ItemQuad::ItemQuad(ItemQuad&& other) noexcept :
	m_Stack(std::move(other.m_Stack)),
	m_Rectangle(std::move(other.m_Rectangle)),
	m_Texture(std::move(other.m_Texture)),
	m_Font(std::move(m_Font)),
	m_CountText(std::move(m_CountText))
{

}

ItemQuad& ItemQuad::operator=(ItemQuad&& other) noexcept
{
	if (this == &other)
		return *this;

	m_Stack = std::move(other.m_Stack);
	m_Rectangle = std::move(other.m_Rectangle);
	m_Texture = std::move(other.m_Texture);

	return *this;
}

void ItemQuad::SetItem(ItemStack* stack)
{
	if (stack == m_Stack)
		return;

	if (m_Stack != nullptr)
		m_Stack->RemoveListener(this);
	if (stack != nullptr)
		stack->AddListener(this);

	m_Stack = stack;
	update();
}

void ItemQuad::StackUpdated() {
	update();
}

void ItemQuad::Render(RenderingParams& params) const
{
	if (m_Stack != nullptr && m_Stack->Empty())
		return;

	params.SetColoringType(ColoringType::ColorTexture);

	m_Texture->Bind(0);
	m_Rectangle.Render();

	params.SetColoringType(ColoringType::AlphaTexture);
	m_CountText.Render(params);
}

glm::vec2 ItemQuad::GetItemTextureSize() {
	constexpr glm::ivec2 atlasTextureSize = { 8, 8 };
	constexpr glm::vec2 itemTextureSize = 1.f / (glm::vec2)atlasTextureSize;
	return  itemTextureSize;
}

glm::vec2 ItemQuad::GetItemTexturePosition(const Item& item) {
	constexpr glm::ivec2 atlasTextureSize = { 8, 8 };
	constexpr glm::vec2 itemTextureSize = 1.f / (glm::vec2)atlasTextureSize;
	ItemTextureData data = item.GetTextureData();
	return (glm::vec2)data.Position * itemTextureSize;
}

void ItemQuad::update() {
	if (m_Stack == nullptr || m_Stack->Empty())
		return;

	const Item& itemHeld = m_Stack->GetItemHeld();

	m_Rectangle.SetTextureCoords(GetItemTexturePosition(itemHeld),
		GetItemTextureSize());
	
	m_CountText.SetText(std::to_string(m_Stack->GetCount()));
}