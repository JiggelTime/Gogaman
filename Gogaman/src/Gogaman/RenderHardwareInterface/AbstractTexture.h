#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Gogaman/Core/Base.h"

namespace Gogaman
{
	namespace RHI
	{
		template<typename ImplementationType>
		class AbstractTexture : public CRTP<ImplementationType, AbstractTexture>
		{
		public:
			enum class BaseFormat : uint8_t
			{
				Depth,
				DepthStencil,
				Value,
				Color
			};

			enum class Format : uint8_t
			{
				//Depth
				D16, D32F,
				//Depth and stencil
				D24S8,
				//One channel
				X8, R8, X16, X16F, X32F,
				//Two channels
				XY8, RG8, XY16, XY16F, XY32F,
				//Four channels
				XYZW8, RGBW8, XYZW16, XYZW16F, XYZW32F
			};
		public:
			AbstractTexture(const AbstractTexture &) = delete;
			AbstractTexture(AbstractTexture &&)      = default;

			AbstractTexture &operator=(const AbstractTexture &) = delete;
			AbstractTexture &operator=(AbstractTexture &&)      = default;

			inline constexpr uint16_t GetWidth()  const { return m_Width;  }
			inline constexpr uint16_t GetHeight() const { return m_Height; }
			inline constexpr uint16_t GetDepth()  const { return m_Depth;  }

			inline constexpr uint8_t GetLevelCount() const { return m_LevelCount; }

			inline constexpr Format GetFormat() const { return m_Format; }

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr auto       &GetNativeData()       { return this->GetImplementation().GetNativeData(); }

			static inline constexpr uint8_t GetFormatDataSize(const Format format)
			{
				switch(format)
				{
				case Format::D16:
					return 2;
				case Format::D32F:
					return 4;
				case Format::D24S8:
					return 4;
				case Format::X8:
					return 1;
				case Format::X16:
				case Format::X16F:
					return 2;
				case Format::X32F:
					return 4;
				case Format::XY8:
					return 2;
				case Format::XY16:
				case Format::XY16F:
					return 4;
				case Format::XY32F:
					return 8;
				case Format::XYZW8:
					return 4;
				case Format::XYZW16:
				case Format::XYZW16F:
					return 8;
				case Format::XYZW32F:
					return 16;
				case Format::R8:
					return 1;
				case Format::RG8:
					return 2;
				case Format::RGBW8:
					return 4;
				default:
					GM_DEBUG_ASSERT(false, "Failed to get texture format data size | Invalid format");
				}
			}

			static inline constexpr BaseFormat GetBaseFormat(const Format format)
			{
				switch(format)
				{
				case Format::D16:
				case Format::D32F:
					return BaseFormat::Depth;
				case Format::D24S8:
					return BaseFormat::DepthStencil;
				case Format::X8:
				case Format::X16:
				case Format::X16F:
				case Format::X32F:
				case Format::XY8:
				case Format::XY16:
				case Format::XY16F:
				case Format::XY32F:
				case Format::XYZW8:
				case Format::XYZW16:
				case Format::XYZW16F:
				case Format::XYZW32F:
					return BaseFormat::Value;
				case Format::R8:
				case Format::RG8:
				case Format::RGBW8:
					return BaseFormat::Color;
				default:
					GM_DEBUG_ASSERT(false, "Failed to get texture base format | Invalid format");
				}
			}

			static inline constexpr auto GetNativeFormat(const Format format) { return ImplementationType::GetNativeFormat(format); }
		private:
			AbstractTexture(const Format format, const uint16_t width = 1, const uint16_t height = 1, const uint16_t depth = 1, const uint8_t levelCount = 1);

			~AbstractTexture() = default;
		protected:
			uint16_t      m_Width, m_Height, m_Depth;
			uint8_t       m_LevelCount;
			Format        m_Format;
		private:
			friend ImplementationType;
		};
	}
}