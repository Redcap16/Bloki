#pragma once

#include <game/graphics/BlockTextureProvider.hpp>
#include <map>
#include <engine/core/Resource.hpp>
#include <engine/graphics/AtlasTexture.hpp>

namespace game {
	namespace graphics {
		class ConcreteBlockTextureLoader : public BlockTextureLoader {
		public:
			ConcreteBlockTextureLoader();

			const Texture& GetWholeTexture() override;
			const Texture& GetBlockTexture(Block::BlockType type) override;

		private:
			static std::map<Block::BlockType, AtlasTexture::SubTexture> s_BlockSubTextures;

			const std::string c_BlockTexturePath = "block.td";
			Resource<AtlasTexture> m_BlockTexture;

			bool isBlockTextureLoaded(Block::BlockType blockType) const;
			void loadBlockTexture(Block::BlockType blockType) const;
		};

	} //end graphics
} //end game