#pragma once

#include <game/graphics/ItemTextureProvider.hpp>
#include <game/graphics/BlockTextureProvider.hpp>
#include <engine/core/Resource.hpp>
#include <engine/graphics/AtlasTexture.hpp>

namespace game {
	namespace graphics {
		class ConcreteItemTextureLoader : public ItemTextureLoader {
		public:
			ConcreteItemTextureLoader(BlockTextureLoader& blockTextureLoader);

			const Texture& GetTextureForBlock(Block::BlockType type) override;
			const Texture& GetTextureForFlat(glm::ivec2 position) override;

		private:
			const glm::ivec2 c_TextureSizeInTiles = { 8, 8 };
			const std::string c_FlatItemTextureFilepath = "items.td";
			Resource<AtlasTexture> m_FlatItemTexture;

			BlockTextureLoader& m_BlockTextureLoader;
		};
	} //end graphics
} //end game