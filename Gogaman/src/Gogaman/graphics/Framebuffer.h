#pragma once

#include "Gogaman/Core.h"
#include "Texture2D.h"
#include "Renderbuffer.h"

#include <glad.h>

namespace Gogaman
{
	class GOGAMAN_API Framebuffer
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
			//std::swap(m_NumRenderTargets,    other.m_NumRenderTargets);
			std::swap(m_RenderTargets,       other.m_RenderTargets);
			return *this;
		}

		void AttachColorBuffer(const Texture2D &texture) { AttachColorBuffer(texture, true); }
		void AttachColorBuffer(const Texture2D &texture, bool renderTarget) { AttachColorBuffer(texture, 1, renderTarget); }
		void AttachColorBuffer(const Texture2D &texture, const int level, bool renderTarget) { AttachColorBuffer(texture, level, m_NumColorAttachments, renderTarget); }
		void AttachColorBuffer(const Texture2D &texture, const int level, const int attachmentIndex, bool renderTarget);

		void AttachColorBuffer(const Renderbuffer &renderbuffer) { AttachColorBuffer(renderbuffer, m_NumColorAttachments); }
		void AttachColorBuffer(const Renderbuffer &renderbuffer, const int attachmentIndex);
		
		void AttachDepthBuffer(const Texture2D    &texture);
		void AttachDepthBuffer(const Renderbuffer &renderbuffer);

		inline void Bind()   const { glBindFramebuffer(GL_FRAMEBUFFER, m_ID); }
		inline void Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
		
		void Clear() const;

		inline GLuint GetID()               const { return m_ID; }
		inline int GetNumColorAttachments() const { return m_NumColorAttachments; }
		//inline int GetNumRenderTargets()    const { return m_NumRenderTargets; }
		inline uint GetNumRenderTargets()    const { return m_RenderTargets.size(); }
	private:
		GLuint m_ID;
		int m_NumColorAttachments;
		//int m_NumRenderTargets;
		std::vector<GLenum> m_RenderTargets;
	};
}