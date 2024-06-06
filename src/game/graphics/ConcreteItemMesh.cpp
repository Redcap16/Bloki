#include <game/graphics/ConcreteItemMesh.hpp>

namespace game {
	namespace graphics {
		FlatItemMesh::FlatItemMesh(const Texture& texture) :
			m_VAO(false),
			m_EBO(m_VAO.GetElementBuffer()),
			m_VBO(m_VAO.CreateVertexBuffer<Vertex3D>(false)),
			m_Shader(c_ShaderPath),
			m_Texture(texture) {
			setupMesh();

			m_Shader->UseProgram();
			m_CameraUpLocation = m_Shader->GetUniformLocation("cameraUp");
			m_CameraRightLocation = m_Shader->GetUniformLocation("cameraRight");
			m_TexturePositionLoc = m_Shader->GetUniformLocation("TexturePosition");
			m_TextureSizeLoc = m_Shader->GetUniformLocation("TextureSize");

			m_Shader->SetUniform(m_Shader->GetUniformLocation("textureAtlas"), 0);
		}

		void FlatItemMesh::Render(const RenderingContext& context) {
			m_Shader->SetUniform(m_CameraUpLocation, context.CurrentCamera3D->GetUpVector());
			m_Shader->SetUniform(m_CameraRightLocation, context.CurrentCamera3D->GetRightVector());
			m_Shader->SetUniform(m_TexturePositionLoc, m_Texture.GetUVPosition());
			m_Shader->SetUniform(m_TextureSizeLoc, m_Texture.GetUVSize());

			m_Texture.Bind(0);
			m_VAO.Draw();
		}

		void FlatItemMesh::setupMesh() {
			m_VBO.AddVertex({ c_Size / 2 * glm::vec3{-1, -1, 0},
				{0.f, 1.},
				{0, 0, 0} });
			m_VBO.AddVertex({ c_Size / 2 * glm::vec3{1, -1, 0},
				{1.f, 1.f},
				{0, 0, 0} });
			m_VBO.AddVertex({ c_Size / 2 * glm::vec3{1, 1, 0},
				{1.f, 0.f},
				{0, 0, 0} });
			m_VBO.AddVertex({ c_Size / 2 * glm::vec3{-1, 1, 0},
				{0.f, 0.f},
				{0, 0, 0} });

			m_EBO.AddIndices({ 0, 1, 2 });
			m_EBO.AddIndices({ 0, 2, 3 });

			m_VBO.UpdateBuffer();
			m_EBO.UpdateBuffer();
			m_EBO.ClearData();
			m_VBO.ClearData();
		}

		BlockItemMesh::BlockItemMesh(const Texture& texture) :
			m_VAO(false),
			m_EBO(m_VAO.GetElementBuffer()),
			m_VBO(m_VAO.CreateVertexBuffer<Vertex3D>(false)),
			m_Shader(c_ShaderPath),
			m_Texture(texture),
			m_CreationTime(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) {
			setupMesh();

			m_Shader->UseProgram();
			m_RotationLoc = m_Shader->GetUniformLocation("rot");
			m_TexturePositionLoc = m_Shader->GetUniformLocation("TexturePosition");
			m_TextureSizeLoc = m_Shader->GetUniformLocation("TextureSize");
			m_Shader->SetUniform(m_Shader->GetUniformLocation("textureAtlas"), 0);
		}

		void BlockItemMesh::Render(const RenderingContext& context) {
			m_Shader->SetUniform(m_TexturePositionLoc, m_Texture.GetUVPosition());
			m_Shader->SetUniform(m_TextureSizeLoc, m_Texture.GetUVSize());
			long long time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - m_CreationTime;
			float angle = time % 3000 / 3000.0f * 2 * M_PI;
			m_Shader->SetUniform(m_RotationLoc, glm::vec2(cos(angle), sin(angle)));

			m_VAO.Draw();
		}

		void BlockItemMesh::setupMesh() {
			std::vector<Vertex3D> vertices = { 
				{ c_Size / 2 * glm::vec3{-1, -1, -1}, {0.75f, 0.667f}, {0, 0, -1} }, //Front face Z-
				{ c_Size / 2 * glm::vec3{1, -1, -1}, {0.75f, 0.333f}, {0, 0, -1} },
				{ c_Size / 2 * glm::vec3{1, 1, -1}, {0.5f, 0.333f}, {0, 0, -1} },
				{ c_Size / 2 * glm::vec3{-1, 1, -1}, {0.5f, 0.667f}, {0, 0, -1} },
				{ c_Size / 2 * glm::vec3{-1, -1, 1}, {0.0f, 0.667f}, {0, 0, 1} }, //Back face Z+
				{ c_Size / 2 * glm::vec3{-1, 1, 1}, {0.25f, 0.667f}, {0, 0, 1} },
				{ c_Size / 2 * glm::vec3{1, 1, 1}, {0.25f, 0.333f}, {0, 0, 1} },
				{ c_Size / 2 * glm::vec3{1, -1, 1}, {0.0f, 0.333f}, {0, 0, 1} },
				{ c_Size / 2 * glm::vec3{1, -1, -1}, {0.5f, 0.0f}, {1, 0, 0} }, //Right face X+
				{ c_Size / 2 * glm::vec3{1, -1, 1}, {0.25f, 0.0f}, {1, 0, 0} },
				{ c_Size / 2 * glm::vec3{1, 1, 1}, {0.25f, 0.333f}, {1, 0, 0} },
				{ c_Size / 2 * glm::vec3{1, 1, -1}, {0.5f, 0.333f}, {1, 0, 0} },
				{ c_Size / 2 * glm::vec3{-1, -1, 1}, {0.25f, 1.0f}, {-1, 0, 0} }, //Left face X-
				{ c_Size / 2 * glm::vec3{-1, 1, 1}, {0.25f, 0.667f}, {-1, 0, 0} },
				{ c_Size / 2 * glm::vec3{-1, 1, -1}, {0.5f, 0.667f}, {-1, 0, 0} },
				{ c_Size / 2 * glm::vec3{-1, -1, -1}, {0.5f, 1.0f}, {-1, 0, 0} }, 
				{ c_Size / 2 * glm::vec3{-1, 1, -1}, {0.5f, 0.667f}, {0, 1, 0} }, //Top face Y+
				{ c_Size / 2 * glm::vec3{1, 1, -1}, {0.5f, 0.333f}, {0, 1, 0} },
				{ c_Size / 2 * glm::vec3{1, 1, 1}, {0.25f, 0.333f}, {0, 1, 0} },
				{ c_Size / 2 * glm::vec3{-1, 1, 1}, {0.25f, 0.667f}, {0, 1, 0} },
				{ c_Size / 2 * glm::vec3{-1, -1, -1}, {0.75f, 0.667f}, {0, -1, 0} }, //Bottom face Y-
				{ c_Size / 2 * glm::vec3{-1, -1, 1}, {1.0f, 0.667f}, {0, -1, 0} },
				{ c_Size / 2 * glm::vec3{1, -1, 1}, {1.0f, 0.333f}, {0, -1, 0} },
				{ c_Size / 2 * glm::vec3{1, -1, -1}, {0.75f, 0.333f}, {0, -1, 0} } };
			m_VBO.AddVertices(vertices);

			m_EBO.AddIndices({ 0, 1, 2,
				0, 2, 3,
				4, 5, 6,
				4, 6, 7,
				8, 9, 10,
				8, 10, 11,
				12, 13, 14,
				12, 14, 15,
				16, 17, 18,
				16, 18, 19,
				20, 21, 22,
				20, 22, 23 });

			m_VBO.UpdateBuffer();
			m_EBO.UpdateBuffer();
			m_EBO.ClearData();
			m_VBO.ClearData();
		}
	}
}