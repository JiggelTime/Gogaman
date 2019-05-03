#include "pch.h"
#include "Texture.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	Texture::Texture()
		: formatInternal(GL_RGB8), formatImage(GL_RGB), filterMin(GL_NEAREST), filterMag(GL_NEAREST), levels(1)
	{}

	Texture::~Texture()
	{}

	void Texture::GenerateMipmap() const
	{
		glGenerateTextureMipmap(m_ID);
		if(levels == 1)
			GM_LOG_CORE_WARNING("Generating texture mipmap with no mipmap levels");
	}
}