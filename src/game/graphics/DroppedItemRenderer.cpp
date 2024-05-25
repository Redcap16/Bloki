#include <game/graphics/DroppedItemRenderer.hpp>
#include <game/graphics/ItemTextureProvider.hpp>

DroppedItemMesh::DroppedItemMesh(const DroppedItem& item, Renderer3D& renderer) :
	m_Item(item),
	m_VAO(false),
	m_EBO(m_VAO.GetElementBuffer()),
	m_VBO(m_VAO.CreateVertexBuffer<Vertex3D>(false)),
	m_Shader(c_ShaderPath),
	m_Texture(c_TextureFilepath),
	m_Renderer(renderer) {
	setupMesh();
	
	m_Shader->UseProgram();
	m_CameraUpLocation = m_Shader->GetUniformLocation("cameraUp");
	m_CameraRightLocation = m_Shader->GetUniformLocation("cameraRight");

	m_Renderer.RegisterRenderable(this, { true, m_Shader.Get(), m_Texture.Get()});
}

DroppedItemMesh::~DroppedItemMesh() {
	m_Renderer.RemoveRenderable(this);
}

glm::mat4 DroppedItemMesh::GetModelMatrix() const {
	return glm::translate(glm::mat4(1.f), m_Item.GetPosition());
}

void DroppedItemMesh::Render(const RenderingContext& context) {
	m_Shader->SetUniform(m_CameraUpLocation, context.CurrentCamera3D->GetUpVector());
	m_Shader->SetUniform(m_CameraRightLocation, context.CurrentCamera3D->GetRightVector());

	m_VAO.Draw();
}

void DroppedItemMesh::setupMesh() {
	auto& texture = m_Item.GetItemStack().GetItemHeld().GetTexture();
	const glm::vec2 texPos = texture.GetUVPosition(),
		texSize = texture.GetUVSize();
	m_VBO.AddVertex({ c_Size / 2 * glm::vec3{-1, -1, 0},
		texPos + texSize * glm::vec2(0.f, 1.f), 
		{0, 0, 0} });
	m_VBO.AddVertex({ c_Size / 2 * glm::vec3{1, -1, 0},
		texPos + texSize * glm::vec2(1.f, 1.f),
		{0, 0, 0} });
	m_VBO.AddVertex({ c_Size / 2 * glm::vec3{1, 1, 0},
		texPos + texSize * glm::vec2(1.f, 0.f),
		{0, 0, 0} });
	m_VBO.AddVertex({ c_Size / 2 * glm::vec3{-1, 1, 0},
		texPos + texSize * glm::vec2(0.f, 0.f),
		{0, 0, 0} });

	m_EBO.AddIndices({ 0, 1, 2 });
	m_EBO.AddIndices({ 0, 2, 3 });

	m_VBO.UpdateBuffer();
	m_EBO.UpdateBuffer();
	m_EBO.ClearData();
	m_VBO.ClearData();
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
	m_Meshes.push_back(std::make_unique<DroppedItemMesh>(item, m_Renderer));
}

void DroppedItemRenderer::removeMesh(DroppedItem& item) {
	for (auto it = m_Meshes.begin(); it != m_Meshes.end(); ++it)
		if ((*it)->GetDroppedItem() == &item) {
			m_Meshes.erase(it);
			return;
		}
}