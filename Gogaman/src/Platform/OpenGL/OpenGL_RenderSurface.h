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
		RenderSurface(RenderSurface &&)      = default;

		~RenderSurface();

		RenderSurface &operator=(const RenderSurface &) = delete;
		RenderSurface &operator=(RenderSurface &&)      = default;

		void AddColorBuffer(const Texture &texture)                                             { AddColorBuffer(texture, true); }
		void AddColorBuffer(const Texture &texture, const bool isRenderTarget)                  { AddColorBuffer(texture, 1, isRenderTarget); }
		void AddColorBuffer(const Texture &texture, const int level, const bool isRenderTarget) { AddColorBuffer(texture, level, m_NumColorAttachments, isRenderTarget); }
		void AddColorBuffer(const Texture &texture, const int level, const int attachmentIndex, const bool isRenderTarget);

		void AddColorBuffer(const Renderbuffer &renderbuffer) { AddColorBuffer(renderbuffer, m_NumColorAttachments); }
		void AddColorBuffer(const Renderbuffer &renderbuffer, const int attachmentIndex);
		
		void AddDepthBuffer(const Texture      &texture);
		void AddDepthBuffer(const Renderbuffer &renderbuffer);

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