#include "pch.h"
#include "Texture3D.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	Texture3D::Texture3D()
		: Texture(), width(0), height(0), depth(0), wrapS(GL_REPEAT), wrapT(GL_REPEAT), wrapR(GL_REPEAT)
	{}

	Texture3D::~Texture3D()
	{}

	void Texture3D::Generate(const GLsizei &width, const GLsizei &height, const GLsizei &depth, const unsigned char *imageData)
	{
		this->width  = width;
		this->height = height;
		this->depth  = depth;

		glCreateTextures(GL_TEXTURE_3D, 1, &m_ID);
		glTextureStorage3D(m_ID, levels == 0 ? floor(log2(max(width, max(height, depth)))) + 1 : levels, formatInternal, width, height, depth);
		if(imageData != nullptr)
			glTextureSubImage3D(m_ID, 0, 0, 0, 0, width, height, depth, formatImage, GL_UNSIGNED_BYTE, imageData);
		
		//Set texture properties
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, wrapS);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, wrapT);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_R, wrapR);
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