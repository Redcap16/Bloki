#include <game/graphics/ConcreteBlockTextureLoader.hpp>

namespace game {
	namespace graphics {
		std::map<Block::BlockType, AtlasTexture::SubTexture> ConcreteBlockTextureLoader::s_BlockSubTextures;

		ConcreteBlockTextureLoader::ConcreteBlockTextureLoader() :
			m_BlockTexture(c_BlockTexturePath) {

		}

		const Texture& ConcreteBlockTextureLoader::GetBlockTexture(Block::BlockType type) {
			if (!isBlockTextureLoaded(type))
				loadBlockTexture(type);

			return s_BlockSubTextures.at(type);
		}

		const Texture& ConcreteBlockTextureLoader::GetWholeTexture() {
			return *m_BlockTexture.Get();
		}

		bool ConcreteBlockTextureLoader::isBlockTextureLoaded(Block::BlockType blockType) const {
			return s_BlockSubTextures.find(blockType) != s_BlockSubTextures.end();
		}

		void ConcreteBlockTextureLoader::loadBlockTexture(Block::BlockType blockType) const {
			const std::string& blockName = Block::GetBlockName(blockType);
			const AtlasTexture::SubTexture* subTexture = m_BlockTexture->GetSubTexture(blockName);

			if (subTexture == nullptr) {
				DEBUG_LOG("Error: cant load block subtextures");
				return;
			}
			s_BlockSubTextures[blockType] = *subTexture;
		}

	} //end graphics
} //end game