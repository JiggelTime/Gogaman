#pragma once

#include "Gogaman/CRTP.h"
#include "Gogaman/Resource.h"

#include "Gogaman/Base.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	enum class TextureAccessMode : uint8_t
	{
		None = 0,
		Full,
		ReadOnly,
		WriteOnly
	};

	//RGB channels are automatically converted from sRGB colorspace to linear when sampled in shaders
	enum class TextureInternalFormat : uint8_t
	{
		None = 0,
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
		None = 0,
		X,
		XY,
		XYZW
	};

	enum class TextureInterpolationMode : uint8_t
	{
		None = 0,
		Point,
		Bilinear,
		Trilinear,
		Anisotropic2x,
		Anisotropic4x,
		Anisotropic8x,
		Anisotropic16x
	};

	enum class TextureWrapMode : uint8_t
	{
		None = 0,
		Repeat,
		Clamp,
		Mirror
	};

	using TextureID = uint16_t;

	template<typename TextureType>
	class AbstractTexture : public CRTP<TextureType, AbstractTexture>, public Resource<TextureID>
	{
	public:
		AbstractTexture(const AbstractTexture &) = delete;
		AbstractTexture(AbstractTexture &&other) noexcept
			: identifier(std::exchange(other.identifier, 0), internalFormat(std::exchange(other.internalFormat, TextureInternalFormat::None)), format(std::exchange(other.format, TextureFormat::None)), interpolationMode(std::exchange(other.interpolationMode, TextureInterpolationMode::None)), levels(std::exchange(other.levels, 0)))
		{}

		AbstractTexture &operator=(const AbstractTexture &) = delete;
		AbstractTexture &operator=(AbstractTexture &&other) noexcept
		{
			std::swap(identifier,        other.identifier);
			std::swap(internalFormat,    other.internalFormat);
			std::swap(format,            other.format);
			std::swap(interpolationMode, other.interpolationMode);
			std::swap(levels,            other.levels);
			return *this;
		}

		void RegenerateMipmap() const { this->GetImplementation().RegenerateMipmap(); }

		inline void Bind(const int unit) const { this->GetImplementation().Bind(unit); }

		inline void BindImage(const int unit)                                                                                                  const { this->GetImplementation().BindImage(unit);                                    }
		inline void BindImage(const int unit, const int level)                                                                                 const { this->GetImplementation().BindImage(unit, level);                             }
		inline void BindImage(const int unit, const int level, const TextureAccessMode accessMode)                                             const { this->GetImplementation().BindImage(unit, level, accessMode);                 }
		inline void BindImage(const int unit, const int level, const TextureAccessMode accessMode, const TextureInternalFormat internalFormat) const { this->GetImplementation().BindImage(unit, level, accessMode, internalFormat); }
	
		static constexpr uint8_t GetNumTextureInternalFormatComponents(TextureInternalFormat internalFormat)
		{
			switch(internalFormat)
			{
				//Depth
			case TextureFormat::Depth16:
			case TextureFormat::Depth24:
			case TextureFormat::Depth32:
			case TextureFormat::Depth32F:
				return 1;
				//Depth channel and stencil channel
			case TextureFormat::Depth24Stencil8:
				return 2;
				//One channel
			case TextureFormat::X8:
			case TextureFormat::X16:
			case TextureFormat::X16F:
			case TextureFormat::X32F:
				return 1;
				//Two channels
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
			return 0;
		}

		static constexpr uint8_t GetNumTextureFormatComponents(TextureFormat format)
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
			return 0;
		}
	protected:
		AbstractTexture()
			: internalFormat(TextureInternalFormat::XYZW8), format(TextureFormat::XYZW), interpolationMode(TextureInterpolationMode::Point), levels(1)
		{}

		~AbstractTexture() = default;
	public:
		TextureInternalFormat    internalFormat;
		TextureFormat            format;
		TextureInterpolationMode interpolationMode;
		int                      levels;
	};
}