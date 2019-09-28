#include "pch.h"
#include "OpenGL_RenderSurface.h"

#include "Gogaman/Core/Base.h"
#include "Gogaman/Core/Logging/Log.h"

namespace Gogaman
{
	RenderSurface::RenderSurface()
		: m_RendererID(0), m_NumColorAttachments(0)
	{
		glCreateFramebuffers(1, &m_RendererID);
		GM_LOG_CORE_TRACE("Created render surface with ID: %d", m_RendererID);
	}

	RenderSurface::~RenderSurface()
	{
		glDeleteFramebuffers(1, &m_RendererID);
	}

	void RenderSurface::AddColorBuffer(const Texture &texture, const int level, const int attachmentIndex, const bool isRenderTarget)
	{
		GM_ASSERT(attachmentIndex <= m_NumColorAttachments && attachmentIndex >= 0, "Failed to attach color buffer to render surface: invalid attachment index");
		GM_ASSERT(level >= 1 && (texture.levels >= level || texture.levels == 0),   "Failed to attach color buffer to render surface: invalid level");

		glNamedFramebufferTexture(m_RendererID, GL_COLOR_ATTACHMENT0 + attachmentIndex, texture.GetRendererID(), level - 1);

		if(attachmentIndex == m_NumColorAttachments)
		{
			m_NumColorAttachments++;
			if(isRenderTarget)
			{
				m_RenderTargets.emplace_back(GL_COLOR_ATTACHMENT0 + attachmentIndex);
				glNamedFramebufferDrawBuffers(m_RendererID, static_cast<GLsizei>(m_RenderTargets.size()), m_RenderTargets.data());
			}
		}
	}

	void RenderSurface::AddColorBuffer(const RenderBuffer &renderbuffer, const int attachmentIndex)
	{
		GM_ASSERT(attachmentIndex <= m_NumColorAttachments && attachmentIndex >= 0, "Failed to attach color buffer to render surface: invalid attachment index");
		glNamedFramebufferRenderbuffer(m_RendererID, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_RENDERBUFFER, renderbuffer.GetRendererID());

		if(attachmentIndex == m_NumColorAttachments)
		{
			m_NumColorAttachments++;
			m_RenderTargets.emplace_back(GL_COLOR_ATTACHMENT0 + attachmentIndex);
			glNamedFramebufferDrawBuffers(m_RendererID, static_cast<GLsizei>(m_RenderTargets.size()), m_RenderTargets.data());
		}
	}

	void RenderSurface::AddDepthBuffer(const Texture &texture)
	{
		GM_ASSERT((texture.internalFormat == TextureInternalFormat::Depth16) || (texture.internalFormat == TextureInternalFormat::Depth24) || (texture.internalFormat == TextureInternalFormat::Depth32) || (texture.internalFormat == TextureInternalFormat::Depth32F), "Failed to attach depth buffer to render surface: invalid texture internal format");
		glNamedFramebufferTexture(m_RendererID, GL_DEPTH_ATTACHMENT, texture.GetRendererID(), 0);
	}

	void RenderSurface::AddDepthBuffer(const RenderBuffer &renderbuffer)
	{
		GM_ASSERT((renderbuffer.internalFormat == TextureInternalFormat::Depth16) || (renderbuffer.internalFormat == TextureInternalFormat::Depth24) || (renderbuffer.internalFormat == TextureInternalFormat::Depth32) || (renderbuffer.internalFormat == TextureInternalFormat::Depth32F), "Failed to attach depth buffer to render surface: invalid render buffer internal format");
		glNamedFramebufferRenderbuffer(m_RendererID, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer.GetRendererID());
	}

	void RenderSurface::Bind() const
	{
		ValidateRenderSurface();
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	}

	void RenderSurface::Clear() const
	{
		const GLfloat clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		glClearNamedFramebufferfv(m_RendererID, GL_DEPTH, 0, &clearColor[0]);
		for(int i = 0; i < m_NumColorAttachments; i++)
			glClearNamedFramebufferfv(m_RendererID, GL_COLOR, i, clearColor);
	}

	void RenderSurface::BlitColorBuffer(const RenderSurface &source, const int width, const int height, const TextureInterpolationMode interpolationMode) const
	{
		GM_ASSERT(interpolationMode != TextureInterpolationMode::Trilinear, "Failed to blit render surface color buffer: invalid interpolation mode")
		glBlitNamedFramebuffer(source.GetRendererID(), m_RendererID, 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, Texture::GetNativeTextureInterpolationMinMode(interpolationMode));
	}

	void RenderSurface::BlitDepthBuffer(const RenderSurface &source, const int width, const int height, const TextureInterpolationMode interpolationMode) const
	{
		GM_ASSERT(interpolationMode != TextureInterpolationMode::Trilinear, "Failed to blit render surface depth buffer: invalid interpolation mode")
		glBlitNamedFramebuffer(source.GetRendererID(), m_RendererID, 0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, Texture::GetNativeTextureInterpolationMinMode(interpolationMode));
	}

	void RenderSurface::BlitStencilBuffer(const RenderSurface &source, const int width, const int height, const TextureInterpolationMode interpolationMode) const
	{
		GM_ASSERT(interpolationMode != TextureInterpolationMode::Trilinear, "Failed to blit render surface stencil buffer: invalid interpolation mode")
		glBlitNamedFramebuffer(source.GetRendererID(), m_RendererID, 0, 0, width, height, 0, 0, width, height, GL_STENCIL_BUFFER_BIT, Texture::GetNativeTextureInterpolationMinMode(interpolationMode));
	}

	void RenderSurface::ClearBackBuffer()
	{
		const GLfloat clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		glClearNamedFramebufferfv(0, GL_DEPTH, 0, &clearColor[0]);
		glClearNamedFramebufferfv(0, GL_COLOR, 0, clearColor);
	}

	void RenderSurface::ValidateRenderSurface() const
	{
		GM_ASSERT(glCheckNamedFramebufferStatus(m_RendererID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Incomplete render surface | Render surface status: %d", glCheckNamedFramebufferStatus(m_RendererID, GL_FRAMEBUFFER))
	}
}