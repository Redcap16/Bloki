#pragma once

#include <engine/core/Renderer.hpp>

namespace game {
	namespace graphics {
		class DroppedItemMesh {
		public:
			virtual void Render(const RenderingContext& context) = 0;
			virtual const Texture& GetTexture() const = 0;
			virtual ShaderProgram& GetShader() = 0;
		};
	}
}