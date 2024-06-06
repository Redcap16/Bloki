#pragma once

#include <game/graphics/DroppedItemMesh.hpp>
#include <engine/graphics/Texture.hpp>
#include <engine/graphics/VertexArray.hpp>
#include <engine/graphics/ShaderProgram.hpp>
#include <game/entity/DroppedItem.hpp>

namespace game {
	namespace graphics {
		class FlatItemMesh : public DroppedItemMesh {
		public:
			FlatItemMesh(const Texture& texture);

			void Render(const RenderingContext& context) override;
			const Texture& GetTexture() const override { return m_Texture; };
			ShaderProgram& GetShader() override { return *m_Shader.Get(); };
			
		private:
			const float c_Size = 0.5f;
			const std::string c_ShaderPath = "flat-dropped-items.shader";

			VertexArray m_VAO;
			VertexBuffer<Vertex3D>& m_VBO;
			ElementBuffer& m_EBO;
			
			const Texture& m_Texture;

			Resource<ShaderProgram> m_Shader;
			UniformLocation m_CameraUpLocation,
				m_CameraRightLocation,
				m_TexturePositionLoc,
				m_TextureSizeLoc;

			void setupMesh();
		};

		class BlockItemMesh : public DroppedItemMesh {
		public:
			BlockItemMesh(const Texture& texture);

			void Render(const RenderingContext& context) override;
			const Texture& GetTexture() const override { return m_Texture; };
			ShaderProgram& GetShader() override { return *m_Shader.Get(); };

		private:
			const float c_Size = 0.3f;
			const std::string c_ShaderPath = "block-dropped-items.shader";

			VertexArray m_VAO;
			VertexBuffer<Vertex3D>& m_VBO;
			ElementBuffer& m_EBO;

			const long long m_CreationTime;
			const Texture& m_Texture;

			Resource<ShaderProgram> m_Shader;
			UniformLocation m_RotationLoc,
				m_TexturePositionLoc,
				m_TextureSizeLoc;

			void setupMesh();
		};
	}
}