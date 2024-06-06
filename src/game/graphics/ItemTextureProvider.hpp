#pragma once

#include <engine/graphics/Texture.hpp>
#include <game/world/Block.hpp>
#include <glm/glm.hpp>

namespace game {
	namespace graphics {
		class ItemTextureLoader {
		public:
			ItemTextureLoader() = default;
			ItemTextureLoader(const ItemTextureLoader&) = delete;
			ItemTextureLoader& operator=(const ItemTextureLoader&) = delete;

			virtual const Texture& GetTextureForBlock(Block::BlockType type) = 0;
			virtual const Texture& GetTextureForFlat(glm::ivec2 position) = 0;
		};

		class ItemTextureProvider {
		public:
			static ItemTextureLoader& GetLoader() { return *m_Instance; };
			static void SetLoader(ItemTextureLoader* instance) { m_Instance = instance; };

		private:
			static ItemTextureLoader* m_Instance;
		};
	} // end graphics
} // end game