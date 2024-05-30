#pragma once

#include <game/graphics/ItemTextureProvider.hpp>
#include <game/graphics/BlockTextureProvider.hpp>
#include <engine/core/Resource.hpp>
#include <engine/graphics/Framebuffer.hpp>
#include <engine/graphics/AtlasTexture.hpp>
#include <map>
#include <engine/graphics/VertexArray.hpp>
#include <engine/graphics/Vertex.hpp>
#include <engine/graphics/ShaderProgram.hpp>

namespace game {
	namespace graphics {
		class ConcreteItemTextureLoader : public ItemTextureLoader {
		public:
			ConcreteItemTextureLoader(BlockTextureLoader& blockTextureLoader);

			const Texture& GetTextureForBlock(Block::BlockType type) override;
			const Texture& GetTextureForFlat(glm::ivec2 position) override;

		private:
			class BlockItemTextureLoader {
			public:
				BlockItemTextureLoader(BlockTextureLoader& blockTextureLoader);

				const Texture& GetTexture(Block::BlockType type);

			private:
				class BlockItemTexture : public Texture {
				public:
					BlockItemTexture(GLuint handle, glm::vec2 size, glm::vec2 position);

					virtual void Bind(GLuint textureUnit) const override;
					virtual TextureHandle GetHandle() const override { return m_Handle; };
					virtual glm::vec2 GetUVSize() const override { return m_Size; };
					virtual glm::vec2 GetUVPosition() const override { return m_Position; };

				private:
					GLuint m_Handle; 
					glm::vec2 m_Size, 
						m_Position;
				};

				class ItemCube {
				public:
					ItemCube();

					void Render();

				private:
					VertexArray m_VAO;
					VertexBuffer<Vertex3D>& m_VBO;
					ElementBuffer& m_EBO;

					void setupBuffers();
				};

				BlockTextureLoader& m_BlockTextureLoader;

				std::map<Block::BlockType, BlockItemTexture> m_BlockItemTextures;
				eng::graphics::Framebuffer m_Framebuffer;
				const std::string c_LoadingShaderPath = "block items.shader";
				Resource<ShaderProgram> m_BlockItemLoadingShader;
				UniformLocation m_TexturePositionLoc;
				UniformLocation m_TextureSizeLoc;
				UniformLocation m_TextureLoc;
				int m_CurrentTextureIndex = 0;

				ItemCube m_Cube;

				const static int c_BlockItemTextureResolution = 64;

				void loadBlockItemTexture(Block::BlockType type);
				bool isBlockItemTextureLoaded(Block::BlockType type);
				static glm::ivec2 getFramebufferSize();
			};

			const glm::ivec2 c_TextureSizeInTiles = { 8, 8 };
			const std::string c_FlatItemTextureFilepath = "items.td";
			Resource<AtlasTexture> m_FlatItemTexture;

			BlockItemTextureLoader m_BlockItemTextureLoader;
		};
	} //end graphics
} //end game