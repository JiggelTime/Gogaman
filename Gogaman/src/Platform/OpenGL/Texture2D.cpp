#include "pch.h"
#include "Texture2D.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	Texture2D::Texture2D()
		: width(0), height(0), wrapS(GL_REPEAT), wrapT(GL_REPEAT)
	{}

	Texture2D::~Texture2D()
	{}

	void Texture2D::Generate(const GLsizei width, const GLsizei height, const unsigned char *imageData)
	{
		GM_ASSERT(width  > 0, "Failed to generate texture2D: invalid width");
		GM_ASSERT(height > 0, "Failed to generate texture2D: invalid height");

		this->width  = width;
		this->height = height;
		if(levels == 0)
			levels = floor(log2(max(width, height))) + 1;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
		glTextureStorage2D(m_ID, levels, formatInternal, width, height);
		if(imageData != nullptr)
			glTextureSubImage2D(m_ID, 0, 0, 0, width, height, formatImage, GL_UNSIGNED_BYTE, imageData);

		//Set texture properties
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, wrapS);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, wrapT);
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