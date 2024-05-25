#include <game/ui/ItemQuad.hpp>
#include <game/graphics/ItemTextureProvider.hpp>

ItemQuad::ItemQuad() :
	m_Rectangle(glm::ivec2(0), c_Size),
	m_Texture(nullptr),
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
	m_Texture(nullptr),
	m_Stack(nullptr),
	m_Font(c_FontFile, c_FontSize),
	m_CountText(*m_Font.Get(), c_CountTextPosition, "0", c_CountTextColor)
{
	SetItem(stack);
}

ItemQuad::ItemQuad(ItemQuad&& other) noexcept :
	m_Stack(std::move(other.m_Stack)),
	m_Rectangle(std::move(other.m_Rectangle)),
	m_Texture(other.m_Texture),
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

void ItemQuad::update() {
	if (m_Stack == nullptr || m_Stack->Empty())
		return;

	const Item& itemHeld = m_Stack->GetItemHeld();

	m_Texture = &itemHeld.GetTexture();
	m_Rectangle.SetTextureCoords(m_Texture->GetUVPosition(),
		m_Texture->GetUVSize());
	
	m_CountText.SetText(std::to_string(m_Stack->GetCount()));
}