#include "pch.h"
#include "OpenGL_Texture2D.h"

#include "Gogaman/Core/Base.h"
#include "Gogaman/Core/Logging/Log.h"

#include <glad.h>

namespace Gogaman
{
	Texture2D::Texture2D()
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		GM_LOG_CORE_TRACE("Created texture with ID: %d", m_RendererID);
	}

	Texture2D::~Texture2D()
	{}

	void Texture2D::Generate(const int width, const int height, const uint8_t *imageData)
	{
		GM_ASSERT(width  > 0, "Failed to generate texture2D: invalid width")
		GM_ASSERT(height > 0, "Failed to generate texture2D: invalid height")
		GM_ASSERT((interpolationMode != TextureInterpolationMode::Trilinear) || levels != 1, "Failed to generate texture2D with trilinear interpolation: texture has only 1 level")

		this->width  = width;
		this->height = height;

		if(levels == 0)
			levels = (int)floor(log2(width > height ? width : height)) + 1;

		glTextureStorage2D(m_RendererID, static_cast<GLsizei>(levels), GetNativeTextureInternalFormat(internalFormat), width, height);
		if(imageData != nullptr)
			glTextureSubImage2D(m_RendererID, 0, 0, 0, width, height, GetNativeTextureFormat(format), GL_UNSIGNED_BYTE, imageData);

		//Set properties
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S,     GetNativeTextureWrapMode(wrapS));
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T,     GetNativeTextureWrapMode(wrapT));
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GetNativeTextureInterpolationMinMode(interpolationMode));
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GetNativeTextureInterpolationMagMode(interpolationMode));
		if(interpolationMode == TextureInterpolationMode::Anisotropic)
		{
			glTextureParameterf(m_RendererID, GL_TEXTURE_MAX_ANISOTROPY_EXT, GetMaxAnisotropicInterpolationSamples());
		}

		//Generate mipmap
		if(levels != 1)
			glGenerateTextureMipmap(m_RendererID);
	}
}