#include "pch.h"
#include "OpenGL_RenderBuffer.h"

#include "Gogaman/Core/Base.h"
#include "Gogaman/Core/Logging/Log.h"

namespace Gogaman
{
	RenderBuffer::RenderBuffer()
		: m_RendererID(0), width(0), height(0), internalFormat(TextureInternalFormat::XYZW8)
	{
		glCreateRenderbuffers(1, &m_RendererID);
		GM_LOG_CORE_TRACE("Created render buffer with ID: %d", m_RendererID);
	}

	RenderBuffer::~RenderBuffer()
	{
		glDeleteRenderbuffers(1, &m_RendererID);
	}

	void RenderBuffer::Generate(const int width, const int height)
	{
		GM_ASSERT(width  > 0, "Failed to generate renderbuffer: invalid width")
		GM_ASSERT(height > 0, "Failed to generate renderbuffer: invalid height")

		this->width  = width;
		this->height = height;

		glNamedRenderbufferStorage(m_RendererID, Texture::GetNativeTextureInternalFormat(internalFormat), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	}
}