#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Gogaman/Core/Base.h"
#include "Gogaman/Core/Logging/Log.h"

namespace Gogaman
{
	enum class TextureAccess : uint8_t
	{
		Full,
		ReadOnly,
		WriteOnly
	};

	//RGB channels are automatically converted from sRGB colorspace to linear when sampled in shaders
	enum class TextureInternalFormat : uint8_t
	{
		//Depth
		              Depth16,          Depth24, Depth32,         Depth32F,
		//Depth channel and stencil channel
		                                         Depth24Stencil8,
		//One channel
		X8,           X16,     X16F,                              X32F,
		//Two channels
		XY8,          XY16,    XY16F,                             XY32F,
		//Four channels
		XYZW8, RGBW8, XYZW16,  XYZW16F,                           XYZW32F
	};

	enum class TextureFormat : uint8_t
	{
		X,
		XY,
		XYZW
	};

	enum class TextureInterpolation : uint8_t
	{
		Point,
		Bilinear,
		Trilinear,
		Anisotropic
	};

	enum class TextureWrap : uint8_t
	{
		Repeat,
		Clamp,
		Mirror
	};

	template<typename ImplementationType>
	class AbstractTexture : public CRTP<ImplementationType, AbstractTexture>
	{
	public:
		AbstractTexture(const AbstractTexture &) = delete;
		AbstractTexture(AbstractTexture &&other) = default;

		AbstractTexture &operator=(const AbstractTexture &) = delete;
		AbstractTexture &operator=(AbstractTexture &&other) = default;

		inline void RegenerateMipmap() const { this->GetImplementation().RegenerateMipmap(); }

		inline void Bind(const int unit) const { this->GetImplementation().Bind(unit); }

		inline void BindImage(const int unit)                                                                                          const { this->GetImplementation().BindImage(unit);                                }
		inline void BindImage(const int unit, const int level)                                                                         const { this->GetImplementation().BindImage(unit, level);                         }
		inline void BindImage(const int unit, const int level, const TextureAccess access)                                             const { this->GetImplementation().BindImage(unit, level, access);                 }
		inline void BindImage(const int unit, const int level, const TextureAccess access, const TextureInternalFormat internalFormat) const { this->GetImplementation().BindImage(unit, level, access, internalFormat); }
	
		static inline constexpr uint8_t GetInternalFormatComponentCount(const TextureInternalFormat internalFormat)
		{
			switch(internalFormat)
			{
				//One channel
			case TextureFormat::Depth16:
			case TextureFormat::Depth24:
			case TextureFormat::Depth32:
			case TextureFormat::Depth32F:
			case TextureFormat::X8:
			case TextureFormat::X16:
			case TextureFormat::X16F:
			case TextureFormat::X32F:
				return 1;
				//Two channels
			case TextureFormat::Depth24Stencil8:
			case TextureFormat::XY8:
			case TextureFormat::XY16:
			case TextureFormat::XY16F:
			case TextureFormat::XY32F:
				return 2;
				//Four channels
			case TextureFormat::XYZW8:
			case TextureFormat::RGBW8:
			case TextureFormat::XYZW16:
			case TextureFormat::XYZW16F:
			case TextureFormat::XYZW32F:
				return 4;
			}

			GM_ASSERT(false, "Failed to get number of texture internal format components: invalid internal format")
		}

		static inline constexpr uint8_t GetTextureFormatComponentCount(TextureFormat format)
		{
			switch(format)
			{
			case TextureFormat::X:
				return 1;
			case TextureFormat::XY:
				return 2;
			case TextureFormat::XYZW:
				return 4;
			}

			GM_ASSERT(false, "Failed to get number of texture format components: invalid format")
		}

		static inline float GetMaxAnisotropicInterpolationSamples() { return ImplementationType::GetMaxAnisotropicInterpolationSamples(); }

		static inline constexpr auto GetNativeTextureAccess(const TextureAccess access) { return ImplementationType::GetNativeTextureAccess(access); }

		static inline constexpr auto GetNativeTextureInternalFormat(const TextureInternalFormat internalFormat) { return ImplementationType::GetNativeTextureInternalFormat(internalFormat); }

		static inline constexpr auto GetNativeTextureFormat(const TextureFormat format) { return ImplementationType::GetNativeTextureFormat(format); }

		static inline constexpr auto GetNativeTextureInterpolationMin(const TextureInterpolation interpolation) { return ImplementationType::GetNativeTextureInterpolationMin(interpolation); }
	
		static inline constexpr auto GetNativeTextureInterpolationMag(const TextureInterpolation interpolation) { return ImplementationType::GetNativeTextureInterpolationMag(interpolation); }
	
		static inline constexpr auto GetNativeTextureWrap(const TextureWrap wrap) { return ImplementationType::GetNativeTextureWrap(wrap); }
	protected:
		AbstractTexture()
			: internalFormat(TextureInternalFormat::XYZW8), format(TextureFormat::XYZW), interpolation(TextureInterpolation::Point), levels(1)
		{}

		~AbstractTexture() = default;
	public:
		TextureInternalFormat internalFormat;
		TextureFormat         format;
		TextureInterpolation  interpolation;
		uint8_t               levels;
	};
}