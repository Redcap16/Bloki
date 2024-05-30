#include <game/graphics/ConcreteItemTextureLoader.hpp>
#include <game/graphics/ChunkRenderer.hpp>

namespace game {
	namespace graphics {
		ConcreteItemTextureLoader::ConcreteItemTextureLoader(BlockTextureLoader& blockTextureLoader) :
			m_FlatItemTexture(c_FlatItemTextureFilepath),
			m_BlockItemTextureLoader(blockTextureLoader) {

		}

		const Texture& ConcreteItemTextureLoader::GetTextureForBlock(Block::BlockType type) {
			return m_BlockItemTextureLoader.GetTexture(type);
		}

		const Texture& ConcreteItemTextureLoader::GetTextureForFlat(glm::ivec2 position) {
			const glm::vec2 uvSize = 1.f / (glm::vec2)c_TextureSizeInTiles,
				uvPosition = (glm::vec2)position * uvSize;
			
			return *m_FlatItemTexture->GetSubTexture(uvPosition, uvSize);
		}

		ConcreteItemTextureLoader::BlockItemTextureLoader::BlockItemTextureLoader(BlockTextureLoader& blockTextureLoader) :
			m_BlockTextureLoader(blockTextureLoader),
			m_Framebuffer(false, c_BlockItemTextureResolution * getFramebufferSize(), 4),
			m_BlockItemLoadingShader(c_LoadingShaderPath) {

			m_Framebuffer.Bind();
			glClear(GL_COLOR_BUFFER_BIT);
			m_Framebuffer.Unbind();

			m_TexturePositionLoc = m_BlockItemLoadingShader->GetUniformLocation("TexturePosition");
			m_TextureSizeLoc = m_BlockItemLoadingShader->GetUniformLocation("TextureSize");
			m_TextureLoc = m_BlockItemLoadingShader->GetUniformLocation("Texture");

			m_BlockItemLoadingShader->UseProgram();
			m_BlockItemLoadingShader->SetUniform(m_TextureLoc, 0);
		}

		const Texture& ConcreteItemTextureLoader::BlockItemTextureLoader::GetTexture(Block::BlockType type) {
			if (!isBlockItemTextureLoaded(type))
				loadBlockItemTexture(type);

			return m_BlockItemTextures.at(type);
		}

		ConcreteItemTextureLoader::BlockItemTextureLoader::BlockItemTexture::BlockItemTexture(GLuint handle, glm::vec2 size, glm::vec2 position) :
			m_Handle(handle),
			m_Size(size),
			m_Position(position) {

		}

		void ConcreteItemTextureLoader::BlockItemTextureLoader::BlockItemTexture::Bind(GLuint textureUnit) const {
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(GL_TEXTURE_2D, m_Handle);
		}

		ConcreteItemTextureLoader::BlockItemTextureLoader::ItemCube::ItemCube() :
			m_VAO(true),
			m_VBO(m_VAO.CreateVertexBuffer<Vertex3D>(true)),
			m_EBO(m_VAO.GetElementBuffer()) {
			setupBuffers();
		}

		void ConcreteItemTextureLoader::BlockItemTextureLoader::ItemCube::Render() {
			m_VAO.Draw();
		}

		void ConcreteItemTextureLoader::BlockItemTextureLoader::ItemCube::setupBuffers() {
			//Front face
			m_VBO.AddVertex({ { 0, 0, 0 }, {0.5f, 0.667f}, {0, 0, -1} });
			m_VBO.AddVertex({ { 0, 1, 0 }, {0.75f, 0.667f}, {0, 0, -1} });
			m_VBO.AddVertex({ { 1, 1, 0 }, {0.75f, 0.333f}, {0, 0, -1} });
			m_VBO.AddVertex({ { 1, 0, 0 }, {0.5f, 0.333f}, {0, 0, -1} });

			//Top face
			m_VBO.AddVertex({ { 1, 0, 0 }, {0.5f, 0.333f}, {0, -1, 0} });
			m_VBO.AddVertex({ { 1, 0, 1 }, {0.25f, 0.333f}, {0, -1, 0} });
			m_VBO.AddVertex({ { 0, 0, 1 }, {0.25f, 0.667f}, {0, -1, 0} });
			m_VBO.AddVertex({ { 0, 0, 0 }, {0.5f, 0.667f}, {0, -1, 0} });

			//Right face
			m_VBO.AddVertex({ { 0, 0, 0 }, {0.5f, 0.667f}, {-1, 0, 0} });
			m_VBO.AddVertex({ { 0, 0, 1 }, {0.25f, 0.667f}, {-1, 0, 0} });
			m_VBO.AddVertex({ { 0, 1, 1 }, {0.25f, 1.0f}, {-1, 0, 0} });
			m_VBO.AddVertex({ { 0, 1, 0 }, {0.5f, 1.0f}, {-1, 0, 0} });

			m_EBO.AddIndices({ 0, 1, 2,
				0, 2, 3,
				4, 5, 6,
				4, 6, 7,
				8, 9, 10,
				8, 10, 11 });

			m_VBO.UpdateBuffer();
			m_EBO.UpdateBuffer();

			m_VBO.ClearData();
			m_EBO.ClearData();
		}

		void ConcreteItemTextureLoader::BlockItemTextureLoader::loadBlockItemTexture(Block::BlockType type) {
			m_Framebuffer.Bind();
			m_BlockItemLoadingShader->UseProgram();

			const Texture& blockTexture = BlockTextureProvider::GetLoader().GetBlockTexture(type);
			m_BlockItemLoadingShader->SetUniform(m_TexturePositionLoc, blockTexture.GetUVPosition());
			m_BlockItemLoadingShader->SetUniform(m_TextureSizeLoc, blockTexture.GetUVSize());
			blockTexture.Bind(0);

			const glm::ivec2 framebufferSize = getFramebufferSize(), 
				texturePosition = { m_CurrentTextureIndex % framebufferSize.x , m_CurrentTextureIndex / framebufferSize.x };
			const glm::vec2 textureUVSize = { 1.f / framebufferSize.x, 1.f / framebufferSize.y };
			glm::mat4 mvp = glm::ortho<float>(0, framebufferSize.x, 0, framebufferSize.y, -1, 1);

			mvp = glm::translate(mvp, { texturePosition.x + 0.5, texturePosition.y + 0.5, 0.5f });
			mvp = glm::scale(mvp, glm::vec3(1.f / sqrt(3)));
			mvp = glm::rotate(mvp, 45 * (float)M_PI / 180, glm::vec3{ 1, 0, 0 });
			mvp = glm::rotate(mvp, -45 * (float)M_PI / 180, glm::vec3{ 0, 1, 0 });
			mvp = glm::translate(mvp, { -0.5, -0.5, -0.5 });

			m_BlockItemLoadingShader->SetMVPMatrix(mvp);

			m_Cube.Render();

			m_Framebuffer.Unbind();
			m_CurrentTextureIndex++;

			m_BlockItemTextures.insert({ type, BlockItemTexture(m_Framebuffer.GetTexture().GetHandle(),
				textureUVSize,
				(glm::vec2)texturePosition * textureUVSize) });
			/*m_BlockItemTextures.insert({ type, BlockItemTexture(m_Framebuffer.GetTexture().GetHandle(),
				glm::vec2{1, 1},
				glm::vec2{0, 0})});*/
		}

		bool ConcreteItemTextureLoader::BlockItemTextureLoader::isBlockItemTextureLoaded(Block::BlockType type) {
			return m_BlockItemTextures.find(type) != m_BlockItemTextures.end();
		}

		glm::ivec2 ConcreteItemTextureLoader::BlockItemTextureLoader::getFramebufferSize() {
			int size = pow(2, ceil(log2(sqrt((double)Block::c_BlockCount))));
			return { size, size };
		}

	} // end graphics
} // end game