#include "pch.h"
#include "OpenGL_Texture.h"

namespace Gogaman
{
	Texture::Texture()
		: m_RendererID(0)
	{
		glCreateTextures(GL_TEXTURE_1D, 1, &m_RendererID);
		GM_LOG_CORE_TRACE("Created texture with ID: %d", m_RendererID);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void Texture::RegenerateMipmap() const
	{
		GM_ASSERT(levels != 1, "Failed to regenerate texture mipmap: texture has 1 level")
		glGenerateTextureMipmap(m_RendererID);
	}
}