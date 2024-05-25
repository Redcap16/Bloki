#pragma once

#include <game/graphics/ItemTextureProvider.hpp>
#include <engine/core/Resource.hpp>
#include <engine/graphics/AtlasTexture.hpp>

namespace game {
	namespace graphics {
		class ConcreteItemTextureLoader : public ItemTextureLoader {
		public:
			ConcreteItemTextureLoader();

			const Texture& GetTextureForBlock(Block::BlockType type) override;
			const Texture& GetTextureForFlat(glm::ivec2 position) override;

		private:
			const glm::ivec2 c_TextureSizeInTiles = { 8, 8 };
			const std::string c_FlatItemTextureFilepath = "items.td";
			Resource<AtlasTexture> m_FlatItemTexture;

			static std::map<Block::BlockType, AtlasTexture::SubTexture> s_BlockSubTextures;
			const std::string c_BlockItemTexturePath = "block.td";
			Resource<AtlasTexture> m_BlockItemTexture;


			bool isBlockTextureLoaded(Block::BlockType blockType) const;
			void loadBlockTexture(Block::BlockType blockType) const;
		};
	} //end graphics
} //end game