#pragma once

#include "Gogaman/Graphics/Texture/AbstractTexture.h"

#include "Gogaman/Base.h"
#include "Gogaman/Logging/Log.h"

#include <glad.h>

namespace Gogaman
{
	class Texture : public AbstractTexture<Texture>
	{
	public:
		Texture(const Texture &) = delete;
		Texture(Texture &&)      = default;

		Texture &operator=(const Texture &) = delete;
		Texture &operator=(Texture &&)      = default;

		void RegenerateMipmap() const;
		
		inline void Bind(const int unit) const { glBindTextureUnit(unit, m_RendererID); }

		inline void BindImage(const int unit)                                                      const { BindImage(unit, 1); }
		inline void BindImage(const int unit, const int level)                                     const { BindImage(unit, level, TextureAccessMode::Full); }
		inline void BindImage(const int unit, const int level, const TextureAccessMode accessMode) const { BindImage(unit, level, accessMode, internalFormat); }
		inline void BindImage(const int unit, const int level, const TextureAccessMode accessMode, const TextureInternalFormat internalFormat) const
		{
			glBindImageTexture(unit, m_RendererID, level - 1, levels != 1 ? GL_TRUE : GL_FALSE, 0, GetNativeTextureAccessMode(accessMode), GetNativeTextureInternalFormat(internalFormat));
		}
		
		inline void Clear() const { glClearTexImage(m_RendererID, 0, GetNativeTextureFormat(format), GL_UNSIGNED_BYTE, nullptr); }
		
		inline uint32_t GetRendererID() const { return m_RendererID; }

		static constexpr GLenum GetNativeTextureAccessMode(const TextureAccessMode accessMode)
		{
			switch(accessMode)
			{
			case TextureAccessMode::Full:
				return GL_READ_WRITE;
			case TextureAccessMode::ReadOnly:
				return GL_READ_ONLY;
			case TextureAccessMode::WriteOnly:
				return GL_WRITE_ONLY;
			default:
				GM_ASSERT(false, "Failed to get native texture access: invalid access type")
			}
		}

		static constexpr GLenum GetNativeTextureInternalFormat(const TextureInternalFormat internalFormat)
		{
			switch(internalFormat)
			{
				//Depth channel
			case TextureInternalFormat::Depth16:
				return GL_DEPTH_COMPONENT16;
			case TextureInternalFormat::Depth24:
				return GL_DEPTH_COMPONENT24;
			case TextureInternalFormat::Depth32:
				return GL_DEPTH_COMPONENT32;
			case TextureInternalFormat::Depth32F:
				return GL_DEPTH_COMPONENT32F;
				//Depth channel and stencil channel
			case TextureInternalFormat::Depth24Stencil8:
				return GL_DEPTH24_STENCIL8;
				//One channel
			case TextureInternalFormat::X8:
				return GL_R8;
			case TextureInternalFormat::X16:
				return GL_R16;
			case TextureInternalFormat::X16F:
				return GL_R16F;
			case TextureInternalFormat::X32F:
				return GL_R32F;
				//Two channels
			case TextureInternalFormat::XY8:
				return GL_RG8;
			case TextureInternalFormat::XY16:
				return GL_RG16;
			case TextureInternalFormat::XY16F:
				return GL_RG16F;
			case TextureInternalFormat::XY32F:
				return GL_RG32F;
				//Four channels
			case TextureInternalFormat::XYZW8:
				return GL_RGBA8;
			case TextureInternalFormat::RGBW8:
				return GL_SRGB8_ALPHA8;
			case TextureInternalFormat::XYZW16:
				return GL_RGBA16;
			case TextureInternalFormat::XYZW16F:
				return GL_RGBA16F;
			case TextureInternalFormat::XYZW32F:
				return GL_RGBA32F;
			}
			
			GM_ASSERT(false, "Failed to get native texture internal format: invalid internal format")
			return 0;
		}

		static constexpr GLenum GetNativeTextureFormat(const TextureFormat format)
		{
			switch(format)
			{
			case TextureFormat::X:
				return GL_RED;
			case TextureFormat::XY:
				return GL_RG;
			case TextureFormat::XYZW:
				return GL_RGBA;
			}
			
			GM_ASSERT(false, "Failed to get native texture format: invalid format")
			return 0;
		}

		static constexpr GLint GetNativeTextureInterpolationMinMode(const TextureInterpolationMode interpolationMode)
		{
			switch(interpolationMode)
			{
			case TextureInterpolationMode::Point:
				return GL_NEAREST;
			case TextureInterpolationMode::Bilinear:
				return GL_LINEAR;
			case TextureInterpolationMode::Trilinear:
				return GL_LINEAR_MIPMAP_LINEAR;
			}
			
			GM_ASSERT(false, "Failed to get native texture interpolation min mode: invalid interpolation mode")
			return 0;
		}

		static constexpr GLint GetNativeTextureInterpolationMagMode(const TextureInterpolationMode interpolationMode)
		{
			switch(interpolationMode)
			{
			case TextureInterpolationMode::Point:
				return GL_NEAREST;
			case TextureInterpolationMode::Bilinear:
				return GL_LINEAR;
			case TextureInterpolationMode::Trilinear:
				return GL_LINEAR;
			}
			
			GM_ASSERT(false, "Failed to get native texture interpolation mag mode: invalid interpolation mode")
			return 0;
		}

		static constexpr GLint GetNativeTextureWrapMode(const TextureWrapMode wrapMode)
		{
			switch(wrapMode)
			{
			case TextureWrapMode::Repeat:
				return GL_REPEAT;
			case TextureWrapMode::Clamp:
				return GL_CLAMP_TO_EDGE;
			case TextureWrapMode::Mirror:
				return GL_MIRRORED_REPEAT;
			}
			
			GM_ASSERT(false, "Failed to get native texture wrap mode: invalid wrap mode")
			return 0;
		}
	protected:
		Texture();
		~Texture();
	protected:
		uint32_t m_RendererID;
	};
}