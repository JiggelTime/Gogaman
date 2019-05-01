#include "pch.h"
#include "Texture3D.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	Texture3D::Texture3D()
		: width(0), height(0), depth(0), formatInternal(GL_RGB8), formatImage(GL_RGB), wrapS(GL_REPEAT), wrapT(GL_REPEAT), wrapR(GL_REPEAT), filterMin(GL_NEAREST), filterMag(GL_NEAREST), mipmapLevels(0)
	{}

	Texture3D::~Texture3D()
	{}

	void Texture3D::Generate(GLsizei width, GLsizei height, GLsizei depth, unsigned char *imageData)
	{
		this->width  = width;
		this->height = height;
		this->depth  = depth;

		glCreateTextures(GL_TEXTURE_3D, 1, &id);
		glTextureStorage3D(id, mipmapLevels + 1, formatInternal, width, height, depth);
		if(imageData != nullptr)
			glTextureSubImage3D(id, 0, 0, 0, 0, width, height, depth, formatImage, GL_UNSIGNED_BYTE, imageData);
		
		//Set texture properties
		glTextureParameteri(id, GL_TEXTURE_WRAP_S, wrapS);
		glTextureParameteri(id, GL_TEXTURE_WRAP_T, wrapT);
		glTextureParameteri(id, GL_TEXTURE_WRAP_R, wrapR);
		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, filterMin);
		if((filterMin == GL_NEAREST_MIPMAP_NEAREST || filterMin == GL_NEAREST_MIPMAP_LINEAR || filterMin == GL_LINEAR_MIPMAP_LINEAR || filterMin == GL_LINEAR_MIPMAP_NEAREST) && (mipmapLevels == 0))
			GM_LOG_CORE_WARNING("Texture using mipmap min filter with mipmap disabled");
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, filterMag);
		if((filterMag == GL_NEAREST_MIPMAP_NEAREST || filterMag == GL_NEAREST_MIPMAP_LINEAR || filterMag == GL_LINEAR_MIPMAP_LINEAR || filterMag == GL_LINEAR_MIPMAP_NEAREST) && (mipmapLevels == 0))
			GM_LOG_CORE_WARNING("Texture using mipmap mag filter with mipmap disabled");
		
		//Generate mipmap
		if(mipmapLevels > 0)
			glGenerateTextureMipmap(id);
	}

	void Texture3D::GenerateMipmap() const
	{
		glGenerateTextureMipmap(id);
		if(mipmapLevels == 0)
			GM_LOG_CORE_WARNING("Generating texture mipmap with mipmap disabled");
	}
}