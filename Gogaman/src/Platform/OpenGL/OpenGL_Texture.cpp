#include "pch.h"
#include "OpenGL_Texture.h"

namespace Gogaman
{
	Texture::Texture()
		: m_RendererID(0)
	{}

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