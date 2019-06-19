#pragma once

#include "Gogaman/CRTP.h"

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
		              Depth16,          Depth24, Depth32,         Depth32F,
		                                         Depth24Stencil8,
		X8,           X16,     X16F,                              X32F,
		XY8,          XY16,    XY16F,                             XY32F,
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
		Trilinear
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
	class AbstractTexture : public CRTP<TextureType, AbstractTexture>
	{
	public:
		AbstractTexture(const AbstractTexture &) = delete;
		AbstractTexture(AbstractTexture &&other) noexcept
			: internalFormat(std::exchange(other.internalFormat, TextureInternalFormat::None)), format(std::exchange(other.format, TextureFormat::None)), interpolationMode(std::exchange(other.interpolationMode, TextureInterpolationMode::None)), levels(std::exchange(other.levels, 0)), identifier(std::exchange(other.identifier, 0))
		{}

		AbstractTexture &operator=(const AbstractTexture &) = delete;
		AbstractTexture &operator=(AbstractTexture &&other) noexcept
		{
			std::swap(internalFormat,    other.internalFormat);
			std::swap(format,            other.format);
			std::swap(interpolationMode, other.interpolationMode);
			std::swap(levels,            other.levels);
			std::swap(identifier,        other.identifier);
			return *this;
		}

		void RegenerateMipmap() const { this->GetImplementation().RegenerateMipmap(); }

		inline void Bind(const int unit) const { this->GetImplementation().Bind(unit); }

		inline void BindImage(const int unit)                                                                                                  const { this->GetImplementation().BindImage(unit);                                    }
		inline void BindImage(const int unit, const int level)                                                                                 const { this->GetImplementation().BindImage(unit, level);                             }
		inline void BindImage(const int unit, const int level, const TextureAccessMode accessMode)                                             const { this->GetImplementation().BindImage(unit, level, accessMode);                 }
		inline void BindImage(const int unit, const int level, const TextureAccessMode accessMode, const TextureInternalFormat internalFormat) const { this->GetImplementation().BindImage(unit, level, accessMode, internalFormat); }
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
		TextureID                identifier;
	};
}