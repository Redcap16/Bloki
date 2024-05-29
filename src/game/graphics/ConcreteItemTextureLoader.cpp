#include <game/graphics/ConcreteItemTextureLoader.hpp>
#include <game/graphics/ChunkRenderer.hpp>

namespace game {
	namespace graphics {
		ConcreteItemTextureLoader::ConcreteItemTextureLoader(BlockTextureLoader& blockTextureLoader) :
			m_FlatItemTexture(c_FlatItemTextureFilepath),
			m_BlockTextureLoader(blockTextureLoader) {

		}

		const Texture& ConcreteItemTextureLoader::GetTextureForBlock(Block::BlockType type) {
			return m_BlockTextureLoader.GetBlockTexture(type);
		}

		const Texture& ConcreteItemTextureLoader::GetTextureForFlat(glm::ivec2 position) {
			const glm::vec2 uvSize = 1.f / (glm::vec2)c_TextureSizeInTiles,
				uvPosition = (glm::vec2)position * uvSize;
			
			return *m_FlatItemTexture->GetSubTexture(uvPosition, uvSize);
		}

	} // end graphics
} // end game