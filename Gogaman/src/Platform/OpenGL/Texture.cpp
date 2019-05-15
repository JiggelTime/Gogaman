#include "pch.h"
#include "Texture.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	Texture::Texture()
		: m_ID(0), formatInternal(GL_RGBA8), formatImage(GL_RGBA), filterMin(GL_NEAREST), filterMag(GL_NEAREST), levels(1)
	{}

	Texture::~Texture()
	{
		if(!m_ID)
			return;

		glDeleteTextures(1, &m_ID);
	}

	void Texture::RegenerateMipmap() const
	{
		glGenerateTextureMipmap(m_ID);
		if(levels == 1)
			GM_LOG_CORE_WARNING("Regenerating texture mipmap with no mipmap levels");
	}
}