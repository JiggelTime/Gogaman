#include "pch.h"
#include "Texture2D.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	Texture2D::Texture2D()
		: width(0), height(0), formatInternal(GL_RGB8), formatImage(GL_RGB), wrapS(GL_REPEAT), wrapT(GL_REPEAT), filterMin(GL_NEAREST), filterMag(GL_NEAREST), mipmapLevels(0)
	{}

	Texture2D::~Texture2D()
	{}

	void Texture2D::Generate(GLsizei width, GLsizei height, unsigned char *imageData)
	{
		this->width  = width;
		this->height = height;

		glCreateTextures(GL_TEXTURE_2D, 1, &id);
		glTextureStorage2D(id, mipmapLevels + 1, formatInternal, width, height);
		if(imageData != nullptr)
			glTextureSubImage2D(id, 0, 0, 0, width, height, formatImage, GL_UNSIGNED_BYTE, imageData);

		//Set texture properties
		glTextureParameteri(id, GL_TEXTURE_WRAP_S, wrapS);
		glTextureParameteri(id, GL_TEXTURE_WRAP_T, wrapT);
		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, filterMin);
		if((filterMin == GL_NEAREST_MIPMAP_NEAREST || filterMin == GL_NEAREST_MIPMAP_LINEAR || filterMin == GL_LINEAR_MIPMAP_LINEAR || filterMin == GL_LINEAR_MIPMAP_NEAREST) && (mipmapLevels == 0))
			GM_LOG_CORE_WARNING("Texture using mipmap min filter with mipmap disabled");
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, filterMag);
		if((filterMag == GL_NEAREST_MIPMAP_NEAREST || filterMag == GL_NEAREST_MIPMAP_LINEAR || filterMag == GL_LINEAR_MIPMAP_LINEAR || filterMag == GL_LINEAR_MIPMAP_NEAREST) && (mipmapLevels == 0))
			GM_LOG_CORE_WARNING("Texture using mipmap max filter with mipmap disabled");
		
		//Generate mipmap
		if(mipmapLevels > 0)
			glGenerateTextureMipmap(id);
	}

	void Texture2D::GenerateMipmap() const
	{
		glGenerateTextureMipmap(id);
		if(mipmapLevels == 0)
			GM_LOG_CORE_WARNING("Generating texture mipmap with mipmap disabled");
	}
}