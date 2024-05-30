#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <glm/glm.hpp>
#include <engine/graphics/Texture.hpp>

namespace eng {
	namespace graphics {
		class Framebuffer {
		public:
			Framebuffer(bool useDepth, glm::ivec2 size, int channelCount);
			Texture& GetTexture();

			void Bind();
			static void Unbind();

		private:
			class FramebufferTexture : public Texture {
			public:
				FramebufferTexture(glm::ivec2 size, GLuint format);

				void Bind(GLuint textureUnit) const override;
				TextureHandle GetHandle() const override;

			private:
				GLuint m_Handle;
			};

			GLuint m_TextureFormat;
			FramebufferTexture m_Texture;
			glm::ivec2 m_Size;
			bool m_UsingDepth;

			GLuint m_FramebufferHandle,
				m_RenderbufferHandle;

			bool setup();
		};
	} //graphics
} //eng