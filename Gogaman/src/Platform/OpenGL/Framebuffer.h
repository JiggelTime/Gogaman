#pragma once

#include "Gogaman/Base.h"
#include "Texture.h"
#include "Renderbuffer.h"

#include <glad.h>

namespace Gogaman
{
	class Framebuffer
	{
	public:
		Framebuffer();
		Framebuffer(const Framebuffer &) = delete;
		Framebuffer(Framebuffer &&other) noexcept
			: m_ID(std::exchange(other.m_ID, 0)), m_NumColorAttachments(std::exchange(other.m_NumColorAttachments, 0)), m_RenderTargets(std::move(other.m_RenderTargets))
		{}

		~Framebuffer();

		Framebuffer &operator=(const Framebuffer &) = delete;
		Framebuffer &operator=(Framebuffer &&other) noexcept
		{
			std::swap(m_ID,                  other.m_ID);
			std::swap(m_NumColorAttachments, other.m_NumColorAttachments);
			std::swap(m_RenderTargets,       other.m_RenderTargets);
			return *this;
		}

		void AttachColorBuffer(const Texture &texture) { AttachColorBuffer(texture, true); }
		void AttachColorBuffer(const Texture &texture, bool isRenderTarget) { AttachColorBuffer(texture, 1, isRenderTarget); }
		void AttachColorBuffer(const Texture &texture, const int level, bool isRenderTarget) { AttachColorBuffer(texture, level, m_NumColorAttachments, isRenderTarget); }
		void AttachColorBuffer(const Texture &texture, const int level, const int attachmentIndex, bool isRenderTarget);

		void AttachColorBuffer(const Renderbuffer &renderbuffer) { AttachColorBuffer(renderbuffer, m_NumColorAttachments); }
		void AttachColorBuffer(const Renderbuffer &renderbuffer, const int attachmentIndex);
		
		void AttachDepthBuffer(const Texture      &texture);
		void AttachDepthBuffer(const Renderbuffer &renderbuffer);

		inline void Bind()   const { glBindFramebuffer(GL_FRAMEBUFFER, m_ID); }
		inline void Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
		
		void Clear() const;

		inline void BlitColorBuffer(const Framebuffer &source, const GLint width, const GLint height, GLenum filter) const { glBlitNamedFramebuffer(source.GetID(), m_ID, 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, filter); }
		inline void BlitDepthBuffer(const Framebuffer &source, const GLint width, const GLint height, GLenum filter) const { glBlitNamedFramebuffer(source.GetID(), m_ID, 0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, filter); }
		inline void BlitStencilBuffer(const Framebuffer &source, const GLint width, const GLint height, GLenum filter) const { glBlitNamedFramebuffer(source.GetID(), m_ID, 0, 0, width, height, 0, 0, width, height, GL_STENCIL_BUFFER_BIT, filter); }

		inline GLuint GetID()                  const { return m_ID; }
		inline size_t GetNumColorAttachments() const { return m_NumColorAttachments; }
		inline size_t GetNumRenderTargets()    const { return m_RenderTargets.size(); }
	private:
		GLuint m_ID;
		int m_NumColorAttachments;
		std::vector<GLenum> m_RenderTargets;
	};
}