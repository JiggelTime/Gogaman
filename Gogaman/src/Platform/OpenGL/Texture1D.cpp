#include "pch.h"
#include "Texture1D.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	Texture1D::Texture1D()
		: width(0), wrapS(GL_REPEAT)
	{}

	Texture1D::~Texture1D()
	{}

	void Texture1D::Generate(const GLsizei width, const unsigned char *imageData)
	{
		GM_ASSERT(width > 0, "Failed to generate texture1D: invalid width");

		this->width  = width;

		if(levels == 0)
			levels = static_cast<GLsizei>(floor(log2(width)) + 1);

		glCreateTextures(GL_TEXTURE_1D, 1, &m_ID);
		glTextureStorage1D(m_ID, levels, formatInternal, width);
		if(imageData != nullptr)
			glTextureSubImage1D(m_ID, 0, 0, width, formatImage, GL_UNSIGNED_BYTE, imageData);

		//Set properties
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, wrapS);
		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, filterMin);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, filterMag);
		if(filterMin != GL_NEAREST && filterMin != GL_LINEAR && levels == 1)
			GM_LOG_CORE_WARNING("Texture using mipmap min filter with no mipmap levels");
		if(filterMag != GL_NEAREST && filterMag != GL_LINEAR && levels == 1)
			GM_LOG_CORE_WARNING("Texture using mipmap mag filter with no mipmap levels");
		
		//Generate mipmap
		if(levels != 1)
			glGenerateTextureMipmap(m_ID);
	}
}