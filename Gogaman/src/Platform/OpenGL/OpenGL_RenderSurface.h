#pragma once

#include "Gogaman/Graphics/AbstractRenderSurface.h"

#include <glad.h>

namespace Gogaman
{
	class RenderSurface : public AbstractRenderSurface<RenderSurface>
	{
	public:
		RenderSurface();
		RenderSurface(const RenderSurface &) = delete;
		RenderSurface(RenderSurface &&other) noexcept
			: m_RendererID(std::exchange(other.m_RendererID, 0)), m_NumColorAttachments(std::exchange(other.m_NumColorAttachments, 0)), m_RenderTargets(std::move(other.m_RenderTargets))
		{}

		~RenderSurface();

		RenderSurface &operator=(const RenderSurface &) = delete;
		RenderSurface &operator=(RenderSurface &&other) noexcept
		{
			std::swap(m_RendererID,          other.m_RendererID);
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

		void Bind() const;
		
		void Clear() const;

		void BlitColorBuffer(const   RenderSurface &source, const int width, const int height, const TextureInterpolationMode interpolationMode) const;
		void BlitDepthBuffer(const   RenderSurface &source, const int width, const int height, const TextureInterpolationMode interpolationMode) const;
		void BlitStencilBuffer(const RenderSurface &source, const int width, const int height, const TextureInterpolationMode interpolationMode) const;

		inline int GetNumColorAttachments() const { return m_NumColorAttachments;  }
		inline int GetNumRenderTargets()    const { return static_cast<int>(m_RenderTargets.size()); }
		
		inline uint32_t GetRendererID() const { return m_RendererID; }
		
		static inline void BindBackBuffer() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

		static void ClearBackBuffer();
	private:
		void ValidateRenderSurface() const;
	private:
		int                 m_NumColorAttachments;
		std::vector<GLenum> m_RenderTargets;
		uint32_t            m_RendererID;
	};
}