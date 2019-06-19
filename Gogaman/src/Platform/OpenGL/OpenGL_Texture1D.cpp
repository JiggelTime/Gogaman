#include "pch.h"
#include "OpenGL_Texture1D.h"

#include "Gogaman/Base.h"
#include "Gogaman/Logging/Log.h"

#include <glad.h>

namespace Gogaman
{
	Texture1D::Texture1D()
	{}

	Texture1D::~Texture1D()
	{}

	void Texture1D::Generate(const int width, const uint8_t *imageData)
	{
		GM_ASSERT(width > 0, "Failed to generate texture1D: invalid width")
		GM_ASSERT((interpolationMode != TextureInterpolationMode::Trilinear) || levels != 1, "Failed to generate texture1D with trilinear interpolation: texture has only 1 level")
		
		this->width = width;

		if(levels == 0)
			levels = static_cast<int>(floor(log2(width)) + 1);

		glTextureStorage1D(m_RendererID, static_cast<GLsizei>(levels), GetNativeTextureInternalFormat(internalFormat), width);
		if(imageData != nullptr)
			glTextureSubImage1D(m_RendererID, 0, 0, width, GetNativeTextureFormat(format), GL_UNSIGNED_BYTE, imageData);

		//Set properties
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S,     GetNativeTextureWrapMode(wrapS));
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GetNativeTextureInterpolationMinMode(interpolationMode));
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GetNativeTextureInterpolationMagMode(interpolationMode));
		
		//Generate mipmap
		if(levels != 1)
			glGenerateTextureMipmap(m_RendererID);
	}
}