#include "pch.h"
#include "OpenGL_Texture3D.h"

#include "Gogaman/Core/Base.h"
#include "Gogaman/Core/Logging/Log.h"

#include <glad.h>

namespace Gogaman
{
	Texture3D::Texture3D()
	{
		glCreateTextures(GL_TEXTURE_3D, 1, &m_RendererID);
		GM_LOG_CORE_TRACE("Created texture with ID: %d", m_RendererID);
	}

	Texture3D::~Texture3D()
	{}

	void Texture3D::Generate(const int width, const int height, const int depth, const uint8_t *imageData)
	{
		GM_ASSERT(width  > 0, "Failed to generate texture3D: invalid width")
		GM_ASSERT(height > 0, "Failed to generate texture3D: invalid height")
		GM_ASSERT(depth  > 0, "Failed to generate texture3D: invalid depth")
		GM_ASSERT((interpolationMode != TextureInterpolationMode::Trilinear) || levels != 1, "Failed to generate texture3D with trilinear interpolation: texture has only 1 level")

		this->width  = width;
		this->height = height;
		this->depth  = depth;

		if(levels == 0)
			levels = (int)floor(log2(width > height ? width : (height > depth ? height : depth))) + 1;

		glTextureStorage3D(m_RendererID, static_cast<GLsizei>(levels), GetNativeTextureInternalFormat(internalFormat), width, height, depth);
		if(imageData != nullptr)
			glTextureSubImage3D(m_RendererID, 0, 0, 0, 0, width, height, depth, GetNativeTextureFormat(format), GL_UNSIGNED_BYTE, imageData);
		
		//Set properties
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S,     GetNativeTextureWrapMode(wrapS));
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T,     GetNativeTextureWrapMode(wrapT));
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R,     GetNativeTextureWrapMode(wrapR));
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