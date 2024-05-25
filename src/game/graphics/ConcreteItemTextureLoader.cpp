#include <game/graphics/ConcreteItemTextureLaoder.hpp>
#include <game/graphics/ChunkRenderer.hpp>

namespace game {
	namespace graphics {
		std::map<Block::BlockType, AtlasTexture::SubTexture> ConcreteItemTextureLoader::s_BlockSubTextures;

		ConcreteItemTextureLoader::ConcreteItemTextureLoader() :
			m_FlatItemTexture(c_FlatItemTextureFilepath),
			m_BlockItemTexture(c_BlockItemTexturePath) {

		}

		const Texture& ConcreteItemTextureLoader::GetTextureForBlock(Block::BlockType type) {
			if (!isBlockTextureLoaded(type))
				loadBlockTexture(type);

			return s_BlockSubTextures[type];
		}

		const Texture& ConcreteItemTextureLoader::GetTextureForFlat(glm::ivec2 position) {
			const glm::vec2 uvSize = 1.f / (glm::vec2)c_TextureSizeInTiles,
				uvPosition = (glm::vec2)position * uvSize;
			
			return *m_FlatItemTexture->GetSubTexture(uvPosition, uvSize);
		}

		bool ConcreteItemTextureLoader::isBlockTextureLoaded(Block::BlockType blockType) const {
			return s_BlockSubTextures.find(blockType) != s_BlockSubTextures.end();
		}

		void ConcreteItemTextureLoader::loadBlockTexture(Block::BlockType blockType) const {
			const std::string& blockName = Block::GetBlockName(blockType);
			const AtlasTexture::SubTexture* subTexture = m_BlockItemTexture->GetSubTexture(blockName);

			if (subTexture == nullptr) {
				DEBUG_LOG("Error: cant load block subtextures");
				return;
			}
			s_BlockSubTextures[blockType] = *subTexture;
		}

	} // end graphics
} // end game