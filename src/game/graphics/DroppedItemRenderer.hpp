#pragma once

#include <engine/core/Renderer.hpp>
#include <game/entity/DroppedItem.hpp>
#include <game/ui/ItemQuad.hpp>

class DroppedItemRenderable : Renderable {
public:
	DroppedItemRenderable(const DroppedItem& item, Renderer3D& renderer);
	~DroppedItemRenderable();
	
	const DroppedItem* GetDroppedItem() const { return &m_Item; }

	glm::mat4 GetModelMatrix() const override;
	glm::vec3 GetRenderPosition() const override { return m_Item.GetPosition(); }
	void Render(const RenderingContext& context) override;
private:
	const DroppedItem& m_Item;
	Renderer3D& m_Renderer;

	std::unique_ptr<game::graphics::DroppedItemMesh> m_Mesh;
};

class DroppedItemRenderer : public DroppedItemRepositoryListener {
public:
	DroppedItemRenderer(Renderer3D& renderer, DroppedItemRepository& repository);

	//DroppedItemRepositoryListener
	void LoadedDroppedItem(DroppedItem& item) override;
	void UnloadedDroppedItem(DroppedItem& item) override;
	void CreatedDroppedItem(DroppedItem& item) override;
	void DestroyedDroppedItem(DroppedItem& item) override;
private:
	Renderer3D& m_Renderer;
	DroppedItemRepository& m_Repository;
	std::vector<std::unique_ptr<DroppedItemRenderable>> m_Meshes;

	void createMesh(DroppedItem& item);
	void removeMesh(DroppedItem& item);
};