#pragma once

#include <engine/graphics/Texture.hpp>
#include <game/world/Block.hpp>

namespace game {
	namespace graphics {
		class BlockTextureLoader {
		public:
			BlockTextureLoader() = default;
			BlockTextureLoader(BlockTextureLoader&&) = delete;
			BlockTextureLoader& operator=(BlockTextureLoader&&) = delete;

			virtual const Texture& GetWholeTexture() = 0;
			virtual const Texture& GetBlockTexture(Block::BlockType type) = 0;
		};

		class BlockTextureProvider {
		public:
			static BlockTextureLoader& GetLoader() { return *m_Instance; }
			static void SetLoader(BlockTextureLoader* instance) { m_Instance = instance; };

		private:
			static BlockTextureLoader* m_Instance;
		};
	} //end graphics
} //end game