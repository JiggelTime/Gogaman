#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Device.h"
//#include "Gogaman/Core/Window.h"
//#include "Gogaman/Core/Application.h"

namespace Gogaman
{
	namespace RHI
	{
		template<typename ImplementationType>
		class AbstractTexture : public CRTP<ImplementationType, AbstractTexture>
		{
		public:
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

			enum class Interpolation : uint8_t
			{
				Point,
				Bilinear,
				Trilinear,
				Anisotropic
			};
		public:
			AbstractTexture(const AbstractTexture &) = delete;
			AbstractTexture(AbstractTexture &&other) = default;

			AbstractTexture &operator=(const AbstractTexture &) = delete;
			AbstractTexture &operator=(AbstractTexture &&other) = default;

			inline constexpr uint16_t GetWidth()  const { return m_Width;  }
			inline constexpr uint16_t GetHeight() const { return m_Height; }
			inline constexpr uint16_t GetDepth()  const { return m_Depth;  }

			inline constexpr uint8_t GetLevelCount() const { return m_LevelCount; }

			inline constexpr Interpolation GetInterpolation() const { return m_Interpolation; }

			inline constexpr Format GetFormat() const { return m_Format; }

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr auto       &GetNativeData()       { return this->GetImplementation().GetNativeData(); }

			static inline constexpr auto GetNativeFormat(const Format format) { return ImplementationType::GetNativeFormat(format); }
		private:
			AbstractTexture(const Device &device, const uint16_t width = 0, const uint16_t height = 0, const uint16_t depth = 0, const uint8_t levelCount = 1)
				: m_Width(width), m_Height(height), m_Depth(depth), m_LevelCount(levelCount)
			{
				#if GM_RHI_DEBUGGING_ENABLED
						GM_ASSERT(m_Width  < device.GetTextureWidthLimit(),  "Failed to construct texture | Width exceeds device limit");
						GM_ASSERT(m_Height < device.GetTextureHeightLimit(), "Failed to construct texture | Height exceeds device limit");
						GM_ASSERT(m_Depth  < device.GetTextureDepthLimit(),  "Failed to construct texture | Depth exceeds device limit");
				#endif
			}

			~AbstractTexture() = default;
		protected:
			uint16_t      m_Width, m_Height, m_Depth;
			uint8_t       m_LevelCount;
			Interpolation m_Interpolation;
			Format        m_Format;
		private:
			friend ImplementationType;
		};
	}
}