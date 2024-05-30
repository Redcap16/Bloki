#include <engine/graphics/Framebuffer.hpp>
#include <engine/graphics/ErrorCheck.hpp>

namespace eng {
	namespace graphics {
		Framebuffer::Framebuffer(bool useDepth, glm::ivec2 size, int channelCount) : 
			m_UsingDepth(useDepth),
			m_Size(size),
			m_TextureFormat(Texture::GetFormatFromChannelCount(channelCount)),
			m_Texture(size, m_TextureFormat),
			m_FramebufferHandle(NULL),
			m_RenderbufferHandle(NULL) {
			if(!setup())
				DebugProvider::Get().Log("Cant create framebuffer!");
		}

		Texture& Framebuffer::GetTexture() {
			return m_Texture;
		}

		void Framebuffer::Bind() {
			glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferHandle);
			glViewport(0, 0, m_Size.x, m_Size.y);
		}

		void Framebuffer::Unbind() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		Framebuffer::FramebufferTexture::FramebufferTexture(glm::ivec2 size, GLuint format) {
			glGenTextures(1, &m_Handle);
			glBindTexture(GL_TEXTURE_2D, m_Handle);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, format, size.x, size.y, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
			CHECK_GL_ERROR();
		}

		void Framebuffer::FramebufferTexture::Bind(GLuint textureUnit) const {
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(GL_TEXTURE_2D, m_Handle);
		}

		TextureHandle Framebuffer::FramebufferTexture::GetHandle() const {
			return m_Handle;
		}

		bool Framebuffer::setup() {
			glGenFramebuffers(1, &m_FramebufferHandle);
			glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferHandle);
			
			if (m_UsingDepth) {
				glGenRenderbuffers(1, &m_RenderbufferHandle);
				glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferHandle);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Size.x, m_Size.y);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferHandle);
				CHECK_GL_ERROR();
			}

			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_Texture.GetHandle(), 0);

			GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, drawBuffers);
			
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				return false;

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			CHECK_GL_ERROR();

			return true;
		}
	}
}