#include <game/graphics/DroppedItemRenderer.hpp>
#include <game/graphics/ItemTextureProvider.hpp>

DroppedItemRenderable::DroppedItemRenderable(const DroppedItem& item, Renderer3D& renderer) :
	m_Item(item),
	m_Renderer(renderer),
	m_Mesh(m_Item.GetItemStack().GetItemHeld().GetMesh()) {

	m_Renderer.RegisterRenderable(this, { true, &m_Mesh->GetShader(), &m_Mesh->GetTexture()});
}

DroppedItemRenderable::~DroppedItemRenderable() {
	m_Renderer.RemoveRenderable(this);
}

glm::mat4 DroppedItemRenderable::GetModelMatrix() const {
	return glm::translate(glm::mat4(1.f), m_Item.GetPosition());
}

void DroppedItemRenderable::Render(const RenderingContext& context) {
	m_Mesh->Render(context);
}

DroppedItemRenderer::DroppedItemRenderer(Renderer3D& renderer, DroppedItemRepository& repository) :
	m_Renderer(renderer),
	m_Repository(repository) {
	m_Repository.AddListener(*this);
}

void DroppedItemRenderer::LoadedDroppedItem(DroppedItem& item) {
	createMesh(item);
}

void DroppedItemRenderer::UnloadedDroppedItem(DroppedItem& item) {
	removeMesh(item);
}

void DroppedItemRenderer::CreatedDroppedItem(DroppedItem& item) {
	createMesh(item);
}

void DroppedItemRenderer::DestroyedDroppedItem(DroppedItem& item) {
	removeMesh(item);
}

void DroppedItemRenderer::createMesh(DroppedItem& item) {
	m_Meshes.push_back(std::make_unique<DroppedItemRenderable>(item, m_Renderer));
}

void DroppedItemRenderer::removeMesh(DroppedItem& item) {
	for (auto it = m_Meshes.begin(); it != m_Meshes.end(); ++it)
		if ((*it)->GetDroppedItem() == &item) {
			m_Meshes.erase(it);
			return;
		}
}