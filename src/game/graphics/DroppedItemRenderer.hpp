#pragma once

#include <engine/core/Renderer.hpp>
#include <game/entity/DroppedItem.hpp>
#include <game/ui/ItemQuad.hpp>

class DroppedItemMesh : Renderable {
public:
	DroppedItemMesh(const DroppedItem& item, Renderer3D& renderer);
	~DroppedItemMesh();
	
	const DroppedItem* GetDroppedItem() const { return &m_Item; }

	glm::mat4 GetModelMatrix() const override;
	void Render(const RenderingContext& context) override;
private:
	VertexArray m_VAO;
	VertexBuffer<Vertex3D>& m_VBO;
	ElementBuffer& m_EBO;

	const float c_Size = 0.7f;
	const std::string c_ShaderPath = "items.shader";

	static constexpr const char* c_TextureFilepath = "items.td";
	Resource<AtlasTexture> m_Texture;

	const DroppedItem& m_Item;
	Resource<ShaderProgram> m_Shader;
	UniformLocation m_CameraUpLocation,
		m_CameraRightLocation;

	Renderer3D& m_Renderer;

	void setupMesh();
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
	std::vector<std::unique_ptr<DroppedItemMesh>> m_Meshes;

	void createMesh(DroppedItem& item);
	void removeMesh(DroppedItem& item);
};